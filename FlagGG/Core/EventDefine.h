#ifndef __EVENT_DEFINE__
#define __EVENT_DEFINE__

#include "Math/Vector2.h"
#include "Core/Function.h"

#include <stdint.h>
#include <functional>

#define DEFINE_EVENT(eventId, eventNative) \
	static const uint32_t eventId = FlagGG::Core::AddEvent(); \
	typedef FlagGG::Core::Function<eventNative> eventId##_HANDLER;

#define EVENT_HANDLER(eventId, func, ...) eventId, FlagGG::Core::Function<eventId##_HANDLER::NativeType>(&func, ##__VA_ARGS__), ##__VA_ARGS__

namespace FlagGG
{
	namespace Core
	{
		static unsigned AddEvent()
		{
			static unsigned count = 0;
			return ++count;
		}

		enum MouseKey
		{
			MOUSE_LEFT = 0,
			MOUSE_MID,
			MOUSE_RIGHT,
		};

		class KeyState;

		namespace InputEvent
		{
			DEFINE_EVENT(KEY_DOWN,		void(KeyState*, uint32_t));
			DEFINE_EVENT(KEY_UP,		void(KeyState*, uint32_t));
			DEFINE_EVENT(MOUSE_DOWN,	void(KeyState*, MouseKey));
			DEFINE_EVENT(MOUSE_UP,		void(KeyState*, MouseKey));
			DEFINE_EVENT(MOUSE_MOVE,	void(KeyState*, const Math::Vector2&));
		}

		namespace Frame
		{
			DEFINE_EVENT(FRAME_BEGIN,	void(float));
			DEFINE_EVENT(FRAME_END,		void(float));
			DEFINE_EVENT(LOGIC_UPDATE,	void(float));
			DEFINE_EVENT(RENDER_UPDATE, void(float));
		}
	}
}

#endif
