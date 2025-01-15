#pragma once

#include "Scene/Component.h"
#include "Scene/ComponentEventListener.h"
#include "Container/LinkedList.h"

namespace FlagGG
{

class FlagGG_API Rigid : public Component, public IComponentEventListener
{
	OBJECT_OVERRIDE(Rigid, Component);
public:
	Rigid();

	~Rigid() override;

protected:
// Override Component:
	// 插入场景时调用
	void OnAddToScene(Scene* scene) override;

	// 从场景删除是调用
	void OnRemoveFromScene(Scene* scene) override;

// Override IComponentEventListener:
	// 组件被插入场景
	void OnGlobalAddToScene(Scene* scene, Component* component) {}

	// 组件从场景中删除
	void OnGlobalRemoveFromScene(Scene* scene, Component* component) {}


private:
	friend class PhysicsScene;

	LinkedListNode<IComponentEventListener> listenerNode_;
};

}
