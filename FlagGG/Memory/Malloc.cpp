#include "Malloc.h"
#include "Memory/Memory.h"

namespace FlagGG
{

void* UseSystemMalloc::operator new(size_t size)
{
	return Memory::SystemMalloc(size);
}

void UseSystemMalloc::operator delete(void* ptr)
{
	Memory::SystemFree(ptr);
}

void* UseSystemMalloc::operator new[](size_t size)
{
	return Memory::SystemMalloc(size);
}

void UseSystemMalloc::operator delete[](void* ptr)
{
	Memory::SystemFree(ptr);
}

}
