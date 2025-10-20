#include "PhysicsScene.h"
#include "Physics/PhysX/Rigid.h"
#include "Physics/PhysX/PhysicsSystem.h"

#ifdef FLAGGG_PHYSICS
#include <PxPhysics.h>
#include <PxSceneDesc.h>
#include <PxScene.h>
#include <PxRigidActor.h>
#include <foundation/PxVec3.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#endif

namespace FlagGG
{

PhysicsScene::PhysicsScene()
{
#ifdef FLAGGG_PHYSICS
	auto* physics = GetSubsystem<Physics>()->GetPxPhysics();

	physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, 0.0f, -9.81f);
	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_PCM;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_STABILIZATION;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	sceneDesc.sceneQueryUpdateMode = physx::PxSceneQueryUpdateMode::eBUILD_ENABLED_COMMIT_DISABLED;
	sceneDesc.gpuMaxNumPartitions = 8;
	pxScene_ = physics->createScene(sceneDesc);
	CRY_ASSERT(pxScene_);
#endif
}

PhysicsScene::~PhysicsScene()
{
#ifdef FLAGGG_PHYSICS
	if (pxScene_)
	{
		pxScene_->release();
		pxScene_ = nullptr;
	}
#endif
}

void PhysicsScene::AddRigid(Rigid* rigid)
{
#ifdef FLAGGG_PHYSICS
	if (rigid)
	{
		pxScene_->addActor(*rigid->rigidActor_);
	}
#endif
}

void PhysicsScene::RemoveRigid(Rigid* rigid)
{
#ifdef FLAGGG_PHYSICS
	if (rigid)
	{
		pxScene_->removeActor(*rigid->rigidActor_);
	}
#endif
}

}
