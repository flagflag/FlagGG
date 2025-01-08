#include "GameBuilder.h"
#include "GameBuilder/Brush/Brush.h"
#include "GameBuilder/Brush/TerrainBrushComponent.h"
#include "GameBuilder/Brush/LightBrushComponent.h"

GameBuilder::GameBuilder()
{
	
}

GameBuilder::~GameBuilder()
{

}

void GameBuilder::Setup(Scene* scene)
{
	brush_ = new Brush(scene);

	auto lightBrushComponent = MakeShared<LightBrushComponent>();
	brush_->AttachComponent(lightBrushComponent);
}
