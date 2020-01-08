
#include <Log.h>

#include "GameLogicServer.h"

FlagGG::Config::LJSONValue commandParam;

void Run()
{
	GameLogicServer app(commandParam);
	app.Run();
}

int main(int argc, const char* argv[])
{
	if (SystemHelper::ParseCommand(argv + 1, argc - 1, commandParam))
	{
		Run();
	}
	else
	{
		FLAGGG_LOG_ERROR("Parse command failed.");
	}
	
	return 0;
}
