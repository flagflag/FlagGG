#pragma once

#include "LGEExport.h"
#include "EngineObject.h"
#include "Unit.h"

#include <Core/BaseTypes.h>
#include <Container/Str.h>
#include <lua.hpp>

namespace LuaGameEngine
{

class LuaGameEngine_API Player : public EngineObject
{
	OBJECT_OVERRIDE(Player, EngineObject);
public:
	~Player() override;

	Int64 GetUserId() const { return userId_; }

	const String& GetUserName() { return userName_; }

	Unit* GetControlUnit() const { return controlUnit_; }

	void SetUserId(Int64 userId);

	void SetUserName(const String& userName);

	void SetControlUnit(Unit* unit);


	static int Create(lua_State* L);
	static int Destroy(lua_State* L);
	static int GetUserId(lua_State* L);
	static int GetUserName(lua_State* L);
	static int GetControlUnit(lua_State* L);
	static int SetUserId(lua_State* L);
	static int SetUserName(lua_State* L);
	static int SetControlUnit(lua_State* L);

private:
	Int64 userId_;
	String userName_;

	Unit* controlUnit_;
};

}
