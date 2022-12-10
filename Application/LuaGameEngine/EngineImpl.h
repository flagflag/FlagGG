#pragma once

#include "Engine.h"
#include "Unit.h"
#include "Movement.h"
#include "Spell.h"
#include "Buff.h"
#include "Player.h"
#include "InternalEvent.h"

#include <Container/IoC.h>
#include <Container/HashMap.h>
#include <Core/ObjectPool.h>

namespace LuaGameEngine
{

class EngineImpl : public Engine, public InternalEvent
{
public:
	EngineImpl(lua_State* L);

	~EngineImpl() override;

	EngineObject* CreateObjectImpl(const char* className) override;

	void DestroyObject(EngineObject* object) override;

	void RegisterEventHandler(LuaEventHandler* handler) override;

	void UnregisterEventHandler(LuaEventHandler* handler) override;

	void AddUser(const LuaUserInfo& info) override;

	void RemoveUser(Int64 userId) override;

	EngineObject* GetPlayer(Int64 userId) override;

	void OnStart() override;

	void OnStop() override;

	void OnFrameUpdate(float timeStep) override;

	Controler* GetControler() override;

	void OnAfterCreateObject(EngineObject* object) override;

	void OnBeforeDestroyObject(EngineObject* object) override;

protected:
	void RegisterEngineObjectType();

	void CreateEngineEntry();

	void CreatePlayerClass();

	void CreateUnitClass();

	void CreateMovementClass();

private:
	lua_State* L_;

	PODVector<LuaEventHandler*> handlers_;

	HashMap<Int64, SharedPtr<Player>> players_;

	IoC<EngineObject> engineObjectCreator_;

	ObjectPool<Unit> unitPool_;
	ObjectPool<Movement> movementPool_;
	ObjectPool<Spell> spellPool_;
	ObjectPool<Buff> buffPool_;

	PODVector<EngineObject*> peddingResolve_;

	Controler* controler_;
};

}
