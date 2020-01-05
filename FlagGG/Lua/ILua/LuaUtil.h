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

		FlagGG_API void SetParam(lua_State* L);

		template < class T >
		void SetParam(lua_State* L, T&& value)
		{
			Set(L, std::forward<T>(value));
		}

		template < class T, class ... Args >
		void SetParam(lua_State *L, T&& value, Args&& ... args)
		{
			Set(L, std::forward<T>(value));

			SetParam(L, std::forward<Args>(args)...);
		}

		template < int returnCount = LUA_MULTRET, class ... Args >
		bool Call(lua_State *L, const Container::String& eventName, Args&& ... args)
		{
			if (!GetGameEvent(L, eventName))
			{
				return false;
			}

			SetParam(L, std::forward<Args>(args)...);

			return CallImpl(L, sizeof...(args), returnCount);
		}

		typedef int(*LuaCFuntion)(lua_State* L);

		struct LuaProxy
		{
			const char* name_;
			LuaCFuntion func_;
		};

		class LuaVM;

		class FlagGG_API LuaVMGuard
		{
		public:
			LuaVMGuard(LuaVM* luaVM, lua_State* luaState);

			~LuaVMGuard();

		private:
			LuaVM* luaVM_;
		};

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
				LuaVMGuard luaVMGuard(luaVM, L);
				return (instance->*func)(luaVM);
			}
		};
	}
}

#define C_LUA_API_PROXY(funcName, luaFuncName) { luaFuncName, funcName }
#define LUA_API_PROXY(className, funcName, luaFuncName) FlagGG::Lua::LuaAPIBinder<className, &className::funcName>::Proxy(luaFuncName)

#endif
