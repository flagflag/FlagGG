#pragma once

#include <Core/Context.h>
#include <Scene/Node.h>

using namespace FlagGG::Scene;
using namespace FlagGG::Core;

class Terrain : public Node
{
public:
	Terrain(Context* context);

	void Create(UInt32 pathSize);

private:
	Context* context_;

	UInt32 xCount_;
	UInt32 yCount_;
};
