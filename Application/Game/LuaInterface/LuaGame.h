#pragma once

#include <Core/Context.h>
#include <Lua/LuaVM.h>
#include <Container/Ptr.h>

#include "Network/NetworkAdaptor.h"

using namespace FlagGG::Core;
using namespace FlagGG::Lua;
using namespace FlagGG::Container;

class LuaGamePlay : public RefCounted
{
public:
	explicit LuaGamePlay(Context* context);

protected:
	int StartGame(LuaVM* luaVM);

	int EndGame(LuaVM* luaVM);

private:
	Context* context_;

	WeakPtr<Network> network_;
};
