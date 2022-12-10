#include "GamePlay/GamePlayLocal.h"

#include <LuaGameEngine/Init.h>

GamePlayLocal::GamePlayLocal(Context* context) :
	context_(context)
{
	LuaVM* luaVM = context_->GetVariable<LuaVM>("LuaVM");
	engine_ = LuaGameEngine::CreateEngine(*luaVM);
}

void GamePlayLocal::Initialize(Scene* scene)
{
	world_ = new World(context_);
	world_->SetScene(scene);
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
