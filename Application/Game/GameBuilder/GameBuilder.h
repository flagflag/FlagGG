#pragma once

#include <Core/Object.h>
#include <Container/Ptr.h>

namespace FlagGG
{
	
class Scene;

}

using namespace FlagGG;

class Brush;

class GameBuilder : public Object
{
	OBJECT_OVERRIDE(GameBuilder, Object);
public:
	GameBuilder();

	~GameBuilder() override;

	void Setup(Scene* scene);

private:
	SharedPtr<Brush> brush_;
};
