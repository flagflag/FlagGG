#include "Scene/Octree.h"
#include "Container/Sort.h"
#include "Scene/DrawableComponent.h"

namespace FlagGG
{

	OctreeNode::OctreeNode(const BoundingBox& box, UInt32 level) :
		box_(box),
		level_(level)
	{
		const auto& halfSize = box_.HalfSize();
		cullingBox_ = BoundingBox(box_.min_ - halfSize, box_.max_ + halfSize);
	}

OctreeNode::~OctreeNode()
{
	RemoveAllChildren();
}

void OctreeNode::Reset(const BoundingBox& box, UInt32 level)
{
	RemoveAllChildren();

	box_ = box;
	level_ = level;

	const auto& halfSize = box_.HalfSize();
	cullingBox_ = BoundingBox(box_.min_ - halfSize, box_.max_ + halfSize);
}

void OctreeNode::AddElement(DrawableComponent* component)
{
	components_.Push(component);
}

void OctreeNode::RemoveElement(DrawableComponent* component)
{
	components_.Remove(component);
}

void OctreeNode::RemoveAllChildren()
{
	for (auto& child : children_)
	{
		if (child)
		{
			delete child;
			child = nullptr;
		}
	}
}

Octree::Octree() :
	maxLevel_(8u),
	root_(BoundingBox(-1000.0f, 1000.0f), 0)
{
}

void Octree::SetSize(const BoundingBox& box, UInt32 numLevels)
{
	root_.Reset(box, 0);
	maxLevel_ = numLevels;
}

void Octree::Raycast(RayOctreeQuery& query)
{
	query.results_.Clear();
	RaycastImpl(&root_, query);
	Sort(query.results_.Begin(), query.results_.End(),
		[](const RayQueryResult& ret1, const RayQueryResult& ret2) -> bool
	{
		return ret1.distance_ < ret2.distance_;
	});
}

void Octree::RaycastImpl(OctreeNode* node, RayOctreeQuery& query)
{
	Real ocnodeDist = query.ray_.HitDistance(node->cullingBox_);
	if (ocnodeDist >= query.maxDistance_)
		return;

	for (auto comp : node->components_)
	{
		comp->ProcessRayQuery(query, query.results_);
	}

	for (auto child : node->children_)
	{
		if (child)
			RaycastImpl(child, query);
	}
}

void Octree::InsertElement(DrawableComponent* component)
{
	InsertElement(&root_, component);
}

bool Octree::CheckInsert(OctreeNode* node, const BoundingBox& box)
{
	const auto targetSize = box.Size();
	const auto halfSize = node->box_.HalfSize();
	if (node->level_ >= maxLevel_ ||
		targetSize.x_ >= halfSize.x_ ||
		targetSize.y_ >= halfSize.y_ ||
		targetSize.z_ >= halfSize.z_)
	{
		return true;
	}

	if (box.min_.x_ <= node->box_.min_.x_ - 0.5f * halfSize.x_ ||
		box.min_.y_ <= node->box_.min_.y_ - 0.5f * halfSize.y_ ||
		box.min_.z_ <= node->box_.min_.z_ - 0.5f * halfSize.z_ ||
		box.max_.x_ >= node->box_.max_.x_ + 0.5f * halfSize.x_ ||
		box.max_.y_ >= node->box_.max_.y_ + 0.5f * halfSize.y_ ||
		box.max_.z_ >= node->box_.max_.z_ + 0.5f * halfSize.z_)
	{
		return true;
	}

	return false;
}

void Octree::InsertElement(OctreeNode* node, DrawableComponent* component)
{
	const BoundingBox& box = component->GetWorldBoundingBox();
	bool insert = false;
	if (node == &root_)
		insert = node->cullingBox_.IsInside(box) != INSIDE || CheckInsert(node, box);
	else
		insert = CheckInsert(node, box);

	if (insert)
	{
		OctreeNode* oldNode = component->GetOcNode();
		if (!oldNode)
		{
			component->SetOcNode(node);
			node->AddElement(component);
		}
		else
		{
			component->SetOcNode(node);
			if (oldNode != node)
			{
				node->AddElement(component);
				oldNode->RemoveElement(component);
			}
		}
		return;
	}

	const Vector3 worldCenter = node->box_.Center();
	const Vector3 center = box.Center();
	UInt32 x = center.x_ < worldCenter.x_ ? 0 : 1;
	UInt32 y = center.y_ < worldCenter.y_ ? 0 : 2;
	UInt32 z = center.z_ < worldCenter.z_ ? 0 : 4;
	InsertElement(GetOrCreateChild(node, x | y | z), component);
}

OctreeNode* Octree::GetOrCreateChild(OctreeNode* node, UInt32 index)
{
	OctreeNode*& child = node->children_[index];
			
	if (!child)
	{
		Vector3 min = node->box_.min_;
		Vector3 max = node->box_.max_;
		Vector3 center = node->box_.Center();

		if (index & 1u)
			min.x_ = center.x_;
		else
			max.x_ = center.x_;

		if (index & 2u)
			min.y_ = center.y_;
		else
			max.y_ = center.y_;

		if (index & 4u)
			min.z_ = center.z_;
		else
			max.z_ = center.z_;

		child = new OctreeNode(BoundingBox(min, max), node->level_ + 1);
	}

	return child;
}

}
