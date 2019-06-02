#include "LuaInterface/LuaNetwork.h"
#include "Network/NetworkAdaptor.h"

LuaNetwork::LuaNetwork(Context* context) :
	context_(context),
	luaVM_(context->GetVariable<LuaVM>("LuaVM"))
{
	luaVM_->RegisterCPPEvents(
		"network",
		this,
		{
			LUA_API_PROXY(LuaNetwork, Connect, "connect"),
			LUA_API_PROXY(LuaNetwork, Disconnect, "disconnect")
		}
	);
}

int LuaNetwork::Connect()
{
	const char* ip = luaVM_->Get<const char*>(1);
	auto port = luaVM_->Get<uint16_t>(2);
	auto* network = context_->GetVariable<Network>("Network");
	network->Connect(ip, port);
	return 0;
}

int LuaNetwork::Disconnect()
{
	auto* network = context_->GetVariable<Network>("Network");
	network->Disconnect();
	return 0;
}

