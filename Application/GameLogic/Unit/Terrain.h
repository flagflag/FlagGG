#pragma once

#include <Core/Context.h>
#include <Scene/Node.h>

using namespace FlagGG::Scene;
using namespace FlagGG::Core;

class Terrain : public Node
{
public:
	Terrain(Context* context);

	void Create(uint32_t pathSize);

private:
	Context* context_;

	uint32_t xCount_;
	uint32_t yCount_;
};
