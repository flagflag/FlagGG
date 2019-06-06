#include "Lua/ILua/LuaType.h"

namespace FlagGG
{
	namespace Lua
	{
		LuaFunction::LuaFunction() :
			luaState_(nullptr),
			ref_(0),
			refCount_(nullptr)
		{ }

		LuaFunction::LuaFunction(lua_State* luaState, int32_t idx) :
			luaState_(luaState),
			refCount_(new int32_t{ 1 })
		{
			lua_pushvalue(luaState_, idx);
			ref_ = luaL_ref(luaState_, LUA_REGISTRYINDEX);
		}

		LuaFunction::LuaFunction(const LuaFunction& func) :
			refCount_(nullptr)
		{
			*(this) = func;
		}

		LuaFunction::~LuaFunction()
		{
			Release();
		}

		LuaFunction& LuaFunction::operator=(const LuaFunction& func)
		{
			Release();

			luaState_ = func.luaState_;
			ref_ = func.ref_;
			refCount_ = func.refCount_;
			if (refCount_)
				++(*refCount_);

			return (*this);
		}

		void LuaFunction::Release()
		{
			if (refCount_)
			{
				--(*refCount_);
				if ((*refCount_) == 0)
				{
					delete refCount_;
					refCount_ = 0;

					if (ref_)
					{
						luaL_unref(luaState_, LUA_REGISTRYINDEX, ref_);
						ref_ = 0;
					}
				}
			}
		}
	}
}

