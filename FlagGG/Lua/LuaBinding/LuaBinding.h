#pragma once

#include <lua.hpp>
#include "Container/Str.h"

namespace FlagGG
{

FlagGG_API void luaex_beginclass(lua_State* L, const char* name, const char* base, lua_CFunction contructor, lua_CFunction destructor);

FlagGG_API void luaex_endclass(lua_State* L);

FlagGG_API void luaex_classfunction(lua_State* L, const char* name, lua_CFunction func);

FlagGG_API void luaex_function(lua_State* L, const char* name, lua_CFunction func);

FlagGG_API void luaex_globalfunction(lua_State* L, const char* name, lua_CFunction func);

FlagGG_API void luaex_constant(lua_State* L, const char* name, long long value);

FlagGG_API void luaex_constant(lua_State* L, const char* name, const char* value);

FlagGG_API void luaex_classconstant(lua_State* L, const char* name, long long value);

FlagGG_API void luaex_classconstant(lua_State* L, const char* name, const String& value);

}
