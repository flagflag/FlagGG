#include "Movement.h"
#include "Common.h"
#include "Engine.h"
#include "Movement/DirectionMovement.h"

namespace LuaGameEngine
{

int Movement::Destroy(lua_State* L)
{
	BaseMovement* movement = (BaseMovement*)luaex_tousertype(L, 1, "BaseMovement");
	if (movement)
	{
		movement->ReleaseRef();
	}
	return 0;
}

int Movement::Start(lua_State* L)
{
	BaseMovement* movement = (BaseMovement*)luaex_tousertype(L, 1, "BaseMovement");
	if (movement)
	{
		movement->Start();
	}
	return 0;
}

int Movement::Stop(lua_State* L)
{
	BaseMovement* movement = (BaseMovement*)luaex_tousertype(L, 1, "BaseMovement");
	if (movement)
	{
		movement->Stop();
	}
	return 0;
}

int Movement::IsActive(lua_State* L)
{
	BaseMovement* movement = (BaseMovement*)luaex_tousertype(L, 1, "BaseMovement");
	lua_pushboolean(L, movement ? movement->IsActive() : false);
	return 1;
}

int Movement::CreateDirectionMovement(lua_State* L)
{ 
	DirectionMovement* movement = new DirectionMovement();
	luaex_pushusertyperef(L, "DirectionMovement", movement);
	return 1;
}

int Movement::SetMoveDirection(lua_State* L)
{
	DirectionMovement* movement = (DirectionMovement*)luaex_tousertype(L, 1, "DirectionMovement");
	if (movement)
	{
		const Vector3 moveDir(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
		movement->SetMoveDirection(moveDir);
	}
	return 0;
}

}
