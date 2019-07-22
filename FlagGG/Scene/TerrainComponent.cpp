#include "Scene/TerrainComponent.h"
#include "Scene/Node.h"
#include "IOFrame/Buffer/StringBuffer.h"
#include "IOFrame/Buffer/IOBufferAux.h"

namespace FlagGG
{
	namespace Scene
	{
		void TerrainComponent::SetPathSize(uint32_t pathSize)
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

				renderContext_.texture_ = nullptr;
				for (uint32_t i = 0; i < MAX_TEXTURE_CLASS; ++i)
				{
					renderContext_.textures_.Push(material_->GetTexture(i));
				}
				renderContext_.vertexShader_ = material_->GetVertexShader();
				renderContext_.pixelShader_ = material_->GetPixelShader();
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
			uint32_t vertexDataSize = vertexBuffer_->GetVertexCount() * vertexBuffer_->GetVertexSize();
			auto* vertexData = vertexBuffer_->Lock(0, vertexDataSize);
			IOFrame::Buffer::StringBuffer buffer1(vertexData, vertexDataSize);

			indexBuffer_->SetSize(sizeof(uint32_t), (verticesNum_.x_ - 1) * (verticesNum_.y_ - 1) * 6);
			uint32_t indexDataSize = indexBuffer_->GetIndexCount() * indexBuffer_->GetIndexSize();
			auto* indexData = indexBuffer_->Lock(0, indexDataSize);
			IOFrame::Buffer::StringBuffer buffer2(indexData, indexDataSize);

			for (uint32_t x = 0; x < verticesNum_.x_; ++x)
			{
				for (uint32_t y = 0; y < verticesNum_.y_; ++y)
				{
					IOFrame::Buffer::WriteVector3(&buffer1, Math::Vector3(x, rand() % 3, y));
					IOFrame::Buffer::WriteVector3(&buffer1, Math::Vector3(1, 1, 1));
					IOFrame::Buffer::WriteVector2(&buffer1, Math::Vector2(x, y));

					if (x != verticesNum_.x_ - 1 && y != verticesNum_.y_ - 1)
					{
						buffer2.WriteInt32(x * verticesNum_.x_ + y);
						buffer2.WriteInt32(x * verticesNum_.x_ + y + 1);
						buffer2.WriteInt32((x + 1) * verticesNum_.x_ + y + 1);

						buffer2.WriteInt32(x * verticesNum_.x_ + y);
						buffer2.WriteInt32((x + 1) * verticesNum_.x_ + y + 1);
						buffer2.WriteInt32((x + 1) * verticesNum_.x_ + y);
					}
				}
			}

			vertexBuffer_->Unlock();
			indexBuffer_->Unlock();

			geometry_->SetDataRange(0, indexBuffer_->GetIndexCount());

			renderContext_.geometryType_ = GEOMETRY_STATIC;
			renderContext_.geometries_.Clear();
			renderContext_.geometries_.Push(geometry_);
			renderContext_.numWorldTransform_ = 1;
			renderContext_.worldTransform_ = &node_->GetWorldTransform();
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
