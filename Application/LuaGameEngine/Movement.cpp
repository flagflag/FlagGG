#include "Movement.h"
#include "Common.h"
#include "Engine.h"

namespace LuaGameEngine
{

int Movement::Create(lua_State* L)
{
	Engine* engine = GetEngine(L);
	Movement* movement = engine->CreateObject<Movement>();
	SetEntry<Movement>(L, movement);
	return 1;
}

int Movement::Destroy(lua_State* L)
{
	Movement* movement = GetEntry<Movement>(L, 1);
	if (!movement) return 0;
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
	return 1;
}

}
