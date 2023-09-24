#include "Core/DeviceEvent.h"
#include "Core/Context.h"
#include "Core/EventManager.h"

#include <windows.h>

namespace FlagGG
{

Input::Input() :
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
	GetSubsystem<EventManager>()->SendEvent<InputEvent::KEY_DOWN_HANDLER>(keyState, keyCode);
}

void Input::OnKeyUp(KeyState* keyState, UInt32 keyCode)
{
	GetSubsystem<EventManager>()->SendEvent<InputEvent::KEY_UP_HANDLER>(keyState, keyCode);
}

void Input::OnMouseDown(KeyState* keyState, MouseKey mouseKey)
{
	GetSubsystem<EventManager>()->SendEvent<InputEvent::MOUSE_DOWN_HANDLER>(keyState, mouseKey);
}

void Input::OnMouseUp(KeyState* keyState, MouseKey mouseKey)
{
	GetSubsystem<EventManager>()->SendEvent<InputEvent::MOUSE_UP_HANDLER>(keyState, mouseKey);
}

void Input::OnMouseMove(KeyState* keyState, const Vector2& delta)
{
	GetSubsystem<EventManager>()->SendEvent<InputEvent::MOUSE_MOVE_HANDLER>(keyState, delta);
}

}
