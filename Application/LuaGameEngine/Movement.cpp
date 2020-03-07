#include "Movement.h"
#include "Common.h"
#include "Engine.h"

namespace LuaGameEngine
{
	int Movement::Create(lua_State* L)
	{
		lua_newtable(L);
		lua_getmetatable(L, 1);
		lua_setmetatable(L, -2);
		Engine* engine = GetEngine(L);
		Movement* movement = engine->CreateObject<Movement>();
		movement->AddRef();
		SetEntry(L, -1, movement);
		return 1;
	}

	int Movement::Destroy(lua_State* L)
	{
		Movement* movement = GetEntry<Movement>(L, 1);
		movement->ReleaseRef();
		Engine* engine = GetEngine(L);
		engine->DestroyObject(movement);
		return 0;
	}
	
	int Movement::Start(lua_State* L)
	{
		Movement* movement = GetEntry<Movement>(L, 1);
		movement->Start();
		return 0;
	}

	int Movement::Stop(lua_State* L)
	{
		Movement* movement = GetEntry<Movement>(L, 1);
		movement->Stop();
		return 0;
	}

	int Movement::IsActive(lua_State* L)
	{
		Movement* movement = GetEntry<Movement>(L, 1);
		lua_pushboolean(L, movement->IsActive());
		return 0;
	}
}
