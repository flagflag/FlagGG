#pragma once

#include <Core/Object.h>
#include <Container/Ptr.h>

namespace FlagGG
{
	
class Scene;
class LuaVM;

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

protected:
	void ExportLuaAPI();

	int GetBrush(LuaVM* luaVM);

private:
	SharedPtr<Brush> brush_;
};
