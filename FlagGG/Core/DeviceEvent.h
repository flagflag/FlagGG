#pragma once

#include "Export.h"
#include "Core/Context.h"
#include "Core/EventDefine.h"
#include "Core/Subsystem.h"
#include "Math/Vector2.h"
#include "Container/RefCounted.h"

namespace FlagGG
{

class Context;

class FlagGG_API KeyState
{
public:
	bool OnCtrl();

	bool OnAlt();

	bool OnShift();

private:

};

class FlagGG_API Input : public Subsystem<Input>
{
public:
	Input();

	~Input() override = default;

	IntVector2 GetMousePos() const;

	void ShowMouse();

	void HideMouse();

	bool IsMouseShow() const;

	void OnKeyDown(KeyState* keyState, UInt32 keyCode);

	void OnKeyUp(KeyState* keyState, UInt32 keyCode);

	void OnMouseDown(KeyState* keyState, MouseKey mouseKey);

	void OnMouseUp(KeyState* keyState, MouseKey mouseKey);

	void OnMouseMove(KeyState* keyState, const Vector2& delta);

private:
	bool isMouseShow_;
};

}
