#include "Scene/TerrainComponent.h"
#include "Scene/TerrainPatchComponent.h"
#include "Scene/Node.h"
#include "IOFrame/Buffer/StringBuffer.h"
#include "IOFrame/Buffer/IOBufferAux.h"

namespace FlagGG
{

TerrainComponent::TerrainComponent()
	: maxLodLevels_(4)
{

}

TerrainComponent::~TerrainComponent()
{

}

void TerrainComponent::SetPatchSize(UInt32 patchSize)
{
	if (patchSize_ != patchSize)
	{
		patchSize_ = patchSize;
	}
}

void TerrainComponent::SetQuadSize(const Vector3& quadSize)
{
	if (quadSize_ != quadSize)
	{
		quadSize_ = quadSize;
	}
}

void TerrainComponent::SetHeightMap(Image* image)
{
	if (heightMap_ != image)
	{
		heightMap_ = image;
	}
}

void TerrainComponent::SetMaterial(Material* material)
{
	if (material_ != material)
	{
		material_ = material;
	}
}

void TerrainComponent::CreateGeometry()
{
	if (patchSize_ == 0 || !heightMap_) return;

	UInt32 lodSize = patchSize_;
	numLodLevels_ = 1;
	while (lodSize > 4 && numLodLevels_ < maxLodLevels_)
	{
		lodSize >>= 1;
		++numLodLevels_;
	}

	patchWorldSize_ = Vector2(patchSize_ * quadSize_.x_, patchSize_ * quadSize_.y_);
	numPatches_ = IntVector2(heightMap_->GetHeight() / patchSize_, heightMap_->GetWidth() / patchSize_);
	numVertices_ = IntVector2(numPatches_.x_ * patchSize_, numPatches_.y_ * patchSize_);
	patchWorldOrigin_ = Vector2::ZERO;

	auto* ownerNode = GetNode();
	ownerNode->RemoveAllChild();

	patches_.Clear();
	patches_.Resize(numPatches_.x_ * numPatches_.y_);

	for (Int32 patchX = 0; patchX < numPatches_.x_; ++patchX)
	{
		for (Int32 patchY = 0; patchY < numPatches_.y_; ++patchY)
		{
			SharedPtr<Node> patchNode(new Node());
			ownerNode->AddChild(patchNode);
			patchNode->SetPosition(Vector3(patchX * patchSize_ * quadSize_.x_, patchY * patchSize_ * quadSize_.y_));

			TerrainPatchComponent* patch = patchNode->CreateComponent<TerrainPatchComponent>();
			patches_[patchX * numPatches_.y_ + patchY] = patch;

			CreatePatchGeometry(patch, patchX, patchY);
		}
	}
}

void TerrainComponent::CreatePatchGeometry(TerrainPatchComponent* patch, int patchX, int patchY)
{
	SharedPtr<Geometry> geometry(new Geometry());
	geometry->SetPrimitiveType(PRIMITIVE_TRIANGLE);
	geometry->SetLodDistance(0);

	SharedPtr<VertexBuffer> vertexBuffer(new VertexBuffer());
	geometry->SetVertexBuffer(0, vertexBuffer);
	SharedPtr<IndexBuffer> indexBuffer(new IndexBuffer());
	geometry->SetIndexBuffer(indexBuffer);

	static const PODVector<VertexElement> vertexElements =
	{
		VertexElement(VE_VECTOR3, SEM_POSITION, 0),
		VertexElement(VE_VECTOR3, SEM_NORMAL, 0),
		VertexElement(VE_VECTOR4, SEM_TANGENT, 0),
		VertexElement(VE_VECTOR2, SEM_TEXCOORD, 0)
	};

	vertexBuffer->SetSize((patchSize_ + 1) * (patchSize_ + 1), vertexElements);
	auto* buffer1 = vertexBuffer->LockStaticBuffer(0, vertexBuffer->GetVertexCount());

	indexBuffer->SetSize(sizeof(UInt32), patchSize_ * patchSize_ * 6);
	auto* buffer2 = indexBuffer->LockStaticBuffer(0, indexBuffer->GetIndexCount());

	// 地形高度图相关
	const unsigned char* src = heightMap_->GetData();
	unsigned imgComps = heightMap_->GetComponents();
	unsigned imgRow = heightMap_->GetWidth() * imgComps;

	BoundingBox meshBoundingBox;

	for (UInt32 x = 0; x <= patchSize_; ++x)
	{
		for (UInt32 y = 0; y <= patchSize_; ++y)
		{
			Int32 xPos = patchX * patchSize_ + x;
			Int32 yPos = patchY * patchSize_ + y;

			const Color& color = heightMap_->GetPixel(yPos, numVertices_.x_ - 1 - xPos);
			Real height = color.r_ * quadSize_.z_;

			Vector3 position(x * quadSize_.x_, y * quadSize_.y_, height);
			IOFrame::Buffer::WriteVector3(buffer1, position);
			IOFrame::Buffer::WriteVector3(buffer1, Vector3(0, 0, 1));
			IOFrame::Buffer::WriteVector4(buffer1, Vector4(0, 1, 0, 1));
			IOFrame::Buffer::WriteVector2(buffer1, Vector2(1.0f * xPos / numVertices_.x_, 1.0f * yPos / numVertices_.y_));

			meshBoundingBox.Merge(position);

			if (x != patchSize_ && y != patchSize_)
			{
				buffer2->WriteInt32(x * (patchSize_ + 1) + y);
				buffer2->WriteInt32((x + 1) * (patchSize_ + 1) + y + 1);
				buffer2->WriteInt32(x * (patchSize_ + 1) + y + 1);

				buffer2->WriteInt32(x * (patchSize_ + 1) + y);
				buffer2->WriteInt32((x + 1) * (patchSize_ + 1) + y);
				buffer2->WriteInt32((x + 1) * (patchSize_ + 1) + y + 1);
			}
		}
}

	vertexBuffer->UnlockStaticBuffer();
	indexBuffer->UnlockStaticBuffer();

	geometry->SetDataRange(0, indexBuffer->GetIndexCount());

	patch->SetGeometry(geometry);
	patch->SetMaterial(material_);
	patch->SetBoundingBox(meshBoundingBox);
	// patch->SetOcclusionCulling(false);
}

}
