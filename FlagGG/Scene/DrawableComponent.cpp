#include "DrawableComponent.h"
#include "Scene/Octree.h"

namespace FlagGG
{

DrawableComponent::~DrawableComponent()
{
	if (ocnode_)
	{
		ocnode_->RemoveElement(this);
		ocnode_ = nullptr;
	}
}

void DrawableComponent::UpdateTreeDirty()
{
	worldBoundingBoxDirty_ = true;
}

void DrawableComponent::OnAddToScene(Scene* scene)
{
	ownerScene_ = scene;
}

void DrawableComponent::OnRemoveFromScene(Scene* scene)
{
	ownerScene_ = nullptr;
}

const BoundingBox& DrawableComponent::GetWorldBoundingBox()
{
	if (worldBoundingBoxDirty_)
	{
		OnUpdateWorldBoundingBox();
		worldBoundingBoxDirty_ = false;
	}
	return worldBoundingBox_;
}

void DrawableComponent::ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results)
{
	Real distance = query.ray_.HitDistance(GetWorldBoundingBox());
	if (distance < query.maxDistance_)
	{
		RayQueryResult ret;
		ret.distance_ = distance;
		ret.position_ = query.ray_.origin_ + distance * query.ray_.direction_;
		ret.normal_ = -query.ray_.direction_;
		ret.component_ = this;
		ret.node_ = node_;
		results.Push(ret);
	}
}

void DrawableComponent::SetOcNode(OctreeNode* ocnode)
{
	ocnode_ = ocnode;
}

void DrawableComponent::SetCastShadow(bool castShadow)
{
	castShadow_ = castShadow;
}

void DrawableComponent::SetHasLitPass(bool hasLitPass)
{
	hasLitPass_ = hasLitPass;
}

void DrawableComponent::SetHiZVisibilityTestInfo(HiZVisibilityTestInfo* HiZTestInfo)
{
	HiZTestInfo_ = HiZTestInfo;
}

}
