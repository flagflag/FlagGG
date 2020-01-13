#pragma once

#include "GamePlay/GamePlayBase.h"

#include <Core/Context.h>

using namespace FlagGG::Core;

class GamePlayOnline : public GamePlayBase
{
public:
	explicit GamePlayOnline(Context* context);

	void Initialize() override;

	void StartGame() override;

	void EndGame() override;

private:
	Context* context_;
};
