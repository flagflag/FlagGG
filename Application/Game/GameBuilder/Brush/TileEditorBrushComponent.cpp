#include "TileEditorBrushComponent.h"
#include "Brush.h"

#include <Scene/Scene.h>

TileEditorBrushComponent::TileEditorBrushComponent()
	: isWorking_(false)
{

}

TileEditorBrushComponent::~TileEditorBrushComponent()
{

}

void TileEditorBrushComponent::OnAttach()
{

}

void TileEditorBrushComponent::OnDetach()
{

}

void TileEditorBrushComponent::OnMouseDown(KeyState* keyState, MouseKey mouseKey)
{
	if (mouseKey == MOUSE_LEFT)
		isWorking_ = true;
}

void TileEditorBrushComponent::OnMouseUp(KeyState* keyState, MouseKey mouseKey)
{
	if (mouseKey == MOUSE_LEFT)
		isWorking_ = false;
}

void TileEditorBrushComponent::OnMouseMove(KeyState* keyState, const Vector2& delta)
{

}

void TileEditorBrushComponent::OnWheel()
{

}

void TileEditorBrushComponent::OnUpdate(float timeStep)
{

}
