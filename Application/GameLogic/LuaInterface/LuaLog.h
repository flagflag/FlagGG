#pragma once

#include <Core/Context.h>
#include <Lua/LuaVM.h>
#include <Container/Ptr.h>

using namespace FlagGG::Core;
using namespace FlagGG::Lua;
using namespace FlagGG::Container;

class LuaLog : public RefCounted
{
public:
	LuaLog(Context* context);
	
protected:
	int Debug();

	int Info();

	int Warn();

	int Error();

	int Critical();

private:
	Context* context_;

	WeakPtr<LuaVM> luaVM_;
};
