//
// 八叉树
//

#pragma once

#include "Export.h"
#include "Define.h"
#include "Container/Vector.h"
#include "Container/Allocator.h"
#include "Scene/Component.h"
#include "Scene/OctreeQuery.h"
#include "Math/BoundingBox.h"

namespace FlagGG
{

class DrawableComponent;

// 八叉树节点
struct FlagGG_API OctreeNode
{
	explicit OctreeNode(const BoundingBox& box, UInt32 level);

	~OctreeNode();

	// 重置八叉树节点
	void Reset(const BoundingBox& box, UInt32 level);

	// 插入component
	void AddElement(DrawableComponent* component);

	// 删除component
	void RemoveElement(DrawableComponent* component);

	// 删除所有children
	void RemoveAllChildren();

	OctreeNode* children_[8]{ nullptr };

	UInt32 level_;
	BoundingBox box_;
	BoundingBox cullingBox_;
	PODVector<DrawableComponent*> components_;
};

// 八叉树
class FlagGG_API Octree : public Component
{
public:
	explicit Octree();

	~Octree() override = default;

	// 设置八叉树范围和层数
	void SetSize(const BoundingBox& box, UInt32 numLevels);

	// 射线查询
	void Raycast(RayOctreeQuery& query) const;

	// 用query获取elments
	void GetElements(OctreeQuery& query) const;

	// 插入八叉树节点
	void InsertElement(DrawableComponent* component);

	// 删除八叉树节点
	void RemoveElement(DrawableComponent* component);

	// 删除所以八叉树节点
	void RemoveAllElement();

protected:
	// 检查ndoe能否插入
	bool CheckInsert(OctreeNode* node, const BoundingBox& box);

	// 将component插入node节点下
	void InsertElement(OctreeNode* node, DrawableComponent* component);

	// 获取or创建节点
	OctreeNode* GetOrCreateChild(OctreeNode* node, UInt32 index);

	// 射线查询实现
	void RaycastImpl(const OctreeNode* node, RayOctreeQuery& query) const;

	// 用query获取elments
	void GetElementsImpl(const OctreeNode* node, OctreeQuery& query, bool inside) const;

private:
	OctreeNode root_;

	UInt32 maxLevel_;

	Allocator<OctreeNode> nodeAllocator_;
};

}
