#pragma once

#include <lua.hpp>

void SetEntry(lua_State*L, int idx, void* lightuserdata);

void* GetEntryImpl(lua_State* L, int idx);

template < class T >
T* GetEntry(lua_State* L, int idx)
{
	return (T*)GetEntryImpl(L, idx);
}

void CreateClass(lua_State* L, const char* name, const luaL_Reg memberFuncs[], lua_CFunction destructor);
