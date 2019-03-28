#include <Lua/LuaVM.h>
#include <Log.h>

void Run()
{
	enum
	{
		_2333 = 0
	};

	FlagGG::Lua::LuaVM luaVM;
	luaVM.Open();
	if (!luaVM.IsOpen())
	{
		FLAGGG_LOG_ERROR("open lua vm failed.");

		return;
	}

	luaVM.CallEvent("2333", true);
	luaVM.CallEvent("2333", _2333);
	luaVM.CallEvent("2333", (int)233);
	luaVM.CallEvent("2333", (float)6666.0);
	luaVM.CallEvent("2333", (double)6666.0);
	luaVM.CallEvent("2333", "2333");
	luaVM.CallEvent("2333", FlagGG::Container::String("2333"));
	FlagGG::Container::String temp1("2333");
	luaVM.CallEvent("2333", temp1);
	const FlagGG::Container::String temp2("2333");
	luaVM.CallEvent("2333", temp2);
}

int main()
{
	Run();

	getchar();

	return 0;
}