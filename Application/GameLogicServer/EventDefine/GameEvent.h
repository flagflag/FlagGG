#pragma once

#include <Core/EventDefine.h>

namespace GameEvent
{
	DEFINE_EVENT(USER_LOGIN, void(Int64));
	DEFINE_EVENT(START_GAME, void(const char*));
	DEFINE_EVENT(STOP_GAME, void(const char*));
	DEFINE_EVENT(START_MOVE, void(Int64));
	DEFINE_EVENT(STOP_MOVE, void(Int64));
}

