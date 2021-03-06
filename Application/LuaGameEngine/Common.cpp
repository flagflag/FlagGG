#include "Common.h"
#include "Engine.h"
#include "InternalEvent.h"

namespace LuaGameEngine
{
	void SetEntry(lua_State*L, int idx, void* lightuserdata)
	{
		lua_pushliteral(L, "_pointer");
		lua_pushlightuserdata(L, lightuserdata);
		lua_rawset(L, idx > 0 ? idx : idx - 2);
	}

	void* GetEntryImpl(lua_State* L, int idx)
	{
		lua_pushliteral(L, "_pointer");
		lua_rawget(L, idx);
		void* lightuserdata = lua_touserdata(L, -1);
		lua_pop(L, 1);
		return lightuserdata;
	}

	void* GetNamedEntryImpl(lua_State* L, int idx, const char* name)
	{
		lua_pushstring(L, name);
		lua_rawget(L, idx);
		void* lightuserdata = lua_touserdata(L, -1);
		lua_pop(L, 1);
		return lightuserdata;
	}

	Engine* GetEngine(lua_State* L)
	{
		lua_getglobal(L, "context");
		Engine* engine = (Engine*)lua_touserdata(L, -1);
		lua_pop(L, 1);
		return engine;
	}

	InternalEvent* GetInternalEvent(lua_State* L)
	{
		lua_getglobal(L, "context");
		InternalEvent* engine = (InternalEvent*)lua_touserdata(L, -1);
		lua_pop(L, 1);
		return engine;
	}

	void CreateClass(lua_State* L, const char* name, const luaL_Reg memberFuncs[], lua_CFunction destructor)
	{
		lua_getglobal(L, "engine");
		lua_newtable(L);
		lua_createtable(L, 1, 0);

		lua_pushliteral(L, "__index");
		lua_createtable(L, sizeof(memberFuncs) - 1, 0);
		luaL_setfuncs(L, memberFuncs, 0);
		lua_rawset(L, -3);

		lua_pushliteral(L, "__gc");
		lua_pushcfunction(L, destructor);
		lua_rawset(L, -3);

		lua_setmetatable(L, -2);
		lua_setfield(L, -2, name);
		lua_pop(L, 1);
	}
}
