#include "Scene/DynamicMeshComponent.h"
#include "Scene/Node.h"
#include "IOFrame/Buffer/StringBuffer.h"
#include "IOFrame/Buffer/IOBufferAux.h"

namespace FlagGG
{
	namespace Scene
	{
		DynamicMeshComponent::DynamicMeshComponent() :
			width_(0u),
			height_(0u),
			elementSize_(0.0f),
			geometryDirty_(false),
			hasGeometry_(false)
		{}

		void DynamicMeshComponent::SetSize(UInt32 width, UInt32 height)
		{
			if (width_ != width || height_ != height)
			{
				width_ = width;
				height_ = height;

				geometryDirty_ = true;
			}
		}

		void DynamicMeshComponent::SetElementSize(Real elementSize)
		{
			if (elementSize_ != elementSize)
			{
				elementSize_ = elementSize;

				geometryDirty_ = true;
			}
		}

		void DynamicMeshComponent::SetMaterial(Graphics::Material* material)
		{
			material_ = material;

			renderContext_.textures_.Clear();
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

		void DynamicMeshComponent::CreateGeometry()
		{
			if (width_ == 0u || height_ == 0u || Math::Equals(elementSize_, 0.0f))
				return;

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

			const auto& vertexElements = GetVertexElement();

			vertexBuffer_->SetSize(width_ * height_, vertexElements);
			UInt32 vertexDataSize = vertexBuffer_->GetVertexCount() * vertexBuffer_->GetVertexSize();
			auto* buffer1 = vertexBuffer_->LockStaticBuffer(0, vertexDataSize);

			indexBuffer_->SetSize(sizeof(UInt32), (width_ - 1) * (height_ - 1) * 6);
			UInt32 indexDataSize = indexBuffer_->GetIndexCount() * indexBuffer_->GetIndexSize();
			auto* buffer2 = indexBuffer_->LockStaticBuffer(0, indexDataSize);

			for (UInt32 x = 0u; x < width_; ++x)
			{
				for (UInt32 y = 0u; y < height_; ++y)
				{
					Real height = GetHeight(x, y, width_, height_);
					IOFrame::Buffer::WriteVector3(buffer1, Math::Vector3((Real)x - (Real)width_ / 2, height, (Real)y - (Real)height_ / 2) * elementSize_);
					IOFrame::Buffer::WriteVector3(buffer1, GetNormal(x, y, width_, height_));
					IOFrame::Buffer::WriteVector2(buffer1, GetUV(x, y, width_, height_));

					if (x != width_ - 1 && y != height_ - 1)
					{
						buffer2->WriteInt32(x * height_ + y);
						buffer2->WriteInt32(x * height_ + y + 1);
						buffer2->WriteInt32((x + 1) * height_ + y + 1);

						buffer2->WriteInt32(x * height_ + y);
						buffer2->WriteInt32((x + 1) * height_ + y + 1);
						buffer2->WriteInt32((x + 1) * height_ + y);
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

			hasGeometry_ = true;
		}

		bool DynamicMeshComponent::IsDrawable()
		{
			return hasGeometry_;
		}

		Graphics::RenderContext* DynamicMeshComponent::GetRenderContext()
		{
			return &renderContext_;
		}

		void DynamicMeshComponent::Update(Real timeStep)
		{
			if (geometryDirty_)
			{
				geometryDirty_ = false;

				CreateGeometry();
			}
		}

		void DynamicMeshComponent::SetViewMask(UInt32 viewMask)
		{
			Component::SetViewMask(viewMask);

			renderContext_.viewMask_ = GetViewMask();
		}

		Real DynamicMeshComponent::GetHeight(Int32 x, Int32 y, Int32 verticesNumX, Int32 verticesNumY)
		{
			return 1.0;
		}

		Math::Vector3 DynamicMeshComponent::GetNormal(Int32 x, Int32 y, Int32 verticesNumX, Int32 verticesNumY)
		{
			return Math::Vector3(0.0, 1.0f, 0.0f);
		}

		Math::Vector2 DynamicMeshComponent::GetUV(Int32 x, Int32 y, Int32 verticesNumX, Int32 verticesNumY)
		{
			return Math::Vector2(1.0f * x / verticesNumX, 1.0f * y / verticesNumY);
		}

		const Container::PODVector<VertexElement>& DynamicMeshComponent::GetVertexElement()
		{
			static const Container::PODVector<VertexElement> vertexElements =
			{
				VertexElement(VE_VECTOR3, SEM_POSITION, 0),
				VertexElement(VE_VECTOR3, SEM_NORMAL, 0),
				VertexElement(VE_VECTOR2, SEM_TEXCOORD, 0)
			};
			return vertexElements;
		}
	}
}
