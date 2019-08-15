#include "Lua/ILua/LuaUtil.h"
#include "Log.h"

#include <lua.hpp>
#ifdef WIN32 || WIN64
#include <windows.h>
#endif

namespace FlagGG
{
	namespace Lua
	{
		bool GetGameEvent(lua_State* L, const Container::String& eventName)
		{
			lua_getglobal(L, "game_events");

			if (TypeOf(L, -1) != Type::TABLE)
			{
				FLAGGG_LOG_ERROR("global game_events is ilegall.");

				lua_pop(L, 1);

				return false;
			}

			lua_getfield(L, -1, eventName.CString());

			//if (TypeOf(L, -1) != Type::FUNCTION)
			//{
			//	FLAGGG_LOG_ERROR("try call non-function[game_events.%s].", eventName.CString());

			//	lua_pop(L, 2);

			//	return false;
			//}

			lua_remove(L, -2);

			return true;
		}

		static int TrackBack(lua_State* L)
		{
			const char* errorMessage = lua_tostring(L, -1);
			luaL_traceback(L, L, errorMessage, 1);

			FLAGGG_LOG_ERROR(errorMessage);

#ifdef WIN32 || WIN64
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
				FLAGGG_LOG_ERROR("call lua function failed, error code: %d.", ret);
			}

			lua_remove(L, pos);

			return ret == LUA_OK;
		}

		void SetParam(lua_State* L) {}
	}
}
