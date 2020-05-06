#pragma once

#include "Export.h"
#include "Scene/Component.h"
#include "Container/Ptr.h"
#include "Graphics/Material.h"
#include "Math/Vector2.h"
#include "Math/Matrix3x4.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{
	namespace Scene
	{
		class FlagGG_API DynamicMeshComponent : public Component
		{
			OBJECT_OVERRIDE(DynamicMeshComponent, Component);
		public:
			DynamicMeshComponent();

			void SetSize(UInt32 width, UInt32 height);

			void SetElementSize(Real elementSize);

			void SetMaterial(Graphics::Material* material);

			bool IsDrawable() override;

			Graphics::RenderContext* GetRenderContext() override;

			void Update(Real timeStep) override;

			void SetViewMask(UInt32 viewMask) override;

			virtual Real GetHeight(Int32 x, Int32 y, Int32 verticesNumX, Int32 verticesNumY);

			virtual Math::Vector3 GetNormal(Int32 x, Int32 y, Int32 verticesNumX, Int32 verticesNumY);

			virtual Math::Vector2 GetUV(Int32 x, Int32 y, Int32 verticesNumX, Int32 verticesNumY);

			virtual const Container::PODVector<VertexElement>& GetVertexElement();

		protected:
			void CreateGeometry();

		private:
			UInt32 width_;
			UInt32 height_;
			Real elementSize_;

			Container::SharedPtr<Graphics::Material> material_;
			Container::SharedPtr<Graphics::Geometry> geometry_;
			Container::SharedPtr<Graphics::VertexBuffer> vertexBuffer_;
			Container::SharedPtr<Graphics::IndexBuffer> indexBuffer_;

			Graphics::RenderContext renderContext_;

			bool geometryDirty_;
			bool hasGeometry_;
		};
	}
}
