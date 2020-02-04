#pragma once

#include <Core/EventDefine.h>

namespace CommandEvent
{
	DEFINE_EVENT(USER_LOGIN, void(Int64));
	DEFINE_EVENT(START_GAME, void(const char*));
	DEFINE_EVENT(STOP_GAME, void());
	DEFINE_EVENT(START_MOVE, void(Int64));
	DEFINE_EVENT(STOP_MOVE, void(Int64));
}
