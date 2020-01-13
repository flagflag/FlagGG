#pragma once

#include <Container/Ptr.h>

using namespace FlagGG::Container;

class GamePlayBase : public RefCounted
{
public:
	virtual void Initialize() = 0;

	virtual void StartGame() = 0;

	virtual void EndGame() = 0;

private:

};
