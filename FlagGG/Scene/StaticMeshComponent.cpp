#include "Scene/StaticMeshComponent.h"
#include "Graphics/RenderEngine.h"

namespace FlagGG
{

void StaticMeshComponent::Update(Real timeStep)
{
}

void StaticMeshComponent::OnUpdateWorldBoundingBox()
{
	worldBoundingBox_ = boundingBox_.Transformed(node_->GetWorldTransform());
}

void StaticMeshComponent::ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results)
{
	switch (query.level_)
	{
	case RAY_QUERY_AABB:
		DrawableComponent::ProcessRayQuery(query, results);
		break;

	case RAY_QUERY_OBB:

		break;
	}
}

void StaticMeshComponent::SetViewMask(UInt32 viewMask)
{
	Component::SetViewMask(viewMask);

	for (UInt32 i = 0; i < renderContexts_.Size(); ++i)
	{
		renderContexts_[0].viewMask_ = GetViewMask();
	}
}

void StaticMeshComponent::SetModel(Model* model)
{
	if (model_ == model)
	{
		return;
	}

	model_ = model;

	OnModel();
}

void StaticMeshComponent::SetMaterial(Material* material)
{
	material_ = material;

	OnMaterial();
}

Model* StaticMeshComponent::GetModel()
{
	return model_;
}

Material* StaticMeshComponent::GetMaterial()
{
	return material_;
}

void StaticMeshComponent::OnModel()
{
	renderContexts_.Resize(model_->GetNumGeometries());
	for (UInt32 i = 0; i < renderContexts_.Size(); ++i)
	{
		auto& renderContext = renderContexts_[i];
		renderContext.geometryType_ = GEOMETRY_STATIC;
		renderContext.geometry_ = model_->GetGeometry(i, 0);
		renderContext.worldTransform_ = &node_->GetWorldTransform();
		renderContext.numWorldTransform_ = 1;
		renderContext.viewMask_ = GetViewMask();
	}

	boundingBox_ = model_->GetBoundingBox();
}

void StaticMeshComponent::OnMaterial()
{
	for (UInt32 i = 0; i < renderContexts_.Size(); ++i)
	{
		renderContexts_[i].material_ = material_;
	}
}

}
