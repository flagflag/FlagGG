#pragma once

#include <Core/Context.h>
#include <Lua/LuaVM.h>
#include <Container/Ptr.h>

#include "GamePlay/GamePlayBase.h"

using namespace FlagGG;

class LuaGamePlay : public RefCounted
{
public:
	explicit LuaGamePlay(Context* context);

protected:
	int Login(LuaVM* luaVM);

	int StartGame(LuaVM* luaVM);

	int EndGame(LuaVM* luaVM);

private:
	Context* context_;

	LuaVM* luaVM_;

	WeakPtr<GamePlayBase> gameplay_;
};
