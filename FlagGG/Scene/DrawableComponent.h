//
// 渲染对象控件
//

#pragma once

#include "Scene/Component.h"

namespace FlagGG
{

class OctreeNode;

class FlagGG_API DrawableComponent : public Component
{
	OBJECT_OVERRIDE(DrawableComponent, Component);
public:
	~DrawableComponent() override;

	void UpdateTreeDirty() override;

	// 是否可渲染
	virtual bool IsRenderable() { return false; }

	// 更新包围盒
	virtual void OnUpdateWorldBoundingBox() {}

	// 获取渲染上下文
	virtual RenderContext* GetRenderContext() { return nullptr; }

	// 设置查询
	virtual void ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results);

	// 设置octree node
	void SetOcNode(OctreeNode* ocnode);

	// 获取世界空间下的包围盒
	const BoundingBox& GetWorldBoundingBox();

	// 获取octree node
	OctreeNode* GetOcNode();

protected:
	BoundingBox worldBoundingBox_;

	bool worldBoundingBoxDirty_{ true };

	OctreeNode* ocnode_{ nullptr };
};

}
