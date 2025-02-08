#pragma once

#include <Core/Context.h>
#include <Lua/LuaVM.h>
#include <Container/Ptr.h>

#include "GamePlay/GamePlayBase.h"

namespace FlagGG
{

class Scene;

}

using namespace FlagGG;

class LuaGamePlay : public Object
{
	OBJECT_OVERRIDE(LuaGamePlay, Object);
public:
	explicit LuaGamePlay(Scene* scene);

protected:
	int Login(LuaVM* luaVM);

	int StartGame(LuaVM* luaVM);

	int EndGame(LuaVM* luaVM);

	int GetScene(LuaVM* luaVM);

private:
	LuaVM* luaVM_;

	SharedPtr<Scene> scene_;

	WeakPtr<GamePlayBase> gameplay_;
};
