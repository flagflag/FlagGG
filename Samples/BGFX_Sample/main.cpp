#include <Config/LJSONValue.h>
#include <Utility/SystemHelper.h>
#include <Log.h>

#include "BgfxApp.h"

FlagGG::Config::LJSONValue commandParam;

void RunLuaVM()
{
	BgfxApp app(commandParam);
	app.Run();
}

int main(int argc, const char* argv[])
{
	if (FlagGG::Utility::SystemHelper::ParseCommand(argv + 1, argc - 1, commandParam))
	{
		RunLuaVM();
	}
	else
	{
		FLAGGG_LOG_ERROR("Parse command failed.");
	}

	return 0;
}