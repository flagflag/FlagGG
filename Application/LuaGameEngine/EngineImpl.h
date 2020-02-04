#pragma once

#include "Engine.h"
#include "Unit.h"
#include "Movement.h"
#include "Spell.h"
#include "Buff.h"

#include <Container/IoC.h>
#include <Container/Vector.h>
#include <Core/ObjectPool.h>

namespace LuaGameEngine
{
	class EngineImpl : public Engine
	{
	public:
		EngineImpl(lua_State* L);

		EngineObject* CreateObjectImpl(const char* className) override;

		void DestroyObject(EngineObject* object) override;

		void RegisterEventHandler(LuaEventHandler* handler) override;

		void UnregisterEventHandler(LuaEventHandler* handler) override;

		void AddUser(const LuaUserInfo& info) override;

		void RemoveUser(Int64 userId) override;

		void OnStart() override;

		void OnStop() override;

		void OnFrameUpdate(float timeStep) override;

	protected:
		void RegisterEngineObjectType();

		void CreateEngineEntry();

		void CreateUnitClass();

	private:
		lua_State* L_;

		FlagGG::Container::PODVector<LuaEventHandler*> handlers_;

		FlagGG::Container::Vector<LuaUserInfo> userInfos_;

		FlagGG::Container::IoC<EngineObject> engineObjectCreator_;

		FlagGG::Core::ObjectPool<Unit> unitPool_;
		FlagGG::Core::ObjectPool<Movement> movementPool_;
		FlagGG::Core::ObjectPool<Spell> spellPool_;
		FlagGG::Core::ObjectPool<Buff> buffPool_;

		FlagGG::Container::PODVector<EngineObject*> peddingResolve_;
	};
}
