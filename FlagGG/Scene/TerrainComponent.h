#pragma once

#include "Export.h"
#include "Scene/Component.h"
#include "Container/Ptr.h"
#include "Graphics/Material.h"
#include "Resource/Image.h"
#include "Math/Vector2.h"
#include "Math/Matrix3x4.h"

namespace FlagGG
{
	namespace Scene
	{
		class FlagGG_API TerrainComponent : public Component
		{
			OBJECT_OVERRIDE(TerrainComponent)
		public:
			void SetPathSize(uint32_t pathSize);

			void SetHeightMap(Resource::Image* image);

			void SetMaterial(Graphics::Material* material);

			bool IsDrawable() override;

			Graphics::RenderContext* GetRenderContext() override;

		protected:
			void CreateGeometry();

		private:
			uint32_t pathSize_;

			Math::IntVector2 patchesNum_;
			Math::IntVector2 verticesNum_;
			Math::Vector2 patchWorldSize_;
			Math::Vector2 patchWorldOrigin_;
			Math::Matrix3x4 indentity_;

			Container::SharedPtr<Resource::Image> heightMap_;
			Container::SharedPtr<Graphics::Material> material_;
			Container::SharedPtr<Graphics::Geometry> geometry_;
			Container::SharedPtr<Graphics::VertexBuffer> vertexBuffer_;
			Container::SharedPtr<Graphics::IndexBuffer> indexBuffer_;

			Graphics::RenderContext renderContext_;
		};
	}
}
