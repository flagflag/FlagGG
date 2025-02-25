#pragma once

#include <Core/Object.h>
#include <Core/DeviceEvent.h>

using namespace FlagGG;

class Brush;

class BrushComponent : public Object
{
	OBJECT_OVERRIDE(BrushComponent, Object);
public:
	BrushComponent();

	~BrushComponent() override;

	void Detach();

	void SetOwnerBrush(Brush* ownerBrush);

// Brush interface
	virtual void OnAttach() {}

	virtual void OnDetach() {}

	virtual void OnMouseDown(KeyState* keyState, MouseKey mouseKey) {}

	virtual void OnMouseUp(KeyState* keyState, MouseKey mouseKey) {}

	virtual void OnMouseMove(KeyState* keyState, const Vector2& delta) {}

	virtual void OnWheel() {}

	virtual void OnKeyDown(KeyState* keyState, UInt32 keyCode) {}

	virtual void OnKeyUp(KeyState* keyState, UInt32 keyCode) {}

	virtual void OnUpdate(float timeStep) {}

protected:
	Brush* ownerBrush_;
};
