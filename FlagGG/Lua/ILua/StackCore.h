#pragma once

#include "Lua/ILua/StackGet.h"
#include "Lua/ILua/StackSet.h"

namespace FlagGG
{

struct FlagGG_API StackGuard
{
public:
	StackGuard(lua_State* L) : L_(L), top_(lua_gettop(L_)) { }

	~StackGuard() { lua_settop(L_, top_); }

private:
	lua_State* L_;

	int top_;
};

}

