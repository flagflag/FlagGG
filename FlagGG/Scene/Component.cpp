#include "Scene/Component.h"
#include "Scene/Node.h"
#include "Scene/Octree.h"

namespace FlagGG
{

Component::~Component()
{
	if (ocnode_)
	{
		ocnode_->RemoveElement(this);
		ocnode_ = nullptr;
	}
}

void Component::UpdateTreeDirty()
{
	worldBoundingBoxDirty_ = true;
}

void Component::ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results)
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

const BoundingBox& Component::GetWorldBoundingBox()
{
	if (worldBoundingBoxDirty_)
	{
		OnUpdateWorldBoundingBox();
		worldBoundingBoxDirty_ = false;
	}
	return worldBoundingBox_;
}

void Component::SetNode(Node* node)
{
	node_ = node;
}

Node* Component::GetNode() const
{
	return node_;
}

void Component::SetOcNode(OctreeNode* ocnode)
{
	ocnode_ = ocnode;
}

OctreeNode* Component::GetOcNode()
{
	return ocnode_;
}

void Component::SetViewMask(UInt32 viewMask)
{
	viewMask_ = viewMask;
}

UInt32 Component::GetViewMask() const
{
	return viewMask_;
}

}

