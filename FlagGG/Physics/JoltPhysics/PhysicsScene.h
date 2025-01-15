#pragma once

#include "Scene/Component.h"
#include "Container/Ptr.h"

namespace physx
{

class PxPhysics;
class PxFoundation;
class PxScene;

}

namespace FlagGG
{

class Rigid;

class FlagGG_API PhysicsScene : public Component
{
	OBJECT_OVERRIDE(PhysicsScene, Component);
public:
	PhysicsScene();

	~PhysicsScene() override;

	void AddRigid(Rigid* rigid);

	void RemoveRigid(Rigid* rigid);

private:
	physx::PxScene* pxScene_;
};

}
