#pragma once

#include "EngineObject.h"

namespace LuaGameEngine
{
	class InternalEvent
	{
	public:
		virtual void OnAfterCreateObject(EngineObject* object) = 0;

		virtual void OnBeforeDestroyObject(EngineObject* object) = 0;
	};
}
