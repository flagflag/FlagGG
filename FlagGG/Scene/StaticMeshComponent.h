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
			OBJECT_OVERRIDE(StaticMeshComponent);
		public:
			void Update(float timeStep) override;

			bool IsDrawable() override;

			Graphics::RenderContext* GetRenderContext() override;

			void SetModel(Graphics::Model* model);

			void SetMaterial(Graphics::Material* material);

			Graphics::Model* GetModel();

			Graphics::Material* GetMaterial();

		protected:
			virtual void OnModel();

			virtual void OnMaterial();

			Container::SharedPtr<Graphics::Model> model_;

			Container::SharedPtr<Graphics::Material> material_;

			Graphics::RenderContext renderContext_;
		};
	}
}
