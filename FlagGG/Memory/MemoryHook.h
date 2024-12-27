//
// 内存hook
//

#pragma once

#include "Memory/MemoryDefines.h"
#include "Memory/Memory.h"
#include "Core/BaseMacro.h"
#include <new>

namespace FlagGG
{

struct FlagGG_API UsdMemoryModule
{
	explicit UsdMemoryModule(const char* moduleName);

	~UsdMemoryModule();
};

}

#if USD_MANAGER
#define REPLACEMENT_OPERATOR_NEW_AND_DELETE_USD \
		OPERATOR_NEW_MSVC_PRAGMA void* operator new  (size_t size                       ) OPERATOR_NEW_THROW_SPEC      { return FlagGG::GetSubsystem<FlagGG::Memory>()->Malloc(size); } \
		OPERATOR_NEW_MSVC_PRAGMA void* operator new[](size_t size                       ) OPERATOR_NEW_THROW_SPEC      { return FlagGG::GetSubsystem<FlagGG::Memory>()->Malloc(size); } \
		OPERATOR_NEW_MSVC_PRAGMA void* operator new  (size_t size, const std::nothrow_t&) OPERATOR_NEW_NOTHROW_SPEC    { return FlagGG::GetSubsystem<FlagGG::Memory>()->Malloc(size); } \
		OPERATOR_NEW_MSVC_PRAGMA void* operator new[](size_t size, const std::nothrow_t&) OPERATOR_NEW_NOTHROW_SPEC    { return FlagGG::GetSubsystem<FlagGG::Memory>()->Malloc(size); } \
		void operator delete  (void* ptr)                                                 OPERATOR_DELETE_THROW_SPEC   { return FlagGG::GetSubsystem<FlagGG::Memory>()->Free(ptr); } \
		void operator delete[](void* ptr)                                                 OPERATOR_DELETE_THROW_SPEC   { return FlagGG::GetSubsystem<FlagGG::Memory>()->Free(ptr); } \
		void operator delete  (void* ptr, const std::nothrow_t&)                          OPERATOR_DELETE_NOTHROW_SPEC { return FlagGG::GetSubsystem<FlagGG::Memory>()->Free(ptr); } \
		void operator delete[](void* ptr, const std::nothrow_t&)                          OPERATOR_DELETE_NOTHROW_SPEC { return FlagGG::GetSubsystem<FlagGG::Memory>()->Free(ptr); } \
		void operator delete  (void* ptr, size_t size)                                    OPERATOR_DELETE_THROW_SPEC   { return FlagGG::GetSubsystem<FlagGG::Memory>()->Free(ptr); } \
		void operator delete[](void* ptr, size_t size)                                    OPERATOR_DELETE_THROW_SPEC   { return FlagGG::GetSubsystem<FlagGG::Memory>()->Free(ptr); } \
		void operator delete  (void* ptr, size_t size, const std::nothrow_t&)             OPERATOR_DELETE_NOTHROW_SPEC { return FlagGG::GetSubsystem<FlagGG::Memory>()->Free(ptr); } \
		void operator delete[](void* ptr, size_t size, const std::nothrow_t&)             OPERATOR_DELETE_NOTHROW_SPEC { return FlagGG::GetSubsystem<FlagGG::Memory>()->Free(ptr); }
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

