#include "MallocSystem.h"
#include "Memory/Memory.h"
#include "Math/Math.h"

namespace FlagGG
{

void* MallocSystem::Malloc(USize size, UInt32 alignment)
{
	return Memory::SystemMalloc(size);
}

void* MallocSystem::Realloc(void* originPtr, USize newSize, UInt32 alignment)
{
	return Memory::SystemRealloc(originPtr, newSize);
}

void MallocSystem::Free(void* ptr)
{
	Memory::SystemFree(ptr);
}

bool MallocSystem::GetAllocationSize(void* ptr, USize& outSize)
{
	return false;
}

void MallocSystem::Trim(bool trimThreadCache)
{
	
}

}
