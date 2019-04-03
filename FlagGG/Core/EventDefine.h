#ifndef __EVENT_DEFINE__
#define __EVENT_DEFINE__

#include "Math/Vector2.h"

#include <stdint.h>
#include <functional>

#define DEFINE_EVENT(eventId, eventNative) \
	static const uint32_t eventId  = FlagGG::Core::AddEvent(); \
	typedef std::function<eventNative> eventId##_HANDLER;

#define EVENT_HANDLER(func, ...) std::bind(&func, ##__VA_ARGS__), ##__VA_ARGS__

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
	}
}

#endif
