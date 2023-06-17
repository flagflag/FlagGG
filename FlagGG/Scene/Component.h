//
// Component基类
//

#pragma once

#include "Export.h"
#include "Core/Object.h"
#include "Math/BoundingBox.h"
#include "Scene/OctreeQuery.h"
#include "Container/Ptr.h"

namespace FlagGG
{

class Node;

class FlagGG_API Component : public Object
{
	OBJECT_OVERRIDE(Component, Object);
public:
	~Component() override;

	// 帧更新
	virtual void Update(Real timeStep) {}

	// 控件树发生dirty
	virtual void UpdateTreeDirty() {}

	// 插入node时调用
	virtual void OnAddToNode(Node* node) {}

	// 从node删除时调用
	virtual void OnRemoveFromNode(Node* node) {}

	// 设置场景节点
	void SetNode(Node* node);

	// 设置相机可见性Mask
	virtual void SetViewMask(UInt32 viewMask);

	// 获取场景节点
	Node* GetNode() const { return node_; }

	// 获取相机可见性mask
	UInt32 GetViewMask() const { return viewMask_; }

protected:
	WeakPtr<Node> node_;

	UInt32 viewMask_{F_MAX_UNSIGNED};
};

}
