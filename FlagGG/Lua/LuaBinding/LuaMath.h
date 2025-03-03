#pragma once

#include <lua.hpp>
#include "Math/Vector4.h"
#include "Math/Quaternion.h"
#include "Math/Color.h"

namespace FlagGG
{

FlagGG_API Vector2 luaex_tovector2(lua_State* L, int idx);

FlagGG_API Vector3 luaex_tovector3(lua_State* L, int idx);

FlagGG_API Vector4 luaex_tovector4(lua_State* L, int idx);

FlagGG_API Quaternion luaex_toquaternion(lua_State* L, int idx);

FlagGG_API Color luaex_tocolor(lua_State* L, int idx);


FlagGG_API void luaex_pushvector2(lua_State* L, const Vector2& vec2);

FlagGG_API void luaex_pushvector3(lua_State* L, const Vector3& vec3);

FlagGG_API void luaex_pushvector4(lua_State* L, const Vector4& vec4);

FlagGG_API void luaex_pushquaternion(lua_State* L, const Quaternion& qua);

FlagGG_API void luaex_pushcolor(lua_State* L, const Color& color);

}
