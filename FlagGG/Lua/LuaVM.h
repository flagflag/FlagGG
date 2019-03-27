#ifndef __LUA_VM__
#define __LUA_VM__

#include "Export.h"
#include "Container/Str.h"
#include "Log.h"
#include "Lua/ILua/StackCore.h"

#include <stdint.h>
#include <lualib.h>

class lua_State;

namespace FlagGG
{
	namespace Lua
	{
		class FlagGG_API LuaVM
		{
		public:
			template < class ... Args >
			bool CallEvent(const Container::String& eventName, Args ... args)
			{
				if (!CheckType(luaState_, -1))
				{
					return false;
				}

				SetFunctionParam(args ...);

				return true;
			}

		protected:
			template < class T, class ... Args >
			void SetFunctionParam(T&& value, Args ... args)
			{
				Setter::Set(luaState_, value);

				SetFunctionParam(args ...);
			}

		private:
			lua_State* luaState_;
		};
	}
}

#endif
