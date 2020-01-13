#include "LuaInterface/LuaGame.h"

LuaGamePlay::LuaGamePlay(Context* context) :
	context_(context)
{
	LuaVM* luaVM = context->GetVariable<LuaVM>("LuaVM");
	luaVM->RegisterCPPEvents(
		"gameplay",
		this,
		{
			LUA_API_PROXY(LuaGamePlay, StartGame, "start_game"),
			LUA_API_PROXY(LuaGamePlay, EndGame, "end_game")
		}
	);

	network_ = context->GetVariable<Network>(NETWORK_TYPE_NAME[NETWORK_TYPE_UDP]);
}

int LuaGamePlay::StartGame(LuaVM* luaVM)
{
	if (!network_)
	{
		luaVM->Set(false);
	}
	else
	{
		String buffer("START_GAME");
		network_->Send(buffer.CString(), buffer.Length());
		luaVM->Set(true);
	}
	return 1;
}

int LuaGamePlay::EndGame(LuaVM* luaVM)
{
	if (!network_)
	{
		luaVM->Set(false);
	}
	else
	{

		luaVM->Set(true);
	}
	return 1;
}
