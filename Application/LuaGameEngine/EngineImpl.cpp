#include "EngineImpl.h"
#include "Unit.h"
#include "Common.h"
#include "ControlerImpl.h"

#include <Lua/ILua/LuaUtil.h>
#include <Log.h>

namespace LuaGameEngine
{
	EngineImpl::EngineImpl(lua_State* L) :
		L_(L),
		controler_(new ControlerImpl(L))
	{
		RegisterEngineObjectType();
		CreateEngineEntry();
		CreatePlayerClass();
		CreateUnitClass();
		CreateMovementClass();
	}

	EngineImpl::~EngineImpl()
	{
		delete controler_;
		controler_ = nullptr;
	}

	EngineObject* EngineImpl::CreateObjectImpl(const char* className)
	{
		return engineObjectCreator_.Create(className);
	}

	void EngineImpl::DestroyObject(EngineObject* object)
	{
		engineObjectCreator_.Destroy(object->GetTypeName(), object);
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
		if (players_.Contains(info.userId_))
			return;

		FlagGG::Container::SharedPtr<Player> player(new Player());
		player->SetUserId(info.userId_);
		player->SetUserName(info.userName_);
		players_[info.userId_] = player;

		FlagGG::Lua::Call(L_, "game.on_player_into", info.userId_);
	}

	void EngineImpl::RemoveUser(Int64 userId)
	{
		if (!players_.Contains(userId))
			return;
		FlagGG::Container::SharedPtr<Player> player = players_[userId];
		players_.Erase(userId);

		FlagGG::Lua::Call(L_, "game.on_player_leave", userId);
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
		FlagGG::Lua::Call(L_, "game.on_start");
	}

	void EngineImpl::OnStop()
	{
		FlagGG::Lua::Call(L_, "game.on_stop");
	}

	void EngineImpl::OnFrameUpdate(float timeStep)
	{
		FlagGG::Lua::Call(L_, "game.on_update", timeStep);

		lua_gc(L_, LUA_GCSTEP, 100);

		const auto& units = unitPool_.GetObjects();
		for (auto unit = units.Begin(); unit != units.End(); ++unit)
		{
			(*unit)->Update();
		}

		for(auto it = peddingResolve_.Begin(); it != peddingResolve_.End(); ++it)
		{
			EngineObject* object = *it;
			if (object->GetType() == Unit::GetTypeStatic())
			{
				Unit* unit = static_cast<Unit*>(object);
				for (auto notify : handlers_)
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
		movementPool_.Recycling([](EngineObject* object)
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

	Controler* EngineImpl::GetControler()
	{
		return controler_;
	}

	void EngineImpl::OnAfterCreateObject(EngineObject* object)
	{

	}

	void EngineImpl::OnBeforeDestroyObject(EngineObject* object)
	{

	}

#define REGISTER_TYPE(Type, objectPool) \
	engineObjectCreator_.RegisterTypeEx<Type>(#Type, \
	[&]() \
	{ \
		EngineObject* object = objectPool.CreateObject(); \
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

#ifdef GetUserName
#undef  GetUserName
#endif

	void EngineImpl::CreatePlayerClass()
	{
		const luaL_Reg lib[] = 
		{
			{ "create", &Player::Create },
			{ "get_user_id", &Player::GetUserId },
			{ "get_user_name", &Player::GetUserName },
			{ "get_control_unit", &Player::GetControlUnit },
			{ "set_user_id", &Player::SetUserId },
			{ "set_user_name", &Player::SetUserName },
			{ "set_control_unit", &Player::SetControlUnit },
			{ nullptr, nullptr }
		};
		CreateClass(L_, "player", lib, &Player::Destroy);
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
			{ "add_movement", &Unit::AddMovement },
			{ "remove_movement", &Unit::RemoveMovement },
			{ nullptr, nullptr, }
		};
		CreateClass(L_, "unit", lib, &Unit::Destroy);
	}

	void EngineImpl::CreateMovementClass()
	{
		const luaL_Reg lib[]=
		{
			{ "create", &Movement::Create },
			{ "start", &Movement::Start },
			{ "stop", &Movement::Stop },
			{ "is_active", &Movement::IsActive },
			{ nullptr, nullptr }
		};
		CreateClass(L_, "movement", lib, &Movement::Destroy);
	}
}
