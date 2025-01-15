#pragma once

#include "Core/Subsystem.h"
#include "Container/Ptr.h"

namespace physx
{

class PxPhysics;
class PxFoundation;

}

namespace FlagGG
{

class PxDeletionListenerImpl;

class FlagGG_API Physics : public Subsystem<Physics>
{
public:
	Physics();

	~Physics() override;

	physx::PxPhysics* GetPxPhysics() const { return physics_; }

	physx::PxFoundation* GetPxFoundation() const { return foundation_; }

private:
	physx::PxPhysics* physics_;

	physx::PxFoundation* foundation_;

	UniquePtr<PxDeletionListenerImpl> deletionListener_;
};

}
