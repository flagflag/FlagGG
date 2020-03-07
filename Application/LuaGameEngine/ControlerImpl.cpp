#include "ControlerImpl.h"

#include <Lua/ILua/LuaUtil.h>

namespace LuaGameEngine
{
	ControlerImpl::ControlerImpl(lua_State* L) :
		L_(L)
	{}

	void ControlerImpl::StartDirectionMove(Int64 userId, const FlagGG::Math::Vector3& direction)
	{
		FlagGG::Lua::Call(L_, "controler.start_direction_move", userId, direction.x_, direction.y_, direction.z_);
	}

	void ControlerImpl::StopDirectionMove(Int64 userId)
	{
		FlagGG::Lua::Call(L_, "controler.stop_direction_move", userId);
	}
}
