///////
///现在只是随便写一个带帧逻辑的lua环境，后面这部分可以合并到客户端或者服务器里
//////

#include <Log.h>
#include "GameApplication.h"
#include "main.h"

static LJSONValue commandParam;

void RunLuaVM()
{
	GameApplication app(commandParam);
	app.Run();
}

int EntryPoint(int argc, const char* argv[])
{
	if (ParseCommand(argv + 1, argc - 1, commandParam))
	{
		RunLuaVM();
	}
	else
	{
		FLAGGG_LOG_ERROR("Parse command failed.");
	}

	return 0;
}
