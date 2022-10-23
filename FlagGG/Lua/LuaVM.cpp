#include "Lua/LuaVM.h"
#include "Lua/LuaBinding/LuaBinding.h"
#include "Lua/LuaBinding/LuaExtend.h"
#include "Log.h"
#include "Container/ArrayPtr.h"
#include "Core/BaseTypes.h"
#include "Core/CryAssert.h"

namespace FlagGG
{
	namespace Lua
	{
#include"Lua/LuaBinding/Class.lua"

		static int luaex_GetContext(lua_State* L)
		{
			luaex_pushcontext(L);
			return 1;
		}

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

			luaex_globalfunction(luaState_, "GetContext", luaex_GetContext);
			ExecuteScript(ClassDefine);

			callHistory_.Clear();
			callHistory_.Push(luaState_);
		}

		void LuaVM::Close()
		{
			if (luaState_)
			{
				lua_close(luaState_);
				luaState_ = nullptr;
			}

			callHistory_.Clear();
		}

		bool LuaVM::IsOpen() const
		{
			return !!luaState_;
		}

		void LuaVM::RegisterCEvents(const Container::Vector<LuaProxy>& librarys)
		{
			for (UInt32 i = 0; i < librarys.Size(); ++i)
			{
				lua_pushcfunction(luaState_, librarys[i].func_);
				lua_setglobal(luaState_, librarys[i].name_);
			}
		}

		void LuaVM::RegisterCPPEvents(const Container::String& className, void* instance, const Container::Vector<LuaProxy>& librarys)
		{
			lua_newtable(luaState_);
			Container::SharedArrayPtr<luaL_Reg> libs(new luaL_Reg[librarys.Size() + 1]);
			for (UInt32 i = 0; i < librarys.Size(); ++i)
			{
				libs[i] = { librarys[i].name_, librarys[i].func_ };
			}
			libs[librarys.Size()] = { nullptr, nullptr };
			lua_pushlightuserdata(luaState_, (void*)instance);
			lua_pushlightuserdata(luaState_, (void*)this);
			luaL_setfuncs(luaState_, libs.Get(), 2);
			lua_setglobal(luaState_, className.CString());
		}

		bool LuaVM::Execute(const Container::String& filePath)
		{
			if (luaL_dofile(luaState_, filePath.CString()))
			{
				FLAGGG_LOG_ERROR("execute lua file failed, error message: {}.", lua_tostring(luaState_, -1));
				CRY_ASSERT_MESSAGE(false, "execute lua file failed");

				lua_pop(luaState_, 1);

				return false;
			}

			return true;
		}

		bool LuaVM::ExecuteScript(const Container::String& scriptCode)
		{
			if (luaL_dostring(luaState_, scriptCode.CString()))
			{
				FLAGGG_LOG_ERROR("execute lua code failed, error message: {}.", lua_tostring(luaState_, -1));
				CRY_ASSERT_MESSAGE(false, "execute lua code failed");

				lua_pop(luaState_, 1);

				return false;
			}

			return true;
		}

		void LuaVM::SetLoaderPath(const Container::String& loaderPath)
		{
			Container::String modePath = loaderPath + "?.lua";
			lua_getglobal(luaState_, "package");
			lua_pushstring(luaState_, modePath.CString());
			lua_setfield(luaState_, -2, "path");
			lua_pop(luaState_, 1);
		}
	}
}

