#pragma once

#include <Container/Vector.h>

using namespace FlagGG;

namespace LuaGameEngine
{

class LuaEventHandler;

struct EngineContext
{
	PODVector<LuaEventHandler*> handlers_;
};

}

