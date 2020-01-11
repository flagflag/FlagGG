#pragma once

#include <lua.hpp>

#include "LGEExport.h"

namespace LuaGameEngine
{
	LuaGameEngine_API void InitEngine(lua_State* L);

	LuaGameEngine_API void UninitEngien(lua_State* L);
}
