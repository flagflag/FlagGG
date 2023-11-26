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
	for (UInt32 i = 0; i < renderContexts_.Size(); ++i)
	{
		renderContexts_[i].material_ = material;
	}
}

void StaticMeshComponent::SetMaterial(UInt32 partIndex, Material* material)
{
	if (partIndex < renderContexts_.Size())
	{
		renderContexts_[partIndex].material_ = material;
	}
}

Model* StaticMeshComponent::GetModel() const
{
	return model_;
}

Material* StaticMeshComponent::GetMaterial() const
{
	return renderContexts_.Size() ? renderContexts_[0].material_ : nullptr;
}

Material* StaticMeshComponent::GetMaterial(UInt32 partIndex) const
{
	return partIndex < renderContexts_.Size() ? renderContexts_[partIndex].material_ : nullptr;
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

}
