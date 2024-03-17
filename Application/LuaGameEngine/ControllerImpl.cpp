#include "ControllerImpl.h"

#include <Lua/ILua/LuaUtil.h>
#include <Lua/ILua/LuaType.h>

namespace LuaGameEngine
{

ControllerImpl::ControllerImpl(lua_State* L) :
	userTypeRef_("Controller", this, L)
{}

void ControllerImpl::StartDirectionMove(const Vector3& direction)
{
	userTypeRef_.Call("start_direction_move", direction.x_, direction.y_, direction.z_);
}

void ControllerImpl::StopDirectionMove()
{
	userTypeRef_.Call("stop_direction_move");
}


int ControllerImpl::Create(lua_State* L)
{
	ControllerImpl* controller = new ControllerImpl(L);
	luaex_pushusertyperef(L, "Controller", controller);
	return 1;
}

int ControllerImpl::Destroy(lua_State* L)
{
	ControllerImpl* controller = (ControllerImpl*)luaex_tousertype(L, 1, "Controller");
	if (controller)
	{
		controller->ReleaseRef();
	}
	return 0;
}

int ControllerImpl::StartDirectionMove(lua_State* L)
{
	ControllerImpl* controller = (ControllerImpl*)luaex_tousertype(L, 1, "Controller");
	if (controller)
	{
		const Vector3 direction(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
		controller->StartDirectionMove(direction);
	}
	return 0;
}

int ControllerImpl::StopDirectionMove(lua_State* L)
{
	ControllerImpl* controller = (ControllerImpl*)luaex_tousertype(L, 1, "Controller");
	if (controller)
	{
		controller->StopDirectionMove();
	}
	return 0;
}

}
