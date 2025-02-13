///////
///现在只是随便写一个带帧逻辑的lua环境，后面这部分可以合并到客户端或者服务器里
//////

#include "Core/EngineSettings.h"
#include <Log.h>
#include <Memory/MemoryHook.h>
#include <CSharpExport/CSharpExport.h>
#include "GameApplication.h"
#include "main.h"

#include <Windows.h>

static LJSONValue CommandParam;
static GameApplication* ApplicationInstance = nullptr;

void RunApplication(SetupFinish setupFinish)
{
	String rendererType = CommandParam["renderer_type"].GetString();
	if (rendererType == "dx11")
		GetSubsystem<EngineSettings>()->rendererType_ = RENDERER_TYPE_D3D11;
	else if(rendererType == "gl")
		GetSubsystem<EngineSettings>()->rendererType_ = RENDERER_TYPE_OPENGL;
	else if(rendererType == "metal")
		GetSubsystem<EngineSettings>()->rendererType_ = RENDERER_TYPE_METAL;
	else if(rendererType == "vk")
		GetSubsystem<EngineSettings>()->rendererType_ = RENDERER_TYPE_VULKAN;
	if (CommandParam.Contains("cluster_light"))
		GetSubsystem<EngineSettings>()->clusterLightEnabled_ = true;
	if (CommandParam.Contains("close_hiz"))
		GetSubsystem<EngineSettings>()->occlusionCullingType_ = OcclusionCullingType::None;
	if (CommandParam.Contains("ao_type"))
		GetSubsystem<EngineSettings>()->aoType_ = (AmbientOcclusionType)ToInt(CommandParam["ao_type"].GetString());
	if (CommandParam.Contains("close_ssr"))
		GetSubsystem<EngineSettings>()->renderSSR_ = false;

	GameApplication app(CommandParam, setupFinish);
	ApplicationInstance = &app;
	app.Run();
}

int EntryPoint(int argc, const char* argv[])
{
	if (ParseCommand(argv + 1, argc - 1, CommandParam))
	{
		RunApplication(nullptr);
	}
	else
	{
		FLAGGG_LOG_ERROR("Parse command failed.");
	}

	return 0;
}

CSharp_API int CSharpEntryPoint(SetupFinish setupFinish)
{
	LPSTR commandLine = GetCommandLineA();
	if (ParseCommand(commandLine, CommandParam))
	{
		RunApplication(setupFinish);
	}
	else
	{
		FLAGGG_LOG_ERROR("Parse command failed.");
	}

	return 0;
}

CSharp_API void AttachTo(void* windowHandle, void* targetWindowHandle, int x, int y, int width, int height)
{
	SetParent((HWND)windowHandle, (HWND)targetWindowHandle);
	SetWindowPos((HWND)windowHandle, NULL, x, y, width, height, 0);
}

CSharp_API void ShowPrefab(const char* prefabPathCStr)
{
	const String prefabPath = prefabPathCStr;
	ApplicationInstance->GetForwarder().Forward([prefabPath]()
	{
		ApplicationInstance->ShowPrefab(prefabPath);
	});
}

IMPLEMENT_MODULE_USD("Game");
