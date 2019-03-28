#ifndef __LUA_UTIL__
#define __LUA_UTIL__

#include "Container/Str.h"
#include "Lua/ILua/StackCore.h"

namespace FlagGG
{
	namespace Lua
	{
		bool GetGameEvent(lua_State* L, const Container::String& eventName);

		bool CallImpl(lua_State* L, const Container::String& eventName, int paramCount, int returnCount);

		template < int returnCount = 0, class ... Args >
		bool Call(lua_State *L, const Container::String& eventName, const Args& ... args)
		{
			if (!GetGameEvent(L, eventName))
			{
				return false;
			}

			SetParam(L, args ...);

			return CallImpl(L, eventName, sizeof...(args), returnCount);
		}

		template < class T >
		void SetParam(lua_State* L, const T& value)
		{
			Set(L, value);
		}

		template < class T, class ... Args >
		void SetParam(lua_State *L, const T& value, const Args& ... args)
		{
			Set(L, value);

			SetParam(L, args ...);
		}
	}
}

#endif
