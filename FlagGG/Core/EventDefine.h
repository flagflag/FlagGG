#pragma once

#include "Math/Vector2.h"
#include "Core/EventCore.h"

namespace FlagGG
{

enum MouseKey
{
	MOUSE_LEFT = 0,
	MOUSE_MID,
	MOUSE_RIGHT,
};

class KeyState;
class Window;

namespace InputEvent
{
	DEFINE_EVENT(KEY_DOWN,		void(KeyState*, UInt32));
	DEFINE_EVENT(KEY_UP,		void(KeyState*, UInt32));
	DEFINE_EVENT(KEY_CHAR,      void(KeyState*, UInt32));
	DEFINE_EVENT(MOUSE_DOWN,	void(KeyState*, MouseKey, const IntVector2&));
	DEFINE_EVENT(MOUSE_UP,		void(KeyState*, MouseKey, const IntVector2&));
	DEFINE_EVENT(MOUSE_MOVE,	void(KeyState*, const IntVector2&, const Vector2&));
	DEFINE_EVENT(MOUSE_WHEEL,   void(KeyState*, Int32));
	DEFINE_EVENT(SET_FOCUS,     void(Window*));
	DEFINE_EVENT(KILL_FOCUS,    void(Window*));
}

namespace Frame
{
	DEFINE_EVENT(BEGIN_FRAME,	void(float));
	DEFINE_EVENT(END_FRAME,		void(float));
	DEFINE_EVENT(LOGIC_UPDATE,	void(float));
	DEFINE_EVENT(PRERENDER_UPDATE, void(float));
	DEFINE_EVENT(RENDER_UPDATE, void(float));
}

namespace Application
{
	DEFINE_EVENT(ENGINE_EXIT,  void());
	DEFINE_EVENT(WINDOW_CLOSE, void(void*));
}

}

