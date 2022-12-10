#pragma once

#include <lua.hpp>

#include "LGEExport.h"
#include "EngineObject.h"

#include <Lua/LuaBinding/LuaExtend.h>

namespace LuaGameEngine
{

class Engine;
class InternalEvent;

template < class T >
void SetEntry(lua_State* L, EngineObject* entry)
{
	if (luaex_pushusertype(L, T::GetTypeNameStatic().CString(), entry))
		entry->AddRef();
}

template < class T >
T* GetEntry(lua_State* L, int idx)
{
	return (T*)luaex_tousertype(L, idx, T::GetTypeNameStatic().CString());
}

LuaGameEngine_API Engine* GetEngine(lua_State* L);

LuaGameEngine_API InternalEvent* GetInternalEvent(lua_State* L);

}
