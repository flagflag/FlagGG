///////
///现在只是随便写一个带帧逻辑的lua环境，后面这部分可以合并到客户端或者服务器里
//////

#include <Lua/LuaVM.h>
#include <Log.h>
#include <Container/Ptr.h>
#include <Utility/SystemHelper.h>

using namespace FlagGG;

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

void RunLuaVM()
{
	Container::SharedPtr<Lua::LuaVM> luaVM(new Lua::LuaVM);
	luaVM->Open();
	if (!luaVM->IsOpen())
	{
		FLAGGG_LOG_ERROR("open lua vm failed.");

		return;
	}

	LogModule logModule(luaVM);
	luaVM->RegisterCPPEvent(
	"log", &logModule,
	{
		LUA_API_PROXY(LogModule, debug, "debug"),
		LUA_API_PROXY(LogModule, info, "info"),
		LUA_API_PROXY(LogModule, warn, "warn"),
		LUA_API_PROXY(LogModule, error, "error")
	});

	if (!luaVM->Execute("LuaCode/main.lua"))
	{
		return;
	}

	FLAGGG_LOG_ERROR("start game.");

	while (true)
	{
		luaVM->CallEvent("update");

		Utility::SystemHelper::Sleep(30);
	}

	FLAGGG_LOG_ERROR("end game.");
}

int main()
{
	RunLuaVM();

	return 0;
}