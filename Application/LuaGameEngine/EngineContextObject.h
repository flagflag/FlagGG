#pragma once

#include "EngineObject.h"

namespace LuaGameEngine
{

struct EngineContext;

class EngineContextObject : public EngineObject
{
	OBJECT_OVERRIDE(EngineContextObject, EngineObject);
public:
	explicit EngineContextObject(EngineContext* engineContext);

protected:
	EngineContext* engineContext_;
};

}
