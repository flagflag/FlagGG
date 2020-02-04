#pragma once

#include <lua.hpp>

#include "LGEExport.h"
#include "Engine.h"

namespace LuaGameEngine
{
	LuaGameEngine_API Engine* CreateEngine(lua_State* L);

	LuaGameEngine_API void DestroyEngine(Engine* engine);
}
