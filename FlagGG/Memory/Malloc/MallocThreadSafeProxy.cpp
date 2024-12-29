#include "MallocThreadSafeProxy.h"
#include "AsyncFrame/Locker.h"

namespace FlagGG
{

MallocThreadSafeProxy::MallocThreadSafeProxy(IMalloc* malloc)
	: malloc_(malloc)
{

}

void* MallocThreadSafeProxy::Malloc(USize size, UInt32 alignment)
{
	Locker _(mutex_);
	return malloc_->Malloc(size, alignment);
}

void* MallocThreadSafeProxy::Realloc(void* originPtr, USize newSize, UInt32 alignment)
{
	Locker _(mutex_);
	return malloc_->Realloc(originPtr, newSize, alignment);
}

void MallocThreadSafeProxy::Free(void* ptr)
{
	Locker _(mutex_);
	malloc_->Free(ptr);
}

bool MallocThreadSafeProxy::GetAllocationSize(void* ptr, USize& outSize)
{
	Locker _(mutex_);
	return malloc_->GetAllocationSize(ptr, outSize);
}

void MallocThreadSafeProxy::Trim(bool trimThreadCache)
{
	Locker _(mutex_);
	malloc_->Trim(trimThreadCache);
}

}
