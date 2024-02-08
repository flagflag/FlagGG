///////
///现在只是随便写一个带帧逻辑的lua环境，后面这部分可以合并到客户端或者服务器里
//////

#include <Log.h>
#include "GameApplication.h"
#include "main.h"

#include <Windows.h>

static LJSONValue CommandParam;
static GameApplication* ApplicationInstance = nullptr;

void RunApplication(SetupFinish setupFinish)
{
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
