#ifndef __LUA_UTIL__
#define __LUA_UTIL__

#include "Export.h"
#include "Container/Str.h"
#include "Lua/ILua/StackSet.h"

namespace FlagGG
{
	namespace Lua
	{
		FlagGG_API bool GetGameEvent(lua_State* L, const Container::String& eventName);

		FlagGG_API bool CallImpl(lua_State* L, int paramCount, int returnCount);

		template < int returnCount = 0, class ... Args >
		bool Call(lua_State *L, const Container::String& eventName, const Args& ... args)
		{
			if (!GetGameEvent(L, eventName))
			{
				return false;
			}

			SetParam(L, args ...);

			return CallImpl(L, sizeof...(args), returnCount);
		}

		FlagGG_API void SetParam(lua_State* L);

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

		typedef int(*LuaCFuntion)(lua_State* L);

		struct LuaProxy
		{
			const char* name_;
			LuaCFuntion func_;
		};

		class LuaVM;

		template < class T, int(T::*func)(LuaVM*) >
		class LuaAPIBinder
		{
		public:
			static LuaProxy Proxy(const char* name)
			{
				return { name, &Proxy_ };
			}

		private:
			static int Proxy_(lua_State* L)
			{
				T* instance = static_cast<T*>(lua_touserdata(L, lua_upvalueindex(1)));
				LuaVM* luaVM = static_cast<LuaVM*>(lua_touserdata(L, lua_upvalueindex(2)));
				return (instance->*func)(luaVM);
			}
		};
	}
}

#define C_LUA_API_PROXY(funcName, luaFuncName) { luaFuncName, funcName }
#define LUA_API_PROXY(className, funcName, luaFuncName) FlagGG::Lua::LuaAPIBinder<className, &className::funcName>::Proxy(luaFuncName)

#endif
