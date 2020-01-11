#pragma once

#include <lua.hpp>

#include "LGEExport.h"

namespace LuaGameEngine
{
	LuaGameEngine_API void SetEntry(lua_State*L, int idx, void* lightuserdata);

	LuaGameEngine_API void* GetEntryImpl(lua_State* L, int idx);

	template < class T >
	T* GetEntry(lua_State* L, int idx)
	{
		return (T*)GetEntryImpl(L, idx);
	}

	LuaGameEngine_API void CreateClass(lua_State* L, const char* name, const luaL_Reg memberFuncs[], lua_CFunction destructor);
}
