#pragma once 

#include "LGEExport.h"
#include "EngineObject.h"

#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <lua.hpp>

namespace LuaGameEngine
{

class Movement
{
public:
	static int Destroy(lua_State* L);
	static int Start(lua_State* L);
	static int Stop(lua_State* L);
	static int IsActive(lua_State* L);

	static int CreateDirectionMovement(lua_State* L);
	static int SetMoveDirection(lua_State* L);
};

}
