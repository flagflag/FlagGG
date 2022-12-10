#include "Core/DeviceEvent.h"
#include "Core/Context.h"

#include <windows.h>

namespace FlagGG
{

Input::Input(Context* context) :
	context_(context),
	isMouseShow_(true)
{ }

IntVector2 Input::GetMousePos() const
{
	POINT point;
	::GetCursorPos(&point);
	return IntVector2(point.x, point.y);
}

void Input::ShowMouse()
{
	::ShowCursor(TRUE);
	isMouseShow_ = true;
}

void Input::HideMouse()
{
	::ShowCursor(FALSE);
	isMouseShow_ = false;
}

bool Input::IsMouseShow() const
{
	return isMouseShow_;
}

void Input::OnKeyDown(KeyState* keyState, UInt32 keyCode)
{
	context_->SendEvent<InputEvent::KEY_DOWN_HANDLER>(InputEvent::KEY_DOWN, keyState, keyCode);
}

void Input::OnKeyUp(KeyState* keyState, UInt32 keyCode)
{
	context_->SendEvent<InputEvent::KEY_UP_HANDLER>(InputEvent::KEY_UP, keyState, keyCode);
}

void Input::OnMouseDown(KeyState* keyState, MouseKey mouseKey)
{
	context_->SendEvent<InputEvent::MOUSE_DOWN_HANDLER>(InputEvent::MOUSE_DOWN, keyState, mouseKey);
}

void Input::OnMouseUp(KeyState* keyState, MouseKey mouseKey)
{
	context_->SendEvent<InputEvent::MOUSE_UP_HANDLER>(InputEvent::MOUSE_UP, keyState, mouseKey);
}

void Input::OnMouseMove(KeyState* keyState, const Vector2& delta)
{
	context_->SendEvent<InputEvent::MOUSE_MOVE_HANDLER>(InputEvent::MOUSE_MOVE, keyState, delta);
}

}
