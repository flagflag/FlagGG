#include <Lua/LuaVM.h>
#include <Log.h>

class LuaLog
{
public:
	int Normal(FlagGG::Lua::LuaVM* vm)
	{
		printf("%s\n", vm->Get<const char*>(1));
		return 0;
	}
};

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

	LuaLog logInstance;

	luaVM.RegisterCPPEvents(
		"log",
		&logInstance,
		{
			LUA_API_PROXY(LuaLog, Normal, "normal")
		}
	);

	luaVM.ExecuteScript(R"(
Test = {}
function Test.normal(...)
	log.normal(...)
end
)");

	luaVM.CallEvent("Test.normal", true);
	luaVM.CallEvent("Test.normal", _2333);
	luaVM.CallEvent("Test.normal", (int)233);
	luaVM.CallEvent("Test.normal", (float)6666.0);
	luaVM.CallEvent("Test.normal", (double)6666.0);
	luaVM.CallEvent("Test.normal", "2333");
	const char* temp0 = "2333";
	luaVM.CallEvent("Test.normal", temp0);
	luaVM.CallEvent("Test.normal", FlagGG::Container::String("2333"));
	FlagGG::Container::String temp1("2333");
	luaVM.CallEvent("Test.normal", temp1);
	const FlagGG::Container::String temp2("2333");
	luaVM.CallEvent("Test.normal", temp2);
}

int main()
{
	Run();

	getchar();

	return 0;
}