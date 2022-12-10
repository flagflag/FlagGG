#pragma once

#include <Core/Context.h>
#include <Lua/LuaVM.h>
#include <Container/Ptr.h>

using namespace FlagGG;

class LuaLog : public RefCounted
{
public:
	explicit LuaLog(Context* context);
	
protected:
	int Debug(LuaVM* luaVM);

	int Info(LuaVM* luaVM);

	int Warn(LuaVM* luaVM);

	int Error(LuaVM* luaVM);

	int Critical(LuaVM* luaVM);

private:
	Context* context_;
};
