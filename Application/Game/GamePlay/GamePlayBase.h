#pragma once

#include <Container/Ptr.h>
#include <Lua/LuaVM.h>
#include <Scene/Scene.h>

using namespace FlagGG::Container;
using namespace FlagGG::Lua;

class GamePlayBase : public RefCounted
{
public:
	virtual void Initialize(FlagGG::Scene::Scene* scene) = 0;

	virtual void Login(const LuaFunction& callback) = 0;

	virtual void StartGame() = 0;

	virtual void EndGame() = 0;

private:

};
