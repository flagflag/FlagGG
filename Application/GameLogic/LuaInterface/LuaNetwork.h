#pragma once

#include <Core/Context.h>
#include <Lua/LuaVM.h>
#include <Container/Ptr.h>

#include "Network/NetworkAdaptor.h"

using namespace FlagGG::Core;
using namespace FlagGG::Lua;
using namespace FlagGG::Container;

class LuaNetwork : public RefCounted
{
public:
	LuaNetwork(Context* context);

protected:
	int Init(LuaVM* luaVM);
	
	int Connect(LuaVM* luaVM);

	int Disconnect(LuaVM* luaVM);
	
	int Send(LuaVM* luaVM);

	int IsActive(LuaVM* luaVM);

	void OnOpend(NetworkType type, IOFrame::Context::IOContextPtr context);

	void OnClosed(NetworkType type, IOFrame::Context::IOContextPtr context);

	void OnError(NetworkType type, IOFrame::Context::IOContextPtr context, const ErrorCode* error);

	void OnMessageRecived(NetworkType type, IOFrame::Context::IOContextPtr context, IOFrame::Buffer::IOBufferPtr buffer);

private:
	Context* context_;

	bool initialized[NETWORK_TYPE_MAX];
	LuaFunction openCall[NETWORK_TYPE_MAX];
	LuaFunction closeCall[NETWORK_TYPE_MAX];
	LuaFunction errorCall[NETWORK_TYPE_MAX];
	LuaFunction messageCall[NETWORK_TYPE_MAX];
	Network* network_[NETWORK_TYPE_MAX];

	String messageBuffer_;
};
