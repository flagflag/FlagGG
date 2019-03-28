#include "Lua/ILua/LuaType.h"

namespace FlagGG
{
	namespace Lua
	{
		Type::_ TypeOf(lua_State* L, int index)
		{
			return lua_type(L, index);
		}
	}
}

