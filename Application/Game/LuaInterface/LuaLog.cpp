#include <Log.h>

#include "LuaInterface/LuaLog.h"

LuaLog::LuaLog(Context* context) :
	context_(context)
{
	LuaVM* luaVM = context->GetVariable<LuaVM>("LuaVM");
	luaVM->RegisterCPPEvents(
		"log",
		this,
		{
			LUA_API_PROXY(LuaLog, Debug, "debug"),
			LUA_API_PROXY(LuaLog, Info, "info"),
			LUA_API_PROXY(LuaLog, Warn, "warn"),
			LUA_API_PROXY(LuaLog, Error, "error"),
			LUA_API_PROXY(LuaLog, Critical, "critical")
		}
	);
}

int LuaLog::Debug(LuaVM* luaVM)
{
	FLAGGG_LOG_DEBUG(luaVM->Get<const char*>(1));
	return 0;
}

int LuaLog::Info(LuaVM* luaVM)
{
	FLAGGG_LOG_INFO(luaVM->Get<const char*>(1));
	return 0;
}

int LuaLog::Warn(LuaVM* luaVM)
{
	FLAGGG_LOG_WARN(luaVM->Get<const char*>(1));
	return 0;
}

int LuaLog::Error(LuaVM* luaVM)
{
	FLAGGG_LOG_ERROR(luaVM->Get<const char*>(1));
	return 0;
}

int LuaLog::Critical(LuaVM* luaVM)
{
	FLAGGG_LOG_CRITICAL(luaVM->Get<const char*>(1));
	return 0;
}
