#pragma once

#include "Physics/Rigid.h"

namespace FlagGG
{

class StaticMeshComponent;

class FlagGG_API RigidStatic : public Rigid
{
	OBJECT_OVERRIDE(RigidStatic, Rigid);
public:
	RigidStatic();

	~RigidStatic() override;

protected:
// Override IComponentEventListener:
	// 组件被插入Node
	void OnGlobalAddToNode(Component* component) override;

	// 组件从Node中删除
	void OnGlobalRemoveFromNode(Component* component) override;

private:
	// 适配的组件
	StaticMeshComponent* meshComponent_;
};

}
