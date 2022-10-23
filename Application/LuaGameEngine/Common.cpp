#include "Common.h"
#include "Engine.h"
#include "InternalEvent.h"

namespace LuaGameEngine
{
	static void PushLuaGameEngine(lua_State* L)
	{
		lua_pushliteral(L, "LuaGameEngine");
		lua_rawget(L, LUA_REGISTRYINDEX);
	}

	Engine* GetEngine(lua_State* L)
	{
		PushLuaGameEngine(L);
		Engine* engine = (Engine*)lua_touserdata(L, -1);
		lua_pop(L, 1);
		return engine;
	}

	InternalEvent* GetInternalEvent(lua_State* L)
	{
		PushLuaGameEngine(L);
		InternalEvent* engine = (InternalEvent*)lua_touserdata(L, -1);
		lua_pop(L, 1);
		return engine;
	}
}
