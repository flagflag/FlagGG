///////
///现在只是随便写一个带帧逻辑的lua环境，后面这部分可以合并到客户端或者服务器里
//////

#include <Lua/LuaVM.h>
#include <Log.h>
#include <Container/Ptr.h>
#include <Utility/SystemHelper.h>

using namespace FlagGG;

Config::LJSONValue commandParam;

class LogModule
{
public:
	LogModule(Container::SharedPtr<Lua::LuaVM> luaVM) :
		luaVM_(luaVM)
	{ }

	int debug()
	{
		FLAGGG_LOG_DEBUG(luaVM_->Get<const char*>(-1));
		return 0;
	}

	int info()
	{
		FLAGGG_LOG_INFO(luaVM_->Get<const char*>(-1));
		return 0;
	}

	int warn()
	{
		FLAGGG_LOG_WARN(luaVM_->Get<const char*>(-1));
		return 0;
	}

	int error()
	{
		FLAGGG_LOG_ERROR(luaVM_->Get<const char*>(-1));
		return 0;
	}

private:
	Container::SharedPtr<Lua::LuaVM> luaVM_;
};

static int Begin(lua_State* L)
{
	FLAGGG_LOG_INFO("start excute lua script[main.lua].");
	return 0;
}

static int End(lua_State* L)
{
	FLAGGG_LOG_INFO("end excute lua script[main.lua].");
	return 0;
}

void RunLuaVM()
{
	Container::SharedPtr<Lua::LuaVM> luaVM(new Lua::LuaVM);
	luaVM->Open();
	if (!luaVM->IsOpen())
	{
		FLAGGG_LOG_ERROR("open lua vm failed.");

		return;
	}

	luaVM->RegisterCEvents(
	{
		C_LUA_API_PROXY(Begin, "main_begin"),
		C_LUA_API_PROXY(Begin, "main_end")
	});

	LogModule logModule(luaVM);
	luaVM->RegisterCPPEvents(
	"log", &logModule,
	{
		LUA_API_PROXY(LogModule, debug, "debug"),
		LUA_API_PROXY(LogModule, info, "info"),
		LUA_API_PROXY(LogModule, warn, "warn"),
		LUA_API_PROXY(LogModule, error, "error")
	});

	const Container::String luaCodePath = commandParam["CodePath"].GetString();

	if (!luaVM->Execute(luaCodePath  + "/main.lua"))
	{
		return;
	}

	FLAGGG_LOG_ERROR("start game.");

	double frameRate = commandParam["FrameRate"].ToDouble();
	uint64_t sleepTime = frameRate == 0.0f ? 32 : (uint64_t)((double)1000 / frameRate);

	while (true)
	{
		luaVM->CallEvent("update");

		Utility::SystemHelper::Sleep(sleepTime);
	}

	FLAGGG_LOG_ERROR("end game.");
}

int main(int argc, const char* argv[])
{
	if (Utility::SystemHelper::ParseCommand(argv + 1, argc - 1, commandParam))
	{
		RunLuaVM();
	}
	else
	{
		FLAGGG_LOG_ERROR("parse command failed.");
	}

	getchar();

	return 0;
}