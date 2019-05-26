#pragma once

#include "Export.h"
#include "Scene/Component.h"
#include "Graphics/Model.h"
#include "Graphics/Material.h"

namespace FlagGG
{
	namespace Scene
	{
		class FlagGG_API StaticMeshComponent : public Component
		{
		public:
			void Update(float timeStep) override;

			void Render(Graphics::RenderContext& renderContext) override;

			virtual void SetModel(Graphics::Model* model);

			void SetMaterial(Graphics::Material* material);

		protected:
			Container::SharedPtr<Graphics::Model> model_;

			Container::SharedPtr<Graphics::Material> material_;
		};
	}
}
