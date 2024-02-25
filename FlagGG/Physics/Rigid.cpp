#include "Rigid.h"
#include "Physics/PhysicsScene.h"
#include "Scene/Scene.h"

namespace FlagGG
{

Rigid::Rigid()
	: listenerNode_(this)
	, rigidActor_(nullptr)
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
