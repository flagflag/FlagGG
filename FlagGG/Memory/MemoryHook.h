//
// 内存hook
//

#pragma once

#include "Memory/Memory.h"
#include "Core/BaseMacro.h"
#include <new>

#if USD_MANAGER

namespace FlagGG
{

FlagGG_API extern bool UsdMemoryInitializing;

class FlagGG_API UsdMemoryManager
{
public:
	static FORCEINLINE void* Malloc(USize size)
	{
		if (!UsdMemoryInitializing)
		{
			return GetSubsystem<Memory>()->Malloc(size);
		}
		else
		{
			return Memory::SystemMalloc(size);
		}
	}

	static FORCEINLINE void* Realloc(void* originPtr, USize newSize)
	{
		if (!UsdMemoryInitializing)
		{
			return GetSubsystem<Memory>()->Realloc(originPtr, newSize);
		}
		else
		{
			return Memory::SystemRealloc(originPtr, newSize);
		}
	}

	static FORCEINLINE void Free(void* ptr)
	{
		if (!UsdMemoryInitializing)
		{
			GetSubsystem<Memory>()->Free(ptr);
		}
		else
		{
			Memory::SystemFree(ptr);
		}
	}
};

struct FlagGG_API UsdMemoryModule
{
	explicit UsdMemoryModule(const char* moduleName);

	~UsdMemoryModule();
};

}

#endif

#if USD_MANAGER
#define REPLACEMENT_OPERATOR_NEW_AND_DELETE_USD \
		OPERATOR_NEW_MSVC_PRAGMA void* operator new  (size_t size                       ) OPERATOR_NEW_THROW_SPEC      { return FlagGG::UsdMemoryManager::Malloc(size); } \
		OPERATOR_NEW_MSVC_PRAGMA void* operator new[](size_t size                       ) OPERATOR_NEW_THROW_SPEC      { return FlagGG::UsdMemoryManager::Malloc(size); } \
		OPERATOR_NEW_MSVC_PRAGMA void* operator new  (size_t size, const std::nothrow_t&) OPERATOR_NEW_NOTHROW_SPEC    { return FlagGG::UsdMemoryManager::Malloc(size); } \
		OPERATOR_NEW_MSVC_PRAGMA void* operator new[](size_t size, const std::nothrow_t&) OPERATOR_NEW_NOTHROW_SPEC    { return FlagGG::UsdMemoryManager::Malloc(size); } \
		void operator delete  (void* ptr)                                                 OPERATOR_DELETE_THROW_SPEC   { return FlagGG::UsdMemoryManager::Free(ptr); } \
		void operator delete[](void* ptr)                                                 OPERATOR_DELETE_THROW_SPEC   { return FlagGG::UsdMemoryManager::Free(ptr); } \
		void operator delete  (void* ptr, const std::nothrow_t&)                          OPERATOR_DELETE_NOTHROW_SPEC { return FlagGG::UsdMemoryManager::Free(ptr); } \
		void operator delete[](void* ptr, const std::nothrow_t&)                          OPERATOR_DELETE_NOTHROW_SPEC { return FlagGG::UsdMemoryManager::Free(ptr); } \
		void operator delete  (void* ptr, size_t size)                                    OPERATOR_DELETE_THROW_SPEC   { return FlagGG::UsdMemoryManager::Free(ptr); } \
		void operator delete[](void* ptr, size_t size)                                    OPERATOR_DELETE_THROW_SPEC   { return FlagGG::UsdMemoryManager::Free(ptr); } \
		void operator delete  (void* ptr, size_t size, const std::nothrow_t&)             OPERATOR_DELETE_NOTHROW_SPEC { return FlagGG::UsdMemoryManager::Free(ptr); } \
		void operator delete[](void* ptr, size_t size, const std::nothrow_t&)             OPERATOR_DELETE_NOTHROW_SPEC { return FlagGG::UsdMemoryManager::Free(ptr); }
#else
#define REPLACEMENT_OPERATOR_NEW_AND_DELETE_USD
#endif

#if USD_MANAGER
#define IMPLEMENT_MODULE_USD(ModuleName) \
	FlagGG::UsdMemoryModule _(ModuleName); \
	REPLACEMENT_OPERATOR_NEW_AND_DELETE_USD
#else
#define IMPLEMENT_MODULE_USD(ModuleName)
#endif

#if USD_MANAGER
#define malloc(ptr) FlagGG::UsdMemoryManager::Malloc(ptr)
#define realloc(originPtr, newSize) FlagGG::UsdMemoryManager::Realloc(originPtr, newSize)
#define free(ptr) FlagGG::UsdMemoryManager::Free(ptr)
#endif
