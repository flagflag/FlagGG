#pragma once

#include <lua.hpp>

#include "LGEExport.h"

namespace LuaGameEngine
{
	class Engine;
	class InternalEvent;

	LuaGameEngine_API void SetEntry(lua_State*L, int idx, void* lightuserdata);

	LuaGameEngine_API void* GetEntryImpl(lua_State* L, int idx);

	LuaGameEngine_API void* GetNamedEntryImpl(lua_State* L, int idx, const char* name);

	template < class T >
	T* GetEntry(lua_State* L, int idx)
	{
		return (T*)GetEntryImpl(L, idx);
	}

	template < class T >
	T* GetNamedEntry(lua_State* L, int idx, const char* name)
	{
		return (T*)GetNamedEntry(L, idx, name);
	}

	LuaGameEngine_API Engine* GetEngine(lua_State* L);

	LuaGameEngine_API InternalEvent* GetInternalEvent(lua_State* L);

	LuaGameEngine_API void CreateClass(lua_State* L, const char* name, const luaL_Reg memberFuncs[], lua_CFunction destructor);
}
