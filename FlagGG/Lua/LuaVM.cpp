#include "Lua/LuaVM.h"
#include "Log.h"
#include "Container/ArrayPtr.h"

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

		void LuaVM::RegisterCEvents(const Container::Vector<LuaProxy>& librarys)
		{
			for (uint32_t i = 0; i < librarys.Size(); ++i)
			{
				lua_pushcfunction(luaState_, librarys[i].func_);
				lua_setglobal(luaState_, librarys[i].name_);
			}
		}

		void LuaVM::RegisterCPPEvents(const Container::String& className, void* instance, const Container::Vector<LuaProxy>& librarys)
		{
			lua_newtable(luaState_);
			Container::SharedArrayPtr<luaL_Reg> libs(new luaL_Reg[librarys.Size() + 1]);
			for (uint32_t i = 0; i < librarys.Size(); ++i)
			{
				libs[i] = { librarys[i].name_, librarys[i].func_ };
			}
			libs[librarys.Size()] = { nullptr, nullptr };
			lua_pushlightuserdata(luaState_, (void*)instance);
			luaL_setfuncs(luaState_, libs.Get(), 1);
			lua_setglobal(luaState_, className.CString());
		}

		bool LuaVM::Execute(const Container::String& filePath)
		{
			if (luaL_dofile(luaState_, filePath.CString()))
			{
				FLAGGG_LOG_ERROR("execute lua file failed, error message: %s.", lua_tostring(luaState_, -1));

				lua_pop(luaState_, 1);

				return false;
			}

			return true;
		}
	}
}

