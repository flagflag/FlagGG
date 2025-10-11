#pragma once

#if FLAGGG_JOLT
#include "Physics/JoltPhysics/PhysicsSystem.h"
#include "Physics/JoltPhysics/PhysicsScene.h"
#include "Physics/JoltPhysics/Rigid.h"
#include "Physics/JoltPhysics/RigidBody.h"
#include "Physics/JoltPhysics/RigidStatic.h"
#include "Physics/JoltPhysics/Ragdoll.h"
#elif FLAGGG_PHYSX
#include "Physics/PhysX/PhysicsSystem.h"
#include "Physics/PhysX/PhysicsScene.h"
#include "Physics/PhysX/Rigid.h"
#include "Physics/PhysX/RigidBody.h"
#include "Physics/PhysX/RigidStatic.h"
// #include "Physics/PhysX/Ragdoll.h"
#endif
