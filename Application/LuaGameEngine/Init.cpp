#include "Init.h"
#include "EngineImpl.h"

#include <Memory/MemoryHook.h>

namespace LuaGameEngine
{

Engine* CreateEngine(lua_State* L)
{
	return new EngineImpl(L);
}

void DestroyEngine(Engine* engine)
{
	delete engine;
}

}

IMPLEMENT_MODULE_USD("LuaGameEngine");
