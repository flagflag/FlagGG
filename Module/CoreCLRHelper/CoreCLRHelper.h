#pragma once

#include <Container/Str.h>

#include "Module/CoreCLRHelper/CoreCLRHelperExport.h"

#include "Module/CoreCLRHelper/Runtime/nethost.h"
#include "Module/CoreCLRHelper/Runtime/coreclr_delegates.h"
#include "Module/CoreCLRHelper/Runtime/hostfxr.h"

namespace FlagGG
{

CoreCLRHelper_API hostfxr_initialize_for_runtime_config_fn DotNet_HostfxrInit;
CoreCLRHelper_API hostfxr_get_runtime_delegate_fn DotNet_HostFxrGetDelegate;
CoreCLRHelper_API hostfxr_close_fn DotNet_HostFxrClose;

CoreCLRHelper_API bool DotNet_Init();

CoreCLRHelper_API void DotNet_Shutdown();

struct DotNetLibRuntime;

template <typename F>
class DotNetFunction;

template <typename Ret, class ...Args>
class DotNetFunction<Ret(Args...)>
{
public:
	typedef void (CORECLR_DELEGATE_CALLTYPE* EntryPointFunction)(Args...);

	bool Bind(DotNetLibRuntime* libRuntime, const String& type, const String& methodName)
	{
		return DotNet_LoadAssmblyAndGetFunctionPtr(libRuntime, type, methodName, (void**)&entryPointFn_) == 0;
	}

	Ret operator()(Args ... args)
	{
		return entryPointFn_(std::forward<Args>(args)...);
	}

private:
	EntryPointFunction entryPointFn_;
};

CoreCLRHelper_API DotNetLibRuntime* DotNet_InitLibRuntime(const String& libName);
CoreCLRHelper_API DotNetLibRuntime* DotNet_GetLibRuntime(const String& libName);
CoreCLRHelper_API int DotNet_LoadAssmblyAndGetFunctionPtr(DotNetLibRuntime* libRuntime, const String& spaceName, const String& methodName, void** entryPointFnPtr);

}
