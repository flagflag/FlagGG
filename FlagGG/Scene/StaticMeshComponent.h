#pragma once

#include "Export.h"
#include "Scene/Component.h"
#include "Graphics/Model.h"
#include "Graphics/Material.h"
#include "Math/BoundingBox.h"

namespace FlagGG
{
	namespace Scene
	{
		class FlagGG_API StaticMeshComponent : public Component
		{
			OBJECT_OVERRIDE(StaticMeshComponent);
		public:
			void Update(Real timeStep) override;

			bool IsDrawable() override;

			Graphics::RenderContext* GetRenderContext() override;

			void OnUpdateWorldBoundingBox() override;

			void ProcessRayQuery(const RayOctreeQuery& query, Container::PODVector<RayQueryResult>& results) override;

			void SetViewMask(UInt32 viewMask) override;

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

			Math::BoundingBox boundingBox_;
		};
	}
}
