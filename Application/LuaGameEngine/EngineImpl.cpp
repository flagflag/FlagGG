#include "EngineImpl.h"
#include "Unit.h"
#include "Common.h"

#include <Lua/ILua/LuaUtil.h>

namespace LuaGameEngine
{
	EngineImpl::EngineImpl(lua_State* L) :
		L_(L)
	{
		RegisterEngineObjectType();
		CreateEngineEntry();
		CreateUnitClass();
	}

	EngineObject* EngineImpl::CreateObjectImpl(const char* className)
	{
		return engineObjectCreator_.Create(className);
	}

	void EngineImpl::DestroyObject(EngineObject* object)
	{
		engineObjectCreator_.Destroy(object->ClassName(), object);
	}

	void EngineImpl::RegisterEventHandler(LuaEventHandler* handler)
	{
		handlers_.Push(handler);
	}

	void EngineImpl::UnregisterEventHandler(LuaEventHandler* handler)
	{
		handlers_.Remove(handler);
	}

	void EngineImpl::AddUser(const LuaUserInfo& info)
	{
		userInfos_.Push(info);
	}

	void EngineImpl::RemoveUser(Int64 userId)
	{
		for (auto it = userInfos_.Begin(); it != userInfos_.End(); ++it)
		{
			if (it->userId_ == userId)
			{
				userInfos_.Erase(it);
				break;
			}
		}
	}

	void EngineImpl::OnStart()
	{
		FlagGG::Lua::Call(L_, "game.on_start");
	}

	void EngineImpl::OnStop()
	{
		FlagGG::Lua::Call(L_, "game.on_stop");
	}

	void EngineImpl::OnFrameUpdate(float timeStep)
	{
		FlagGG::Lua::Call(L_, "game.on_update", timeStep);

		for(auto it = peddingResolve_.Begin(); it != peddingResolve_.End(); ++it)
		{
			EngineObject* object = *it;
			if (object->ClassName() == "Unit")
			{
				Unit* unit = static_cast<Unit*>(object);
				for (auto notify : handlers_)
				{
					notify->OnAppearUnit(unit->GetId(), unit);
				}
			}
		}

		peddingResolve_.Clear();
	}

#define REGISTER_TYPE(Type, objectPool) \
	engineObjectCreator_.RegisterTypeEx<Type>(#Type, \
	[&]() \
	{ \
		EngineObject* object = objectPool.CreateObject(); \
		peddingResolve_.Push(object); \
		return object; \
	}, \
		[&](EngineObject* object) \
	{ \
		peddingResolve_.Remove(object); \
		objectPool.DestroyObject(static_cast<Type*>(object)); \
	});

	void EngineImpl::RegisterEngineObjectType()
	{
		REGISTER_TYPE(Unit, unitPool_);
		REGISTER_TYPE(Movement, movementPool_);
		REGISTER_TYPE(Spell, spellPool_);
		REGISTER_TYPE(Buff, buffPool_);
	}

	void EngineImpl::CreateEngineEntry()
	{
		lua_newtable(L_);
		lua_setglobal(L_, "engine");

		lua_pushlightuserdata(L_, this);
		lua_setglobal(L_, "context");
	}

	void EngineImpl::CreateUnitClass()
	{
		const luaL_Reg lib[] =
		{
			{ "create", &Unit::Create },
			{ "get_name", &Unit::GetName },
			{ "get_position", &Unit::GetPosition },
			{ "get_rotation", &Unit::GetRotation },
			{ "get_scale", &Unit::GetScale },
			{ "set_name", &Unit::SetName },
			{ "set_position", &Unit::SetPosition },
			{ "set_rotation", &Unit::SetRotation },
			{ "set_scale", &Unit::SetScale },
			{ nullptr, nullptr, }
		};
		CreateClass(L_, "unit", lib, &Unit::Destroy);
	}
}
