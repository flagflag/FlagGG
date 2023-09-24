#pragma once

#include <Core/Context.h>
#include <Lua/LuaVM.h>
#include <Container/Ptr.h>

#include "GamePlay/GamePlayBase.h"

using namespace FlagGG;

class LuaGamePlay : public Object
{
	OBJECT_OVERRIDE(LuaGamePlay, Object);
public:
	explicit LuaGamePlay();

protected:
	int Login(LuaVM* luaVM);

	int StartGame(LuaVM* luaVM);

	int EndGame(LuaVM* luaVM);

private:
	LuaVM* luaVM_;

	WeakPtr<GamePlayBase> gameplay_;
};
