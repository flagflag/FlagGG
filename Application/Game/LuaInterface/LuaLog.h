#pragma once

#include <Core/Object.h>
#include <Core/Context.h>
#include <Lua/LuaVM.h>
#include <Container/Ptr.h>

using namespace FlagGG;

class LuaLog : public Object
{
	OBJECT_OVERRIDE(LuaLog, Object);
public:
	explicit LuaLog();
	
protected:
	int Debug(LuaVM* luaVM);

	int Info(LuaVM* luaVM);

	int Warn(LuaVM* luaVM);

	int Error(LuaVM* luaVM);

	int Critical(LuaVM* luaVM);

	int Alert(LuaVM* luaVM);
};
