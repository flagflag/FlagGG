#include "LuaMath.h"

namespace FlagGG
{

template < class T >
float PODDGet(const T& value, unsigned index)
{
	return value[index];
}

template <>
float PODDGet<Quaternion>(const Quaternion& value, unsigned index)
{
	return (&value.w_)[index];
}

template <>
float PODDGet<Color>(const Color& value, unsigned index)
{
	return (&value.r_)[index];
}

template < class T >
void PODSet(T& value, unsigned index, float v)
{
	value[index] = v;
}

template <>
void PODSet<Quaternion>(Quaternion& value, unsigned index, float v)
{
	(&value.w_)[index] = v;
}

template <>
void PODSet<Color>(Color& value, unsigned index, float v)
{
	(&value.r_)[index] = v;
}

template < unsigned N, class T >
T pod_tovalue(lua_State* L, int idx)
{
	T ret;
	int len = lua_rawlen(L, idx);
	if (len == N)
	{
		for (unsigned i = 0; i < N; ++i)
		{
			lua_rawgeti(L, idx, i + 1);
			PODSet<T>(ret, i, lua_tonumber(L, -1));
			lua_pop(L, 1);
		}
	}
	return ret;
}

template < unsigned N, class T >
void pod_pushvalue(lua_State* L, const T& value)
{
	lua_createtable(L, N, 0);

	for (unsigned i = 0; i < N; ++i)
	{
		lua_pushnumber(L, PODDGet<T>(value, i));
		lua_rawseti(L, -2, i + 1);
	}
}

Vector2 luaex_tovector2(lua_State* L, int idx)
{
	return pod_tovalue<2, Vector2>(L, idx);
}

Vector3 luaex_tovector3(lua_State* L, int idx)
{
	return pod_tovalue<3, Vector3>(L, idx);
}

Vector4 luaex_tovector4(lua_State* L, int idx)
{
	return pod_tovalue<4, Vector4>(L, idx);
}

Quaternion luaex_toquaternion(lua_State* L, int idx)
{
	return pod_tovalue<4, Quaternion>(L, idx);
}

Color luaex_tocolor(lua_State* L, int idx)
{
	return pod_tovalue<4, Color>(L, idx);
}

void luaex_pushvector2(lua_State* L, const Vector2& vec2)
{
	pod_pushvalue<2>(L, vec2);
}

void luaex_pushvector3(lua_State* L, const Vector3& vec3)
{
	pod_pushvalue<3>(L, vec3);
}

void luaex_pushvector4(lua_State* L, const Vector4& vec4)
{
	pod_pushvalue<4>(L, vec4);
}

void luaex_pushquaternion(lua_State* L, const Quaternion& qua)
{
	pod_pushvalue<4>(L, qua);
}

void luaex_pushcolor(lua_State* L, const Color& color)
{
	pod_pushvalue<4>(L, color);
}

}
