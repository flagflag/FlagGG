#pragma once

#include <Core/Context.h>
#include <Scene/Node.h>

using namespace FlagGG;

class Terrain : public Node
{
public:
	Terrain();

	void Create(UInt32 pathSize, const Vector3& quadSize, const String& heightMap, const String& materialPath);

private:
	UInt32 xCount_;
	UInt32 yCount_;
};
