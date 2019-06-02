#include <Log.h>

#include "GameApplication.h"

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

GameApplication::GameApplication(LJSONValue commandParam) :
	commandParam_(commandParam),
	luaVM_(new LuaVM()),
	network_(new Network())
{ }

void GameApplication::Start()
{
	GameEngine::Start();

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

	const String luaCodePath = commandParam_["CodePath"].GetString();

	if (!luaVM_->Execute(luaCodePath + "/main.lua"))
	{
		return;
	}

	FLAGGG_LOG_INFO("start game.");

	context_->RegisterVariable<LuaVM>(luaVM_, "LuaVM");
	context_->RegisterVariable<Network>(network_, "Network");

	logModule_ = new LuaLog(context_);
	networkModule_ = new LuaNetwork(context_);

	context_->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, GameApplication::Update, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_DOWN, GameApplication::OnKeyDown, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_UP, GameApplication::OnKeyUp, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_DOWN, GameApplication::OnMouseDown, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_UP, GameApplication::OnMouseUp, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_MOVE, GameApplication::OnMouseMove, this));

	SetFrameRate(commandParam_["FrameRate"].ToDouble());
}

void GameApplication::Stop()
{
	FLAGGG_LOG_ERROR("end game.");
}

void GameApplication::Update(float timeStep)
{
	luaVM_->CallEvent("update", timeStep);
}

void GameApplication::OnKeyDown(KeyState* keyState, unsigned keyCode)
{
	luaVM_->CallEvent("on_key_down", keyCode);
}

void GameApplication::OnKeyUp(KeyState* keyState, unsigned keyCode)
{
	luaVM_->CallEvent("on_key_up", keyCode);
}

void GameApplication::OnMouseDown(KeyState* keyState, MouseKey mouseKey)
{
	luaVM_->CallEvent("on_mouse_down", static_cast<uint32_t>(mouseKey));
}

void GameApplication::OnMouseUp(KeyState* keyState, MouseKey mouseKey)
{
	luaVM_->CallEvent("on_mouse_up", static_cast<uint32_t>(mouseKey));
}

void GameApplication::OnMouseMove(KeyState* keyState, const Vector2& delta)
{
	luaVM_->CallEvent("on_mouse_move");
}
