#include "Scene/TerrainComponent.h"
#include "Scene/Node.h"
#include "IOFrame/Buffer/StringBuffer.h"
#include "IOFrame/Buffer/IOBufferAux.h"

namespace FlagGG
{
	namespace Scene
	{
		void TerrainComponent::SetPathSize(UInt32 pathSize)
		{
			if (pathSize_ != pathSize)
			{
				pathSize_ = pathSize;

				CreateGeometry();
			}
		}

		void TerrainComponent::SetHeightMap(Resource::Image* image)
		{
			if (heightMap_ != image)
			{
				heightMap_ = image;

				CreateGeometry();
			}
		}

		void TerrainComponent::SetMaterial(Graphics::Material* material)
		{
			if (material_ != material)
			{
				material_ = material;

				for (UInt32 i = 0; i < MAX_TEXTURE_CLASS; ++i)
				{
					renderContext_.textures_.Push(material_->GetTexture(i));
				}
				renderContext_.vertexShader_ = material_->GetVertexShader();
				renderContext_.pixelShader_ = material_->GetPixelShader();
				renderContext_.renderPass_ = &material_->GetRenderPass();
				renderContext_.shaderParameters_ = material_->GetShaderParameters();
				renderContext_.rasterizerState_ = material_->GetRasterizerState();
			}
		}

		void TerrainComponent::CreateGeometry()
		{
			if (pathSize_ == 0 || !heightMap_) return;

			patchWorldSize_ = Math::Vector2(pathSize_, pathSize_);
			patchesNum_ = Math::IntVector2(heightMap_->GetWidth() / pathSize_, heightMap_->GetHeight() / pathSize_);
			verticesNum_ = Math::IntVector2(patchesNum_.x_ * pathSize_, patchesNum_.y_ * pathSize_);
			patchWorldOrigin_ = Math::Vector2(-0.5f * patchesNum_.x_ * patchWorldSize_.x_, -0.5f * patchesNum_.y_ * patchWorldSize_.y_);

			geometry_ = new Graphics::Geometry();
			geometry_->SetPrimitiveType(PRIMITIVE_TRIANGLE);
			geometry_->SetLodDistance(0);

			if (!vertexBuffer_)
			{
				vertexBuffer_ = new Graphics::VertexBuffer();
			}
			geometry_->SetVertexBuffer(0, vertexBuffer_);

			if (!indexBuffer_)
			{
				indexBuffer_ = new Graphics::IndexBuffer();
			}
			geometry_->SetIndexBuffer(indexBuffer_);

			static const Container::PODVector<VertexElement> vertexElements =
			{
				VertexElement(VE_VECTOR3, SEM_POSITION, 0),
				VertexElement(VE_VECTOR3, SEM_NORMAL, 0),
				VertexElement(VE_VECTOR2, SEM_TEXCOORD, 0)
			};

			vertexBuffer_->SetSize(verticesNum_.x_ * verticesNum_.y_, vertexElements);
			UInt32 vertexDataSize = vertexBuffer_->GetVertexCount() * vertexBuffer_->GetVertexSize();
			auto* buffer1 = vertexBuffer_->LockStaticBuffer(0, vertexDataSize);

			indexBuffer_->SetSize(sizeof(UInt32), (verticesNum_.x_ - 1) * (verticesNum_.y_ - 1) * 6);
			UInt32 indexDataSize = indexBuffer_->GetIndexCount() * indexBuffer_->GetIndexSize();
			auto* buffer2 = indexBuffer_->LockStaticBuffer(0, indexDataSize);

			// 地形高度图相关
			const unsigned char* src = heightMap_->GetData();
			unsigned imgComps = heightMap_->GetComponents();
			unsigned imgRow = heightMap_->GetWidth() * imgComps;

			for (UInt32 x = 0; x < verticesNum_.x_; ++x)
			{
				for (UInt32 y = 0; y < verticesNum_.y_; ++y)
				{
					Real height = src[imgRow * (verticesNum_.x_ - 1 - x) + y];
					IOFrame::Buffer::WriteVector3(buffer1, Math::Vector3(x, height, y));
					IOFrame::Buffer::WriteVector3(buffer1, Math::Vector3(1, 1, 1));
					IOFrame::Buffer::WriteVector2(buffer1, Math::Vector2(1.0f * x / verticesNum_.x_, 1.0f * y / verticesNum_.y_));

					if (x != verticesNum_.x_ - 1 && y != verticesNum_.y_ - 1)
					{
						buffer2->WriteInt32(x * verticesNum_.y_ + y);
						buffer2->WriteInt32(x * verticesNum_.y_ + y + 1);
						buffer2->WriteInt32((x + 1) * verticesNum_.y_ + y + 1);

						buffer2->WriteInt32(x * verticesNum_.y_ + y);
						buffer2->WriteInt32((x + 1) * verticesNum_.y_ + y + 1);
						buffer2->WriteInt32((x + 1) * verticesNum_.y_ + y);
					}
				}
			}

			vertexBuffer_->UnlockStaticBuffer();
			indexBuffer_->UnlockStaticBuffer();

			geometry_->SetDataRange(0, indexBuffer_->GetIndexCount());

			renderContext_.geometryType_ = GEOMETRY_STATIC;
			renderContext_.geometries_.Clear();
			renderContext_.geometries_.Push(geometry_);
			renderContext_.numWorldTransform_ = 1;
			renderContext_.worldTransform_ = &node_->GetWorldTransform();
			renderContext_.viewMask_ = GetViewMask();
		}

		bool TerrainComponent::IsDrawable()
		{
			return true;
		}

		Graphics::RenderContext* TerrainComponent::GetRenderContext()
		{
			return &renderContext_;
		}
	}
}
