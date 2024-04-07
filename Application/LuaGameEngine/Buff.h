#pragma once 

#include "LGEExport.h"
#include "EngineContextObject.h"

namespace LuaGameEngine
{

class LuaGameEngine_API Buff : public EngineContextObject
{
	OBJECT_OVERRIDE(Buff, EngineContextObject)
public:
	explicit Buff(EngineContext* engineContext);
};

}
