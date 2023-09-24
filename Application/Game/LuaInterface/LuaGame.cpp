#include "LuaInterface/LuaGame.h"

LuaGamePlay::LuaGamePlay()
{
	luaVM_ = GetSubsystem<Context>()->GetVariable<LuaVM>("LuaVM");
	luaVM_->RegisterCPPEvents(
		"gameplay",
		this,
		{
			LUA_API_PROXY(LuaGamePlay, Login, "login"),
			LUA_API_PROXY(LuaGamePlay, StartGame, "start_game"),
			LUA_API_PROXY(LuaGamePlay, EndGame, "end_game")
		}
	);

	gameplay_ = GetSubsystem<Context>()->GetVariable<GamePlayBase>("GamePlayBase");
}

int LuaGamePlay::Login(LuaVM* luaVM)
{
	gameplay_->Login(luaVM->Get<LuaFunction>(1));
	return 0;
}

int LuaGamePlay::StartGame(LuaVM* luaVM)
{
	gameplay_->StartGame();
	return 0;
}

int LuaGamePlay::EndGame(LuaVM* luaVM)
{
	gameplay_->EndGame();
	return 0;
}

