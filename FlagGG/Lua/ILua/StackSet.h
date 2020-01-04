#pragma once

#include "Container/Str.h"

#include <lua.hpp>

namespace FlagGG
{
	namespace Lua
	{
		template < class T, class Ignore = void >
		struct Setter {};

		template <>
		struct Setter<bool>
		{
			static void Set(lua_State* L, bool value)
			{
				lua_pushboolean(L, value ? 1 : 0);
			}
		};

		template < class T >
		struct Setter<T, typename std::enable_if<std::is_enum<T>::value>::type>
		{
			static void Set(lua_State* L, T value)
			{
				lua_pushinteger(L, value);
			}
		};

		template < class T >
		struct Setter<T, typename std::enable_if<std::is_integral<T>::value>::type>
		{
			static void Set(lua_State* L, T value)
			{
				lua_pushinteger(L, value);
			}
		};

		template < class T >
		struct Setter<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
		{
			static void Set(lua_State* L, T value)
			{
				lua_pushnumber(L, value);
			}
		};

		template <>
		struct Setter<const char*>
		{
			static void Set(lua_State* L, const char* value)
			{
				lua_pushstring(L, value);
			}
		};

		template <>
		struct Setter<Container::String>
		{
			static void Set(lua_State* L, const Container::String& value)
			{
				lua_pushstring(L, value.CString());
			}
		};

		template < class T >
		void Set(lua_State* L, T&& value)
		{
			Setter<typename std::decay<T>::type>::Set(L, value);
		}
    }
}
