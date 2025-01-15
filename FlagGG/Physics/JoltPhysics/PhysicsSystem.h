#pragma once

#include "Core/Subsystem.h"
#include "Container/Ptr.h"

namespace JPH
{

class PhysicsSystem;
class JobSystem;
class TempAllocator;

}

namespace FlagGG
{

class PxDeletionListenerImpl;

class FlagGG_API Physics : public Subsystem<Physics>
{
public:
	Physics();

	~Physics() override;

	JPH::PhysicsSystem* GetJoltPhysicsSystem() { return joltPhysicsSystem_; }

private:
	JPH::PhysicsSystem* joltPhysicsSystem_;

	JPH::JobSystem* joltJobSystem_;
};

}
