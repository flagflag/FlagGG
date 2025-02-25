#include "Scene/TerrainComponent.h"
#include "Scene/Node.h"
#include "IOFrame/Buffer/StringBuffer.h"
#include "IOFrame/Buffer/IOBufferAux.h"

namespace FlagGG
{

void TerrainComponent::SetPatchSize(UInt32 patchSize)
{
	if (patchSize_ != patchSize)
	{
		patchSize_ = patchSize;

		CreateGeometry();
	}
}

void TerrainComponent::SetQuadSize(UInt32 quadSize)
{
	if (quadSize_ != quadSize)
	{
		quadSize_ = quadSize;

		CreateGeometry();
	}
}

void TerrainComponent::SetHeightMap(Image* image)
{
	if (heightMap_ != image)
	{
		heightMap_ = image;

		CreateGeometry();
	}
}

void TerrainComponent::SetMaterial(Material* material)
{
	if (material_ != material)
	{
		material_ = material;

		if (renderContexts_.Size())
		{
			auto& renderContext = renderContexts_[0];
			renderContext.material_ = material_;
		}
	}
}

void TerrainComponent::CreateGeometry()
{
	if (patchSize_ == 0 || !heightMap_) return;

	meshBoundingBox_.Clear();

	patchWorldSize_ = Vector2(patchSize_, patchSize_);
	patchesNum_ = IntVector2(heightMap_->GetHeight() / patchSize_, heightMap_->GetWidth() / patchSize_);
	verticesNum_ = IntVector2(patchesNum_.x_ * patchSize_, patchesNum_.y_ * patchSize_);
	patchWorldOrigin_ = Vector2(-0.5f * patchesNum_.x_ * patchWorldSize_.x_, -0.5f * patchesNum_.y_ * patchWorldSize_.y_);

	geometry_ = new Geometry();
	geometry_->SetPrimitiveType(PRIMITIVE_TRIANGLE);
	geometry_->SetLodDistance(0);

	if (!vertexBuffer_)
	{
		vertexBuffer_ = new VertexBuffer();
	}
	geometry_->SetVertexBuffer(0, vertexBuffer_);

	if (!indexBuffer_)
	{
		indexBuffer_ = new IndexBuffer();
	}
	geometry_->SetIndexBuffer(indexBuffer_);

	static const PODVector<VertexElement> vertexElements =
	{
		VertexElement(VE_VECTOR3, SEM_POSITION, 0),
		VertexElement(VE_VECTOR3, SEM_NORMAL, 0),
		VertexElement(VE_VECTOR4, SEM_TANGENT, 0),
		VertexElement(VE_VECTOR2, SEM_TEXCOORD, 0)
	};

	vertexBuffer_->SetSize(verticesNum_.x_ * verticesNum_.y_, vertexElements);
	auto* buffer1 = vertexBuffer_->LockStaticBuffer(0, vertexBuffer_->GetVertexCount());

	indexBuffer_->SetSize(sizeof(UInt32), (verticesNum_.x_ - 1) * (verticesNum_.y_ - 1) * 6);
	auto* buffer2 = indexBuffer_->LockStaticBuffer(0, indexBuffer_->GetIndexCount());

	// 地形高度图相关
	const unsigned char* src = heightMap_->GetData();
	unsigned imgComps = heightMap_->GetComponents();
	unsigned imgRow = heightMap_->GetWidth() * imgComps;

	for (UInt32 x = 0; x < verticesNum_.x_; ++x)
	{
		for (UInt32 y = 0; y < verticesNum_.y_; ++y)
		{
#if 0
			// Real height = src[imgRow * (verticesNum_.x_ - 1 - x) + y];
#else
			const Color& color = heightMap_->GetPixel(y, verticesNum_.x_ - 1 - x);
			Real height = color.r_ * 255.0f;
#endif
			Vector3 position(x, y, height);
			IOFrame::Buffer::WriteVector3(buffer1, position);
			IOFrame::Buffer::WriteVector3(buffer1, Vector3(0, 0, 1));
			IOFrame::Buffer::WriteVector4(buffer1, Vector4(0, 1, 0, 1));
			IOFrame::Buffer::WriteVector2(buffer1, Vector2(1.0f * x / verticesNum_.x_, 1.0f * y / verticesNum_.y_));

			meshBoundingBox_.Merge(position);

			if (x != verticesNum_.x_ - 1 && y != verticesNum_.y_ - 1)
			{
				buffer2->WriteInt32(x * verticesNum_.y_ + y);
				buffer2->WriteInt32((x + 1) * verticesNum_.y_ + y + 1);
				buffer2->WriteInt32(x * verticesNum_.y_ + y + 1);

				buffer2->WriteInt32(x * verticesNum_.y_ + y);
				buffer2->WriteInt32((x + 1) * verticesNum_.y_ + y);
				buffer2->WriteInt32((x + 1) * verticesNum_.y_ + y + 1);
			}
		}
	}

	vertexBuffer_->UnlockStaticBuffer();
	indexBuffer_->UnlockStaticBuffer();

	geometry_->SetDataRange(0, indexBuffer_->GetIndexCount());

	renderContexts_.Resize(1);
	auto& renderContext = renderContexts_[0];
	renderContext.geometryType_ = GEOMETRY_STATIC;
	renderContext.geometry_ = geometry_;
	renderContext.numWorldTransform_ = 1;
	renderContext.worldTransform_ = &node_->GetWorldTransform();
	renderContext.viewMask_ = GetViewMask();

	if (meshBoundingBox_.min_.z_ == meshBoundingBox_.max_.z_)
		meshBoundingBox_.max_.z_++;
}

void TerrainComponent::OnUpdateWorldBoundingBox()
{
	worldBoundingBox_ = meshBoundingBox_.Transformed(node_->GetWorldTransform());
}

}
