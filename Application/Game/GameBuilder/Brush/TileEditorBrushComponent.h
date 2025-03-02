//
// 瓦片地形笔刷
//

#pragma once

#include "GameBuilder/Brush/BrushComponent.h"

class TileEditorBrushComponent : public BrushComponent
{
	OBJECT_OVERRIDE(TileEditorBrushComponent, BrushComponent);
public:
	TileEditorBrushComponent();

	~TileEditorBrushComponent() override;

// Override BrushComponent interface
	void OnAttach() override;

	void OnDetach() override;

	void OnMouseDown(KeyState* keyState, MouseKey mouseKey) override;

	void OnMouseUp(KeyState* keyState, MouseKey mouseKey) override;

	void OnMouseMove(KeyState* keyState, const Vector2& delta) override;

	void OnWheel() override;

	void OnUpdate(float timeStep) override;

private:
	bool isWorking_;
};
