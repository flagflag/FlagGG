#pragma once

#include "Controler.h"

#include <lua.hpp>

namespace LuaGameEngine
{
	class ControlerImpl : public Controler
	{
	public:
		ControlerImpl(lua_State* L);

		void StartDirectionMove(Int64 userId, const FlagGG::Math::Vector3& direction) override;

		void StopDirectionMove(Int64 userId) override;

	private:
		lua_State* L_;
	};
}
