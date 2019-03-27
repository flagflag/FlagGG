#ifndef __STACK_CORE__
#define __STACK_CORE__

#include "Lua/ILua/LuaType.h"

namespace FlagGG
{
	namespace Lua
	{
		template < class T >
		bool IsSame(lua_State* L, int index)
		{
			return LuaTypeOf<T>::value == TypeOf(L, index);
		}

//Getter:
		template < class T >
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
		struct Getter<typename std::enable_if<std::is_enum<T>::value>::type>
		{
			static T Get(lua_State *L, int index)
			{
				return static_cast<T>(lua_tointeger(L, index));
			}
		};

		template < class T >
		struct Getter<typename std::enable_if<std::is_integral<T>::value>::type>
		{
			static T Get(lua_State* L, int index)
			{
				return static_cast<T>(lua_tointeger(L, index));
			}
		};

		template < class T >
		struct Getter<typename std::enable_if<std::is_floating_point<T>::value>::type>
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

//Setter:
		template < class T >
		struct Setter {};

		template <>
		struct Setter<bool>
		{
			static void Set(lua_State* L, bool&& value)
			{
				lua_pushboolean(L, value ? 1 : 0);
			}
		};

		template < class T >
		struct Setter<typename std::enable_if<std::is_enum<T>::value>::type>
		{
			static void Set(lua_State* L, T value)
			{
				lua_pushinteger(L, value);
			}
		};

		template < class T >
		struct Setter<typename std::enable_if<std::is_integral<T>::value>::type>
		{
			static void Set(lua_State* L, T value)
			{
				lua_pushinteger(L, value);
			}
		};

		template < class T >
		struct Setter<typename std::enable_if<std::is_floating_point<T>::value>::type>
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
			static void Set(lua_State*L, const Container::String& value)
			{
				lua_pushstring(L, value.CString());
			}
		};
	}
}

#endif
