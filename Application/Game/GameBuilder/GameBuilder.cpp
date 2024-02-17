#include "GameBuilder.h"
#include "GameBuilder/Brush/Brush.h"
#include "GameBuilder/Brush/TerrainBrushComponent.h"

GameBuilder::GameBuilder()
{
	
}

GameBuilder::~GameBuilder()
{

}

void GameBuilder::Setup(Scene* scene)
{
	brush_ = new Brush(scene);

	auto terrainBrushComponent = MakeShared<TerrainBrushComponent>();
	brush_->AttachComponent(terrainBrushComponent);
}
