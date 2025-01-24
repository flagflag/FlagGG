#include "Memory.h"
#include "Memory/Malloc/MallocSystem.h"
#include "Memory/Malloc/MallocMimalloc.h"
#include "Memory/Malloc/MallocStomp.h"
#include "Memory/Malloc/MallocThreadSafeProxy.h"
#include "Memory/WindowsCommandLine.h"

namespace FlagGG
{

FlagGG_API extern bool UsdMemoryInitializing = false;

Memory::Memory()
{
	// 这里用到一个技巧，Memory作为单例初始化时是线程安全的（函数内static变量，系统会做线程安全保证，具体原理这里不赘述）
	// 所以初始化时，我在Memory构造函数改UsdMemoryInitializing是线程安全的
	UsdMemoryInitializing = true;

	if (
#if USE_MALLOC_STOMP
		true
#else
		CommandLineHas("-use_stomp_malloc")
#endif
		)
	{
		malloc_ = new MallocStomp();
	}
	else if (
#if USE_SYSTEM_MALLOC
		true
#else
		CommandLineHas("-use_system_malloc")
#endif
		)
	{
		malloc_ = new MallocSystem();
	}
	else
	{
		malloc_ = new MallocMimalloc();
	}

	if (!malloc_->IsInternallyThreadSafe())
	{
		malloc_ = new MallocThreadSafeProxy(malloc_);
	}

	UsdMemoryInitializing = false;
}

Memory::Memory(IMalloc* malloc)
	: malloc_(malloc)
{

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
