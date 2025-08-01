#include "Core/DeviceEvent.h"
#include "Core/Context.h"
#include "Core/EventManager.h"
#if PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace FlagGG
{

RawMsgParam::RawMsgParam()
	: rawParams_{}
	, rawParamCount_(0)
{

}

void RawMsgParam::Clear()
{
	rawParamCount_ = 0;
}

void RawMsgParam::AddRawParam(UInt32 paramValue)
{
	rawParams_[rawParamCount_++] = paramValue;
}

UInt32 RawMsgParam::GetParamCount() const
{
	return rawParamCount_;
}

UInt64 RawMsgParam::GetRawParam(UInt32 index)
{
	return rawParams_[index];
}

KeyState::KeyState(Window* sender, bool ctrl, bool alt, bool shift, RawMsgParam* rawMsgParam)
	: sender_(sender)
	, ctrl_(ctrl)
	, alt_(alt)
	, shift_(shift)
	, rawMsgParam_(rawMsgParam)
{

}

Input::Input() :
	isMouseShow_(true)
{ }

IntVector2 Input::GetMousePos() const
{
#if PLATFORM_WINDOWS
	POINT point;
	::GetCursorPos(&point);
	return IntVector2(point.x, point.y);
#else
	return IntVector2::ZERO;
#endif
}

void Input::ShowMouse()
{
#if PLATFORM_WINDOWS
	::ShowCursor(TRUE);
	isMouseShow_ = true;
#endif
}

void Input::HideMouse()
{
#if PLATFORM_WINDOWS
	::ShowCursor(FALSE);
	isMouseShow_ = false;
#endif
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

void Input::OnChar(KeyState* keyState, UInt32 keyCode)
{
	GetSubsystem<EventManager>()->SendEvent<InputEvent::KEY_CHAR_HANDLER>(keyState, keyCode);
}

void Input::OnMouseDown(KeyState* keyState, MouseKey mouseKey, const IntVector2& mousePos)
{
	GetSubsystem<EventManager>()->SendEvent<InputEvent::MOUSE_DOWN_HANDLER>(keyState, mouseKey, mousePos);
}

void Input::OnMouseUp(KeyState* keyState, MouseKey mouseKey, const IntVector2& mousePos)
{
	GetSubsystem<EventManager>()->SendEvent<InputEvent::MOUSE_UP_HANDLER>(keyState, mouseKey, mousePos);
}

void Input::OnMouseMove(KeyState* keyState, const IntVector2& mousePos, const Vector2& delta)
{
	GetSubsystem<EventManager>()->SendEvent<InputEvent::MOUSE_MOVE_HANDLER>(keyState, mousePos, delta);
}

void Input::OnMouseWheel(KeyState* keyState, Int32 delta)
{
	GetSubsystem<EventManager>()->SendEvent<InputEvent::MOUSE_WHEEL_HANDLER>(keyState, delta);
}

void Input::OnSetFocus(Window* window)
{
	GetSubsystem<EventManager>()->SendEvent<InputEvent::SET_FOCUS_HANDLER>(window);
}

void Input::OnKillFocus(Window* window)
{
	GetSubsystem<EventManager>()->SendEvent<InputEvent::KILL_FOCUS_HANDLER>(window);
}

}
