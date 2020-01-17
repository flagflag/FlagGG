#include "LuaInterface/LuaGame.h"
#include "Proto/Game.pb.h"

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
		Proto::Game::RequestStartGame request;
		request.set_user_id(0u);
		request.set_game_name("TestGame");

		Proto::Game::MessageHeader header;
		header.set_message_type(Proto::Game::MessageType_RequestStartGame);
		header.set_message_body(request.SerializeAsString());

		const std::string& buffer = header.SerializeAsString();

		network_->Send(buffer.data(), buffer.length());
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
