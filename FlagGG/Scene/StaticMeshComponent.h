#pragma once

#include "Export.h"
#include "Scene/Component.h"
#include "Graphics/Model.h"
#include "Graphics/Material.h"
#include "Math/BoundingBox.h"

namespace FlagGG
{

class FlagGG_API StaticMeshComponent : public Component
{
	OBJECT_OVERRIDE(StaticMeshComponent, Component);
public:
	void Update(Real timeStep) override;

	bool IsDrawable() override;

	RenderContext* GetRenderContext() override;

	void OnUpdateWorldBoundingBox() override;

	void ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results) override;

	void SetViewMask(UInt32 viewMask) override;

	void SetModel(Model* model);

	void SetMaterial(Material* material);

	Model* GetModel();

	Material* GetMaterial();

protected:
	virtual void OnModel();

	virtual void OnMaterial();

	SharedPtr<Model> model_;

	SharedPtr<Material> material_;

	RenderContext renderContext_;

	BoundingBox boundingBox_;
};

}
