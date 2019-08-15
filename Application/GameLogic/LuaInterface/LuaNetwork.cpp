#include <Log.h>
#include "LuaInterface/LuaNetwork.h"

LuaNetwork::LuaNetwork(Context* context) :
	context_(context)
{
	INIT_ARRAY(initialized, false);
	INIT_ARRAY(network_, nullptr);

	LuaVM* luaVM = context->GetVariable<LuaVM>("LuaVM");
	luaVM->RegisterCPPEvents(
		"network",
		this,
		{
			LUA_API_PROXY(LuaNetwork, Init, "init"),
			LUA_API_PROXY(LuaNetwork, Connect, "connect"),
			LUA_API_PROXY(LuaNetwork, Disconnect, "disconnect"),
			LUA_API_PROXY(LuaNetwork, Send, "send"),
		}
	);

	context_->RegisterEvent(EVENT_HANDLER(NetworkEvent::OPEND, LuaNetwork::OnOpend, this));
	context_->RegisterEvent(EVENT_HANDLER(NetworkEvent::CLOSED, LuaNetwork::OnClosed, this));
	context_->RegisterEvent(EVENT_HANDLER(NetworkEvent::CATCH_ERROR, LuaNetwork::OnError, this));
	context_->RegisterEvent(EVENT_HANDLER(NetworkEvent::MESSAGE_RECIVED, LuaNetwork::OnMessageRecived, this));
}

int LuaNetwork::Init(LuaVM* luaVM)
{
	uint32_t networkType = luaVM->Get<uint32_t>(1);
	initialized[networkType] = true;
	openCall[networkType] = luaVM->Get<LuaFunction>(2);
	closeCall[networkType] = luaVM->Get<LuaFunction>(3);
	errorCall[networkType] = luaVM->Get<LuaFunction>(4);
	messageCall[networkType] = luaVM->Get<LuaFunction>(5);
	network_[networkType] = context_->GetVariable<Network>(NETWORK_TYPE_NAME[networkType]);
	return 0;
}

int LuaNetwork::Connect(LuaVM* luaVM)
{
	uint32_t networkType = luaVM->Get<uint32_t>(1);
	const char* ip = luaVM->Get<const char*>(2);
	auto port = luaVM->Get<uint16_t>(3);
	auto* network = network_[networkType];
	if (network)
		network->Connect(ip, port);
	return 0;
}

int LuaNetwork::Disconnect(LuaVM* luaVM)
{
	uint32_t networkType = luaVM->Get<uint32_t>(1);
	auto* network = network_[networkType];
	if (network)
		network->Disconnect();
	return 0;
}

int LuaNetwork::Send(LuaVM* luaVM)
{
	uint32_t networkType = luaVM->Get<uint32_t>(1);
	const char* data = luaVM->Get<const char*>(2);
	uint32_t dataSize = luaVM->Get<uint32_t>(3);
	auto* network = network_[networkType];
	if (network)
		network->SendMessage(data, dataSize);
	return 0;
}

void LuaNetwork::OnOpend(NetworkType type, IOFrame::Context::IOContextPtr context)
{
	if (initialized[type])
	{
		openCall[type]();
	}
	else
	{
		FLAGGG_LOG_ERROR("OnOpend, Network type(%d) not initialized.", type);
	}
}

void LuaNetwork::OnClosed(NetworkType type, IOFrame::Context::IOContextPtr context)
{
	if (initialized[type])
	{
		closeCall[type]();
	}
	else
	{
		FLAGGG_LOG_ERROR("OnClosed, Network type(%d) not initialized.", type);
	}
}

void LuaNetwork::OnError(NetworkType type, IOFrame::Context::IOContextPtr context, const ErrorCode* error)
{
	if (initialized[type])
	{
		errorCall[type](error->Value(), error->Message());
	}
	else
	{
		FLAGGG_LOG_ERROR("OnError, Network type(%d) not initialized.", type);
	}
}

void LuaNetwork::OnMessageRecived(NetworkType type, IOFrame::Context::IOContextPtr context, IOFrame::Buffer::IOBufferPtr buffer)
{
	if (initialized[type])
	{
		buffer->ToString(messageBuffer_);
		messageCall[type](messageBuffer_.CString());
	}
	else
	{
		FLAGGG_LOG_ERROR("OnMessageRecived, Network type(%d) not initialized.", type);
	}
}
