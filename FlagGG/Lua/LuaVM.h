#ifndef __LUA_VM__
#define __LUA_VM__

#include "Export.h"
#include "Container/Ptr.h"
#include "Container/Str.h"
#include "Log.h"
#include "Lua/ILua/StackCore.h"
#include "Lua/ILua/LuaUtil.h"

#include <stdint.h>

struct lua_State;

namespace FlagGG
{
	namespace Lua
	{
		class FlagGG_API LuaVM : public Container::RefCounted
		{
		public:
			LuaVM();

			~LuaVM() override;

			void Open();

			void Close();

			bool IsOpen() const;

			template < class ... Args >
			bool CallEvent(const Container::String& eventName, const Args ... args)
			{
				StackGuard guard(luaState_);

				return Call(luaState_, eventName, args ...);
			}

		private:
			lua_State* luaState_;
		};
	}
}

#endif
