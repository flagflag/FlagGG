#include "Core/DeviceEvent.h"
#include "Core/Context.h"

#include <windows.h>

namespace FlagGG
{
	namespace Core
	{
		Input::Input(Context* context) :
			context_(context)
		{ }

		Math::IntVector2 Input::GetMousePos() const
		{
			POINT point;
			::GetCursorPos(&point);
			return Math::IntVector2(point.x, point.y);
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

		void Input::OnMouseMove(KeyState* keyState, const Math::Vector2& delta)
		{
			context_->SendEvent<InputEvent::MOUSE_MOVE_HANDLER>(InputEvent::MOUSE_MOVE, keyState, delta);
		}
	}
}
