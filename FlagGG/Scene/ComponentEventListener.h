//
// 组件事件监听
//

#pragma once

namespace FlagGG
{

class Node;
class Scene;
class Component;

class IComponentEventListener
{
public:
	virtual ~IComponentEventListener() {}

	// 组件被插入Node
	virtual void OnGlobalAddToNode(Component* component) = 0;

	// 组件从Node中删除
	virtual void OnGlobalRemoveFromNode(Component* component) = 0;

	// 组件被插入场景
	virtual void OnGlobalAddToScene(Scene* scene, Component* component) = 0;

	// 组件从场景中删除
	virtual void OnGlobalRemoveFromScene(Scene* scene, Component* component) = 0;
};

}
