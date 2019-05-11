#include "Core/DeviceEvent.h"
#include "Core/Context.h"

namespace FlagGG
{
	namespace Core
	{
		Input::Input(Context* context) :
			context_(context)
		{ }

		void Input::OnKeyDown(KeyState* keyState, uint32_t keyCode)
		{
			context_->SendEvent<InputEvent::KEY_DOWN_HANDLER>(InputEvent::KEY_DOWN, keyState, keyCode);
		}

		void Input::OnKeyUp(KeyState* keyState, uint32_t keyCode)
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
