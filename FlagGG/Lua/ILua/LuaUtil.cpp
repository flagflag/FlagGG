#include "Lua/ILua/LuaUtil.h"
#include "Lua/ILua/LuaType.h"
#include "Lua/LuaVM.h"
#include "Log.h"

#include <lua.hpp>
#ifdef _WIN32
#include <windows.h>
#endif

namespace FlagGG
{
	namespace Lua
	{
		bool GetGameEvent(lua_State* L, const Container::String& eventName)
		{
			static Container::String nameSpaceOrName, funcName;

			auto nPos = eventName.Find(".");
			nameSpaceOrName = nPos != Container::String::NPOS ? eventName.Substring(0, nPos) : eventName;

			lua_getglobal(L, nameSpaceOrName.CString());

			if (TypeOf(L, -1) == Type::TABLE)
			{
				funcName = eventName.Substring(nPos + 1);
				lua_getfield(L, -1, funcName.CString());
			}
			else
			{
				FLAGGG_LOG_ERROR("global {} is ilegall.", nameSpaceOrName.CString());

				lua_pop(L, 1);
			}

#if 0
			if (TypeOf(L, -1) != Type::FUNCTION)
			{
				FLAGGG_LOG_ERROR("try call non-function[game_events.{}].", eventName.CString());
				Container::String content = "try call non-function[" + eventName + "].";
				MessageBoxA(nullptr, "error", content.CString(), 0);

				lua_pop(L, 2);

				return false;
			}
#endif

			lua_remove(L, -2);

			return true;
		}

		static int TrackBack(lua_State* L)
		{
			const char* errorMessage = lua_tostring(L, -1);
			luaL_traceback(L, L, errorMessage, 1);

			FLAGGG_LOG_ERROR(errorMessage);

#ifdef _WIN32
			MessageBoxA(nullptr, errorMessage, "FlagGG Lua Error", 0);
#endif
			return 0;
		}

		bool CallImpl(lua_State *L, int paramCount, int returnCount)
		{
			int pos = lua_gettop(L) - paramCount;
			lua_pushcfunction(L, TrackBack);
			lua_insert(L, pos);
			int ret = lua_pcall(L, paramCount, returnCount, pos);

			if (ret != LUA_OK)
			{
				FLAGGG_LOG_ERROR("call lua function failed, error code: {}.", ret);
			}

			lua_remove(L, pos);

			return ret == LUA_OK;
		}

		void SetParam(lua_State* L) {}


		LuaVMGuard::LuaVMGuard(LuaVM* luaVM, lua_State* luaState) :
			luaVM_(luaVM)
		{
			luaVM_->callHistory_.Push(luaState);
			luaVM_->luaState_ = luaState;
		}

		LuaVMGuard::~LuaVMGuard()
		{
			luaVM_->callHistory_.Pop();
			luaVM_->luaState_ = luaVM_->callHistory_.Back();
		}
	}
}
