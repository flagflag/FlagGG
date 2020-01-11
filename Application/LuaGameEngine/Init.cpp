#include "Init.h"
#include "Unit.h"
#include "Common.h"

namespace LuaGameEngine
{
	static void CreateEngineEntry(lua_State* L)
	{
		lua_newtable(L);
		lua_setglobal(L, "engine");
	}

	static void CreateUnitClass(lua_State* L)
	{
		const luaL_Reg lib[] =
		{
			{ "create", &Unit::Create },
			{ "get_name", &Unit::GetName },
			{ "get_position", &Unit::GetPosition },
			{ "get_rotation", &Unit::GetRotation },
			{ "get_scale", &Unit::GetScale },
			{ "set_name", &Unit::SetName },
			{ "set_position", &Unit::SetPosition },
			{ "set_rotation", &Unit::SetRotation },
			{ "set_scale", &Unit::SetScale },
			{ nullptr, nullptr, }
		};
		CreateClass(L, "unit", lib, &Unit::Destroy);
	}

	void InitEngine(lua_State* L)
	{
		CreateEngineEntry(L);
		CreateUnitClass(L);
	}

	void UninitEngien(lua_State* L)
	{

	}
}
