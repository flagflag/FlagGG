#include "CoreCLRHelper.h"

#include <Container/HashMap.h>
#include <Utility/SystemHelper.h>
#include <Log.h>

#include <Windows.h>

namespace FlagGG
{

static HashMap<String, DotNetLibRuntime*> DotNetLibRuntimeMap;

struct DotNetLibRuntime
{
	String libName_;

	String libPath_;

	load_assembly_and_get_function_pointer_fn loadAssemblyAndGetFunctionPtr_;
};

bool DotNet_Init()
{
	// Pre-allocate a large buffer for the path to hostfxr
	wchar_t hostfxrPath[MAX_PATH];
	size_t pathLength = sizeof(hostfxrPath) / sizeof(wchar_t);
	int rc = get_hostfxr_path(hostfxrPath, &pathLength, nullptr);
	if (rc != 0)
	{
		FLAGGG_LOG_ERROR("Get hostfxr path failed: {}", rc);
		return false;
	}

	// Load hostfxr and get desired exports
	HMODULE lib = ::LoadLibraryW(hostfxrPath);
	DotNet_HostfxrInit = (hostfxr_initialize_for_runtime_config_fn)::GetProcAddress(lib, "hostfxr_initialize_for_runtime_config");
	DotNet_HostFxrGetDelegate = (hostfxr_get_runtime_delegate_fn)::GetProcAddress(lib, "hostfxr_get_runtime_delegate");
	DotNet_HostFxrClose = (hostfxr_close_fn)::GetProcAddress(lib, "hostfxr_close");

	return true;
}

void DotNet_Shutdown()
{

}

DotNetLibRuntime* DotNet_InitLibRuntime(const String& libName)
{
	DotNetLibRuntime* libRuntime = DotNet_GetLibRuntime(libName);
	if (libRuntime)
		return libRuntime;

	// Load .NET Corez
	hostfxr_handle cxt = nullptr;
	String runtimeConfig = GetProgramDir() + libName + ".runtimeconfig.json";
	WString runtimeConfigW(runtimeConfig);
	int rc = DotNet_HostfxrInit(runtimeConfigW.CString(), nullptr, &cxt);
	if (rc != 0 || cxt == nullptr)
	{
		FLAGGG_LOG_ERROR("Init failed: {}", rc);
		DotNet_HostFxrClose(cxt);
		return nullptr;
	}

	void* loadAssemblyAndGetFunctionPtr = nullptr;
	// Get the load assembly function pointer
	rc = DotNet_HostFxrGetDelegate(
		cxt,
		hdt_load_assembly_and_get_function_pointer,
		&loadAssemblyAndGetFunctionPtr);

	DotNet_HostFxrClose(cxt);

	if (rc != 0 || loadAssemblyAndGetFunctionPtr == nullptr)
	{
		FLAGGG_LOG_ERROR("Get delegate failed: {}", rc);
		return nullptr;
	}

	String libPath = GetProgramDir() + libName + ".dll";
	libRuntime = new DotNetLibRuntime{ libName, libPath, (load_assembly_and_get_function_pointer_fn)loadAssemblyAndGetFunctionPtr };

	DotNetLibRuntimeMap.Insert(MakePair(String(libName), libRuntime));

	return libRuntime;
}

DotNetLibRuntime* DotNet_GetLibRuntime(const String& libName)
{
	auto it = DotNetLibRuntimeMap.Find(String(libName));
	return it != DotNetLibRuntimeMap.End() ? it->second_ : nullptr;
}

int DotNet_LoadAssmblyAndGetFunctionPtr(DotNetLibRuntime* libRuntime, const String& spaceName, const String& methodName, void** entryPointFnPtr)
{
	WString libPathW(libRuntime->libPath_);
	WString typeW(spaceName + ", " + libRuntime->libName_);
	WString methodNameW(methodName);

	int rc = libRuntime->loadAssemblyAndGetFunctionPtr_(
		libPathW.CString(),
		typeW.CString(),
		methodNameW.CString(),
		UNMANAGEDCALLERSONLY_METHOD,
		nullptr,
		entryPointFnPtr);
	return rc;
}

}
