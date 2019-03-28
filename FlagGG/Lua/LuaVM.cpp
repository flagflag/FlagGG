#include "Lua/LuaVM.h"

#include <lauxlib.h>

namespace FlagGG
{
	namespace Lua
	{
		LuaVM::LuaVM() :
			luaState_(nullptr)
		{ }

		LuaVM::~LuaVM()
		{
			Close();
		}

		void LuaVM::Open()
		{
			luaState_ = luaL_newstate();
			luaL_openlibs(luaState_);

			lua_newtable(luaState_);
			lua_setglobal(luaState_, "game_events");
		}

		void LuaVM::Close()
		{
			if (luaState_)
			{
				lua_close(luaState_);
				luaState_ = nullptr;
			}
		}

		bool LuaVM::IsOpen() const
		{
			return !!luaState_;
		}
	}
}

