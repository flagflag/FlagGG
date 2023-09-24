#pragma once

#include <Core/Object.h>
#include <Core/Context.h>
#include <Lua/LuaVM.h>
#include <Container/Ptr.h>

#include "Network/NetworkAdaptor.h"

using namespace FlagGG;

class LuaNetwork : public Object
{
	OBJECT_OVERRIDE(LuaNetwork, Object);
public:
	explicit LuaNetwork();

protected:
	int Init(LuaVM* luaVM);
	
	int Connect(LuaVM* luaVM);

	int Disconnect(LuaVM* luaVM);
	
	int Send(LuaVM* luaVM);

	int IsActive(LuaVM* luaVM);

	void OnOpend(NetworkType type, IOFrame::Context::IOContextPtr context);

	void OnClosed(NetworkType type, IOFrame::Context::IOContextPtr context);

	void OnError(NetworkType type, IOFrame::Context::IOContextPtr context, int errorCode, String errorMsg);

	void OnMessageRecived(NetworkType type, IOFrame::Context::IOContextPtr context, IOFrame::Buffer::IOBufferPtr buffer);

private:
	bool initialized[NETWORK_TYPE_MAX];
	LuaFunction openCall[NETWORK_TYPE_MAX];
	LuaFunction closeCall[NETWORK_TYPE_MAX];
	LuaFunction errorCall[NETWORK_TYPE_MAX];
	LuaFunction messageCall[NETWORK_TYPE_MAX];
	Network* network_[NETWORK_TYPE_MAX];

	String messageBuffer_;
};
