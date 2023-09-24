#pragma once

#include <Core/Context.h>
#include <Scene/Node.h>

using namespace FlagGG;

class Terrain : public Node
{
public:
	Terrain();

	void Create(UInt32 pathSize);

private:
	UInt32 xCount_;
	UInt32 yCount_;
};
