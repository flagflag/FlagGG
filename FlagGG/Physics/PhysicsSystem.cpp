#include "PhysicsSystem.h"
#include "Log.h"

#ifdef FLAGGG_PHYSICS
#include <PxPhysics.h>
#include <PxFoundation.h>
#include <PxPhysicsVersion.h>
#include <PxDeletionListener.h>
#include <common/PxTolerancesScale.h>
#include <foundation/PxAllocatorCallback.h>
#include <foundation/PxErrorCallback.h>
#endif

namespace FlagGG
{

#ifdef FLAGGG_PHYSICS
class PxAllocatorCallbackImpl : public physx::PxAllocatorCallback
{
public:
	void* allocate(size_t size, const char* typeName, const char* filename, int line) override
	{
		return ::malloc(size);
	}

	void deallocate(void* ptr) override
	{
		::free(ptr);
	}
};

class PxErrorCallbackImpl : public physx::PxErrorCallback
{
public:
	void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override
	{
		FLAGGG_LOG_ERROR("Physics error({}) msg({}) file({}) line({}).", static_cast<int>(code), message, file, line);
	}
};

class PxDeletionListenerImpl : public physx::PxDeletionListener
{
public:
	void onRelease(const physx::PxBase* observed, void* userData, physx::PxDeletionEventFlag::Enum deletionEvent) override
	{

	}
};

static PxAllocatorCallbackImpl PhysXAllocator;
static PxErrorCallbackImpl PhysXError;
#else
class PxDeletionListenerImpl {};
#endif

Physics::Physics()
{
#ifdef FLAGGG_PHYSICS
	deletionListener_ = new PxDeletionListenerImpl();
	foundation_ = PxCreateFoundation(PX_PHYSICS_VERSION, PhysXAllocator, PhysXError);
	CRY_ASSERT(foundation_);
	physics_ = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation_, physx::PxTolerancesScale(), true, nullptr);
	CRY_ASSERT(physics_);
	physics_->registerDeletionListener(*deletionListener_, physx::PxDeletionEventFlag::eUSER_RELEASE);
#endif
}

Physics::~Physics()
{

}

}
