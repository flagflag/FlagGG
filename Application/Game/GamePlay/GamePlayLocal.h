#pragma once

#include "GamePlay/GamePlayBase.h"
#include "GamePlay/World.h"

#include <Core/Context.h>
#include <Container/HashMap.h>

namespace LuaGameEngine
{
	class Engine;
}

using namespace FlagGG;

class GamePlayLocal : public GamePlayBase
{
public:
	explicit GamePlayLocal(Context* context);

	void Initialize(Scene* scene) override;

	void Login(const LuaFunction& callback) override;

	void StartGame() override;

	void EndGame() override;

	void FrameUpdate(float timeStep) override;

private:
	Context* context_;

	SharedPtr<World> world_;

	LuaGameEngine::Engine* engine_;
};
