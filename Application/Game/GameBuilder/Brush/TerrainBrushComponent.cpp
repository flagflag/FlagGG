#include "TerrainBrushComponent.h"
#include "Brush.h"

#include <Scene/Scene.h>
#include <Scene/TerrainComponent.h>

TerrainBrushComponent::TerrainBrushComponent()
	: isWorking_(false)
	, terrainComponent_(nullptr)
{

}

TerrainBrushComponent::~TerrainBrushComponent()
{

}

void TerrainBrushComponent::OnAttach()
{
	terrainComponent_ = ownerBrush_->GetOwnerScene()->GetComponentRecursive<TerrainComponent>();
}

void TerrainBrushComponent::OnDetach()
{

}

void TerrainBrushComponent::OnMouseDown(KeyState* keyState, MouseKey mouseKey)
{
	if (mouseKey == MOUSE_LEFT)
		isWorking_ = true;
}

void TerrainBrushComponent::OnMouseUp(KeyState* keyState, MouseKey mouseKey)
{
	if (mouseKey == MOUSE_LEFT)
		isWorking_ = false;
}

void TerrainBrushComponent::OnMouseMove(KeyState* keyState, const Vector2& delta)
{

}

void TerrainBrushComponent::OnWheel()
{

}

void TerrainBrushComponent::OnUpdate(float timeStep)
{

}
