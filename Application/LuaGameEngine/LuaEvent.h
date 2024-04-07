#pragma once

#include <Core/BaseTypes.h>
#include <Math/Vector3.h>

#include "LGEExport.h"
#include "Unit.h"
#include "Movement.h"
#include "Spell.h"
#include "Buff.h"

namespace LuaGameEngine
{

enum EventType
{
	LuaEvent_KeepAlive = 0,
	LuaEvent_StartGame,
	LuaEvent_StopGame,
	LuaEvent_AppearUnit,
	LuaEvent_DisapperUnit,
	LuaEvent_SyncUnitTansform,
	LuaEvent_SyncUnitStatus,
	LuaEvent_SyncUnitAttribute,
	LuaEvent_SyncUnitMovement,
	LuaEvent_UnitCastSpell,
	LuaEvent_UnitAttachBuff,
	LuaEvent_UnitDettachBuff,
};

class LuaGameEngine_API LuaEventHandler
{
public:
	virtual ~LuaEventHandler() = default;

	virtual void OnKeepAlive() = 0;

	virtual void OnStartGame(const char* gameName) = 0;

	virtual void OnStopGame() = 0;

	virtual void OnAppearUnit(Int64 unitId, Unit* unit) = 0;

	virtual void OnDisappearUnit(Int64 unitId) = 0;

	virtual void OnSyncUnitTansform(Int64 unitId, Transform* transform) = 0;

	virtual void OnSyncUnitStatus(Int64 unitId, Status status) = 0;

	virtual void OnSyncUnitAttribute(Int64 unitId, Attribute* attribute) = 0;

	virtual void OnSyncUnitMovement(Int64 unitId, Movement* movement) = 0;

	virtual void OnUnitCastSpell(Int64 unitId, Spell* spell) = 0;

	virtual void OnUnitAttachBuff(Int64 unitId, Buff* buff) = 0;

	virtual void OnUnitDettachBuff(Int64 unitId, Buff* buff) = 0;

	virtual void OnDebugUnitTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale) = 0;
};

}
