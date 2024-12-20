#include "EngineImpl.h"
#include "Unit.h"
#include "Common.h"
#include "ControllerImpl.h"

#include <Lua/ILua/LuaUtil.h>
#include <Lua/LuaBinding/LuaBinding.h>
#include <Log.h>

namespace LuaGameEngine
{

EngineImpl::EngineImpl(lua_State* L) :
	L_(L)
{
	RegisterEngineObjectType();
	CreateEngineEntry();
	CreatePlayerClass();
	CreateUnitClass();
	CreateMovementClass();
	CreateControllerClass();
}

EngineImpl::~EngineImpl()
{

}

EngineObject* EngineImpl::CreateObjectImpl(const char* className)
{
	return engineObjectCreator_.Create(className, &engineContext_);
}

void EngineImpl::DestroyObject(EngineObject* object)
{
	engineObjectCreator_.Destroy(object->GetTypeName(), object);
}

void EngineImpl::RegisterEventHandler(LuaEventHandler* handler)
{
	engineContext_.handlers_.Push(handler);
}

void EngineImpl::UnregisterEventHandler(LuaEventHandler* handler)
{
	engineContext_.handlers_.Remove(handler);
}

void EngineImpl::AddUser(const LuaUserInfo& info)
{
	if (players_.Contains(info.userId_))
		return;

	SharedPtr<Player> player(new Player(&engineContext_));
	player->SetUserId(info.userId_);
	player->SetUserName(info.userName_);
	players_[info.userId_] = player;

	Call(L_, "game.on_player_into", info.userId_);
}

void EngineImpl::RemoveUser(Int64 userId)
{
	if (!players_.Contains(userId))
		return;
	SharedPtr<Player> player = players_[userId];
	players_.Erase(userId);

	Call(L_, "game.on_player_leave", userId);
}

EngineObject* EngineImpl::GetPlayer(Int64 userId)
{
	auto it = players_.Find(userId);
	if (it == players_.End())
		return nullptr;
	return it->second_;
}

void EngineImpl::OnStart()
{
	Call(L_, "game.on_start");
}

void EngineImpl::OnStop()
{
	Call(L_, "game.on_stop");
}

void EngineImpl::OnFrameUpdate(float timeStep)
{
	Call(L_, "game.on_update", timeStep);

	lua_gc(L_, LUA_GCSTEP, 100);

	const auto& units = unitPool_.GetObjects();
	for (auto unit = units.Begin(); unit != units.End(); ++unit)
	{
		(*unit)->Update(timeStep);
	}

	for(auto it = peddingResolve_.Begin(); it != peddingResolve_.End(); ++it)
	{
		EngineObject* object = *it;
		if (object->GetType() == Unit::GetTypeStatic())
		{
			Unit* unit = static_cast<Unit*>(object);
			for (auto notify : engineContext_.handlers_)
			{
				notify->OnAppearUnit(unit->GetId(), unit);
			}
		}
	}

	peddingResolve_.Clear();

	unitPool_.Recycling([](EngineObject* object)
	{
		return object->GetRef() == 0;
	});
	spellPool_.Recycling([](EngineObject* object)
	{
		return object->GetRef() == 0;
	});
	buffPool_.Recycling([](EngineObject* object)
	{
		return object->GetRef() == 0;
	});
}

Controller* EngineImpl::GetControler(Int64 userId)
{
	auto it = players_.Find(userId);
	if (it != players_.End())
	{
		return it->second_->GetController();
	}
	return nullptr;
}

void EngineImpl::OnAfterCreateObject(EngineObject* object)
{

}

void EngineImpl::OnBeforeDestroyObject(EngineObject* object)
{

}

#define REGISTER_TYPE(Type, objectPool) \
	engineObjectCreator_.RegisterTypeEx<Type>(#Type, \
	[&](EngineContext* engineContext) \
	{ \
		EngineObject* object = objectPool.CreateObject(engineContext); \
		object->SetValid(true); \
		peddingResolve_.Push(object); \
		return object; \
	}, \
	[&](EngineObject* object) \
	{ \
		peddingResolve_.Remove(object); \
		object->SetValid(false); \
		objectPool.DelayDestroyObject(static_cast<Type*>(object)); \
	});

void EngineImpl::RegisterEngineObjectType()
{
	REGISTER_TYPE(Unit, unitPool_);
	REGISTER_TYPE(Spell, spellPool_);
	REGISTER_TYPE(Buff, buffPool_);
}

void EngineImpl::CreateEngineEntry()
{
	lua_pushliteral(L_, "LuaGameEngine");
	lua_pushlightuserdata(L_, this);
	lua_rawset(L_, LUA_REGISTRYINDEX);
}

void EngineImpl::CreatePlayerClass()
{
	luaex_beginclass(L_, "Player", "", nullptr, &Player::Destroy);
		luaex_classfunction(L_, "get", &Player::Get);
		luaex_classfunction(L_, "get_user_id", &Player::GetUserId);
		luaex_classfunction(L_, "get_user_name", &Player::GetUserName);
		luaex_classfunction(L_, "get_control_unit", &Player::GetControlUnit);
		luaex_classfunction(L_, "get_controller", &Player::GetController);
		luaex_classfunction(L_, "set_user_id", &Player::SetUserId);
		luaex_classfunction(L_, "set_user_name", &Player::SetUserName);
		luaex_classfunction(L_, "set_control_unit", &Player::SetControlUnit);
		luaex_classfunction(L_, "set_controller", &Player::SetController);
	luaex_endclass(L_);
}

void EngineImpl::CreateUnitClass()
{
	luaex_beginclass(L_, "Unit", "", &Unit::Create, &Unit::Destroy);
		luaex_classfunction(L_, "create", &Unit::Create);
		luaex_classfunction(L_, "get_name", &Unit::GetName);
		luaex_classfunction(L_, "get_position", &Unit::GetPosition);
		luaex_classfunction(L_, "get_rotation", &Unit::GetRotation);
		luaex_classfunction(L_, "get_scale", &Unit::GetScale);
		luaex_classfunction(L_, "get_asset_id", &Unit::GetAssetId);
		luaex_classfunction(L_, "set_name", &Unit::SetName);
		luaex_classfunction(L_, "set_position", &Unit::SetPosition);
		luaex_classfunction(L_, "set_rotation", &Unit::SetRotation);
		luaex_classfunction(L_, "set_scale", &Unit::SetScale);
		luaex_classfunction(L_, "set_asset_id", &Unit::SetAssetId);
		luaex_classfunction(L_, "add_movement", &Unit::AddMovement);
		luaex_classfunction(L_, "remove_movement", &Unit::RemoveMovement);
	luaex_endclass(L_);
}

void EngineImpl::CreateMovementClass()
{
	luaex_beginclass(L_, "BaseMovement", "", nullptr, &Movement::Destroy);
		luaex_classfunction(L_, "start", &Movement::Start);
		luaex_classfunction(L_, "stop", &Movement::Stop);
		luaex_classfunction(L_, "is_active", &Movement::IsActive);
	luaex_endclass(L_);

	luaex_beginclass(L_, "DirectionMovement", "BaseMovement", &Movement::CreateDirectionMovement, &Movement::Destroy);
		luaex_classfunction(L_, "set_move_direction", &Movement::SetMoveDirection);
	luaex_endclass(L_);
}

void EngineImpl::CreateControllerClass()
{
	luaex_beginclass(L_, "Controller", "", &ControllerImpl::Create, &ControllerImpl::Destroy);
	luaex_endclass(L_);
}

}
