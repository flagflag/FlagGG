#pragma once

#include "Controller.h"

#include <lua.hpp>
#include <Lua/ILua/LuaType.h>

namespace LuaGameEngine
{

class ControllerImpl : public Controller
{
public:
	ControllerImpl(lua_State* L);

	void StartDirectionMove(const Vector3& direction) override;

	void StopDirectionMove() override;


	static int Create(lua_State* L);
	static int Destroy(lua_State* L);
	static int StartDirectionMove(lua_State* L);
	static int StopDirectionMove(lua_State* L);

private:
	UserTypeRef userTypeRef_;
};

}
