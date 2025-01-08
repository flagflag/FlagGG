#pragma once

#include "GamePlay/GamePlayBase.h"
#include "GamePlay/World.h"
#include "GameBuilder/GameBuilder.h"

#include <Core/Context.h>
#include <Container/HashMap.h>

namespace LuaGameEngine
{
	class Engine;
}

using namespace FlagGG;

class GamePlayLocal : public GamePlayBase
{
	OBJECT_OVERRIDE(GamePlayLocal, GamePlayBase);
public:
	explicit GamePlayLocal();

	void Initialize(Scene* scene) override;

	void Login(const LuaFunction& callback) override;

	void StartGame() override;

	void EndGame() override;

	void FrameUpdate(float timeStep) override;

private:
	SharedPtr<World> world_;

	LuaGameEngine::Engine* engine_;

	GameBuilder gameBuilder_;
};
