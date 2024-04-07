#pragma once

#include "LGEExport.h"
#include "EngineObject.h"
#include "Unit.h"
#include "Controller.h"

#include <Core/BaseTypes.h>
#include <Container/Str.h>
#include <lua.hpp>

#ifdef GetUserName
#undef  GetUserName
#endif

namespace LuaGameEngine
{

class LuaGameEngine_API Player : public EngineContextObject
{
	OBJECT_OVERRIDE(Player, EngineContextObject);
public:
	explicit Player(EngineContext* engineContext);

	~Player() override;

	Int64 GetUserId() const { return userId_; }

	const String& GetUserName() { return userName_; }

	Unit* GetControlUnit() const { return controlUnit_; }

	Controller* GetController() const { return controller_; }

	void SetUserId(Int64 userId);

	void SetUserName(const String& userName);

	void SetControlUnit(Unit* unit);

	void SetController(Controller* controller);


	static int Get(lua_State* L);
	static int Destroy(lua_State* L);
	static int GetUserId(lua_State* L);
	static int GetUserName(lua_State* L);
	static int GetControlUnit(lua_State* L);
	static int GetController(lua_State* L);
	static int SetUserId(lua_State* L);
	static int SetUserName(lua_State* L);
	static int SetControlUnit(lua_State* L);
	static int SetController(lua_State* L);

private:
	Int64 userId_;
	String userName_;

	Unit* controlUnit_;

	SharedPtr<Controller> controller_;
};

}
