#pragma once

#include <lua.hpp>
#include "Container/RefCounted.h"

namespace FlagGG
{
	namespace Lua
	{
		FlagGG_API lua_State* luaex_getmainstate(lua_State* L);

		FlagGG_API void luaex_pushglobaltable(lua_State* L, const char* name, bool keyWeakRef = false, bool valueWeakRef = false);

		FlagGG_API void luaex_pushcontext(lua_State* L);

		FlagGG_API int luaex_pushusertype(lua_State* L, const char* userType, void* userValue);

		FlagGG_API int luaex_pushusertyperef(lua_State* L, const char* userType, Container::RefCounted* ref);

		FlagGG_API void* luaex_tousertype(lua_State* L, int idx, const char* userType);

		FlagGG_API bool luaex_juedge_inherit(lua_State* L, const Container::String& type, const Container::String& superClass);
	}
}
