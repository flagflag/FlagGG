#include "Init.h"
#include "EngineImpl.h"

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
