#include "PhysicsSystem.h"
#include "Log.h"
#include "Memory/Memory.h"

#ifdef FLAGGG_PHYSICS
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Physics/PhysicsSystem.h>
#endif

namespace FlagGG
{
Physics::Physics()
{
#ifdef FLAGGG_PHYSICS
	joltJobSystem_ = new JPH::JobSystemThreadPool(2048, 8, 8);
	joltPhysicsSystem_ = new JPH::PhysicsSystem();
#endif
}

Physics::~Physics()
{
	if (joltPhysicsSystem_)
	{
		delete joltPhysicsSystem_;
		joltPhysicsSystem_ = nullptr;
	}
}

}
