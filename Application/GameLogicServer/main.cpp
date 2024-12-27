
#include <Log.h>
#include <Memory/MemoryHook.h>

#include "GameLogicServer.h"

LJSONValue commandParam;

void Run()
{
	GameLogicServer app(commandParam);
	app.Run();
}

int main(int argc, const char* argv[])
{
	if (ParseCommand(argv + 1, argc - 1, commandParam))
	{
		Run();
	}
	else
	{
		FLAGGG_LOG_ERROR("Parse command failed.");
	}
	
	return 0;
}

IMPLEMENT_MODULE_USD("GameLogicServer");
