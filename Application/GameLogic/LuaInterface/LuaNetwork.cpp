#include "LuaInterface/LuaNetwork.h"
#include "Network/NetworkAdaptor.h"

LuaNetwork::LuaNetwork(Context* context) :
	context_(context)
{
	LuaVM* luaVM = context->GetVariable<LuaVM>("LuaVM");
	luaVM->RegisterCPPEvents(
		"network",
		this,
		{
			LUA_API_PROXY(LuaNetwork, Connect, "connect"),
			LUA_API_PROXY(LuaNetwork, Disconnect, "disconnect")
		}
	);
}

int LuaNetwork::Connect(LuaVM* luaVM)
{
	const char* ip = luaVM->Get<const char*>(1);
	auto port = luaVM->Get<uint16_t>(2);
	auto* network = context_->GetVariable<Network>("Network");
	network->Connect(ip, port);
	return 0;
}

int LuaNetwork::Disconnect(LuaVM* luaVM)
{
	auto* network = context_->GetVariable<Network>("Network");
	network->Disconnect();
	return 0;
}

