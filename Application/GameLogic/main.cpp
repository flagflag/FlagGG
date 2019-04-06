///////
///现在只是随便写一个带帧逻辑的lua环境，后面这部分可以合并到客户端或者服务器里
//////

#include <Lua/LuaVM.h>
#include <Log.h>
#include <Container/Ptr.h>
#include <Utility/SystemHelper.h>

#include "DemoScene.h"

using namespace FlagGG::Graphics;
using namespace FlagGG::Math;
using namespace FlagGG::Core;
using namespace FlagGG::Container;
using namespace FlagGG::Config;
using namespace FlagGG::Lua;
using namespace FlagGG::Utility;

LJSONValue commandParam;

class LogModule
{
public:
	LogModule(SharedPtr<LuaVM> luaVM) :
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
	SharedPtr<LuaVM> luaVM_;
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

class GameLogic : public DemoScene
{
public:
	GameLogic() :
		luaVM_(new LuaVM()),
		logModule_(luaVM_)
	{ }

protected:
	void Start() override
	{
		DemoScene::Start();

		luaVM_->Open();
		if (!luaVM_->IsOpen())
		{
			FLAGGG_LOG_ERROR("open lua vm failed.");

			return;
		}

		luaVM_->RegisterCEvents(
		{
			C_LUA_API_PROXY(Begin, "main_begin"),
			C_LUA_API_PROXY(Begin, "main_end")
		});

		luaVM_->RegisterCPPEvents(
			"log", &logModule_,
			{
			LUA_API_PROXY(LogModule, debug, "debug"),
			LUA_API_PROXY(LogModule, info, "info"),
			LUA_API_PROXY(LogModule, warn, "warn"),
			LUA_API_PROXY(LogModule, error, "error")
		});

		const String luaCodePath = commandParam["CodePath"].GetString();

		if (!luaVM_->Execute(luaCodePath + "/main.lua"))
		{
			return;
		}

		FLAGGG_LOG_ERROR("start game.");

		double frameRate = commandParam["FrameRate"].ToDouble();
		sleepTime_ = frameRate == 0.0f ? 32 : (uint64_t)((double)1000 / frameRate);

		context_->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, GameLogic::Update, this));
		context_->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_DOWN, GameLogic::OnKeyDown, this));
		context_->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_UP, GameLogic::OnKeyUp, this));
		context_->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_DOWN, GameLogic::OnMouseDown, this));
		context_->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_UP, GameLogic::OnMouseUp, this));
		context_->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_MOVE, GameLogic::OnMouseMove, this));
	}

	void Stop() override
	{
		FLAGGG_LOG_ERROR("end game.");
	}

	void Update(float timeStep)
	{
		luaVM_->CallEvent("update", timeStep);

		SystemHelper::Sleep(sleepTime_);
	}

	void OnKeyDown(KeyState* keyState, unsigned keyCode)
	{
		luaVM_->CallEvent("on_key_down", keyCode);
	}

	void OnKeyUp(KeyState* keyState, unsigned keyCode)
	{
		luaVM_->CallEvent("on_key_up", keyCode);
	}

	void OnMouseDown(KeyState* keyState, MouseKey mouseKey)
	{
		luaVM_->CallEvent("on_mouse_down", static_cast<uint32_t>(mouseKey));
	}

	void OnMouseUp(KeyState* keyState, MouseKey mouseKey)
	{
		luaVM_->CallEvent("on_mouse_up", static_cast<uint32_t>(mouseKey));
	}

	void OnMouseMove(KeyState* keyState, const Vector2& delta)
	{
		luaVM_->CallEvent("on_mouse_move");
	}

private:
	SharedPtr<LuaVM> luaVM_;

	LogModule logModule_;

	uint64_t sleepTime_;
};

void RunLuaVM()
{
	GameLogic logic;
	logic.Run();
}

int main(int argc, const char* argv[])
{
	if (SystemHelper::ParseCommand(argv + 1, argc - 1, commandParam))
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