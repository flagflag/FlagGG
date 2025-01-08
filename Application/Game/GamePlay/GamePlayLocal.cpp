#include "GamePlay/GamePlayLocal.h"

#include <LuaGameEngine/Init.h>

GamePlayLocal::GamePlayLocal()
{
	LuaVM* luaVM = GetSubsystem<Context>()->GetVariable<LuaVM>("LuaVM");
	engine_ = LuaGameEngine::CreateEngine(*luaVM);
}

void GamePlayLocal::Initialize(Scene* scene)
{
	world_ = new World();
	world_->SetScene(scene);
	gameBuilder_.Setup(scene);
}

void GamePlayLocal::Login(const LuaFunction& callback)
{

}

void GamePlayLocal::StartGame()
{

}

void GamePlayLocal::EndGame()
{
}

void GamePlayLocal::FrameUpdate(float timeStep)
{

}
