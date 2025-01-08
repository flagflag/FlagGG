#pragma once

#include "GameBuilder/Brush/BrushComponent.h"

namespace FlagGG
{

class Node;

}

class LightBrushComponent : public BrushComponent
{
	OBJECT_OVERRIDE(LightBrushComponent, BrushComponent);
public:
	LightBrushComponent();

	~LightBrushComponent() override;

	// Override BrushComponent interface
	void OnAttach() override;

	void OnDetach() override;

	void OnMouseDown(KeyState* keyState, MouseKey mouseKey) override;

	void OnMouseUp(KeyState* keyState, MouseKey mouseKey) override;

	void OnMouseMove(KeyState* keyState, const Vector2& delta) override;

	void OnWheel() override;

	void OnUpdate(float timeStep) override;

private:
	SharedPtr<Node> controlNode_;
};
