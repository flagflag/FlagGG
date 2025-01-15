#include "Rigid.h"
#include "Scene/Scene.h"
#include "Physics/JoltPhysics/PhysicsScene.h"

namespace FlagGG
{

Rigid::Rigid()
	: listenerNode_(this)
{

}

Rigid::~Rigid()
{
	listenerNode_.RemoveFromList();
}

void Rigid::OnAddToScene(Scene* scene)
{
	auto* physicsScene = scene->GetComponent<PhysicsScene>();
	if (physicsScene)
	{
		physicsScene->AddRigid(this);
	}
}

void Rigid::OnRemoveFromScene(Scene* scene)
{
	auto* physicsScene = scene->GetComponent<PhysicsScene>();
	if (physicsScene)
	{
		physicsScene->RemoveRigid(this);
	}
}

}
