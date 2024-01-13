#include "AllocatorLJ.h"
#include "LJAlloc/lj_alloc.h"
#include "AsyncFrame/Locker.h"

namespace FlagGG
{

AllocatorLJInstance::AllocatorLJInstance()
{
	PRNGState_ = new PRNGState();
	PRNGState_->label = 0;
	alloctorHandle_ = lj_alloc_create(PRNGState_);
}

AllocatorLJInstance::~AllocatorLJInstance()
{
	lj_alloc_destroy(alloctorHandle_);
	delete PRNGState_;
}

void* AllocatorLJInstance::Malloc(USize size)
{
	Locker _(mutex_);
	return lj_alloc_malloc(alloctorHandle_, size);
}

void* AllocatorLJInstance::Realloc(void* ptr, USize size)
{
	Locker _(mutex_);
	return lj_alloc_realloc(alloctorHandle_, ptr, size);
}

void AllocatorLJInstance::Free(void* ptr)
{
	Locker _(mutex_);
	lj_alloc_free(alloctorHandle_, ptr);
}

}
