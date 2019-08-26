#pragma once

#include "Container/Str.h"
#include "Lua/ILua/LuaType.h"

#include <lua.hpp>

namespace FlagGG
{
	namespace Lua
	{
		template < class T, class Ignore = void >
		struct Getter {};

		template <>
		struct Getter<bool>
		{
			static bool Get(lua_State* L, int index)
			{
				return !!lua_toboolean(L, index);
			}
		};

		template < class T >
		struct Getter<T, typename std::enable_if<std::is_enum<T>::value>::type>
		{
			static T Get(lua_State *L, int index)
			{
				return static_cast<T>(lua_tointeger(L, index));
			}
		};

		template < class T >
		struct Getter<T, typename std::enable_if<std::is_integral<T>::value>::type>
		{
			static T Get(lua_State* L, int index)
			{
				return static_cast<T>(lua_tointeger(L, index));
			}
		};

		template < class T >
		struct Getter<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
		{
			static T Get(lua_State *L, int index)
			{
				return static_cast<T>(lua_tonumber(L, index));
			}
		};

		template <>
		struct Getter<const char*>
		{
			static const char* Get(lua_State *L, int index)
			{
				return lua_tostring(L, index);
			}
		};

		template <>
		struct Getter<Container::String>
		{
			static Container::String Get(lua_State *L, int index)
			{
				return std::move(Container::String(lua_tostring(L, index)));
			}
		};

		template <>
		struct Getter<LuaFunction>
		{
			static LuaFunction Get(lua_State* L, int index)
			{
				return LuaFunction(L, index);
			}
		};

		template < class T >
		T Get(lua_State* L, int index = -1)
		{
			return Getter<T>::Get(L, index);
		}
    }
}
