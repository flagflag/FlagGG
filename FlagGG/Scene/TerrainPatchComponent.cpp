#include "TerrainPatchComponent.h"
#include "Scene/Node.h"

namespace FlagGG
{

void TerrainPatchComponent::SetMaterial(Material* material)
{
	if (material_ != material)
	{
		material_ = material;

		if (renderContexts_.Size())
		{
			auto& renderContext = renderContexts_[0];
			renderContext.material_ = material_;
		}
	}
}

void TerrainPatchComponent::SetGeometry(Geometry* geometry)
{
	geometry_ = geometry;

	renderContexts_.Resize(1);
	auto& renderContext = renderContexts_[0];
	renderContext.geometryType_ = GEOMETRY_STATIC;
	renderContext.geometry_ = geometry_;
	renderContext.numWorldTransform_ = 1;
	renderContext.worldTransform_ = &node_->GetWorldTransform();
	renderContext.viewMask_ = GetViewMask();
}

void TerrainPatchComponent::SetBoundingBox(const BoundingBox& aabb)
{
	meshBoundingBox_ = aabb;

	if (meshBoundingBox_.min_.z_ == meshBoundingBox_.max_.z_)
		meshBoundingBox_.max_.z_++;
}

void TerrainPatchComponent::OnUpdateWorldBoundingBox()
{
	worldBoundingBox_ = meshBoundingBox_.Transformed(node_->GetWorldTransform());
}

}
