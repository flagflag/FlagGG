#include <Log.h>

#include "LuaInterface/LuaLog.h"

LuaLog::LuaLog(Context* context) :
	context_(context),
	luaVM_(context->GetVariable<LuaVM>("LuaVM"))
{
	luaVM_->RegisterCPPEvents(
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

int LuaLog::Debug()
{
	FLAGGG_LOG_DEBUG(luaVM_->Get<const char*>(1));
	return 0;
}

int LuaLog::Info()
{
	FLAGGG_LOG_INFO(luaVM_->Get<const char*>(1));
	return 0;
}

int LuaLog::Warn()
{
	FLAGGG_LOG_WARN(luaVM_->Get<const char*>(1));
	return 0;
}

int LuaLog::Error()
{
	FLAGGG_LOG_ERROR(luaVM_->Get<const char*>(1));
	return 0;
}

int LuaLog::Critical()
{
	FLAGG_LOG_CRITICAL(luaVM_->Get<const char*>(1));
	return 0;
}
