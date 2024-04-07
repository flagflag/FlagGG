#pragma once 

#include "LGEExport.h"
#include "EngineContextObject.h"

namespace LuaGameEngine
{

class LuaGameEngine_API Spell : public EngineContextObject
{
	OBJECT_OVERRIDE(Spell, EngineContextObject);
public:
	explicit Spell(EngineContext* engineContext);
};

}
