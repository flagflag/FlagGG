#include "Memory.h"
#include "Memory/Malloc/MallocSystem.h"
#include "Memory/Malloc/MallocMimalloc.h"

namespace FlagGG
{

Memory::Memory()
{
#if !USE_SYSTEM_MALLOC
	malloc_ = new MallocMimalloc();
#else
	malloc_ = new MallocSystem();
#endif
}

Memory::~Memory()
{
	delete malloc_;
	malloc_ = nullptr;
}

void* Memory::Malloc(USize size, UInt32 alignment)
{
	return malloc_->Malloc(size, alignment);
}

void* Memory::Realloc(void* originPtr, USize newSize, UInt32 alignment)
{
	return malloc_->Realloc(originPtr, newSize, alignment);
}

void Memory::Free(void* ptr)
{
	malloc_->Free(ptr);
}

bool Memory::GetAllocationSize(void* ptr, USize& outSize)
{
	return malloc_->GetAllocationSize(ptr, outSize);
}

void Memory::Trim(bool trimThreadCache)
{
	malloc_->Trim(trimThreadCache);
}

}
