#pragma once

#include <Core/Context.h>
#include <Lua/LuaVM.h>
#include <Container/Ptr.h>

using namespace FlagGG::Core;
using namespace FlagGG::Lua;
using namespace FlagGG::Container;

class LuaNetwork : public RefCounted
{
public:
	LuaNetwork(Context* context);

protected:
	int Connect(LuaVM* luaVM);

	int Disconnect(LuaVM* luaVM);

private:
	Context* context_;
};
