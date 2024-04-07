#include "Player.h"
#include "Common.h"
#include "Engine.h"

namespace LuaGameEngine
{

Player::Player(EngineContext* engineContext)
	: EngineContextObject(engineContext)
	, controlUnit_(nullptr)
{

}

Player::~Player()
{
	if (controlUnit_)
		controlUnit_->ReleaseRef();
}

void Player::SetUserId(Int64 userId)
{
	userId_ = userId;
}

void Player::SetUserName(const String& userName)
{
	userName_ = userName;
}

void Player::SetControlUnit(Unit* unit)
{
	if (controlUnit_ != unit)
	{
		if (controlUnit_)
			controlUnit_->ReleaseRef();
		controlUnit_ = unit;
		if (controlUnit_)
			controlUnit_->AddRef();
	}
}

void Player::SetController(Controller* controller)
{
	controller_ = controller;
}


int Player::Get(lua_State* L)
{
	Engine* engine = GetEngine(L);
	auto* player = engine->GetPlayer(lua_tointeger(L, 1));
	SetEntry<Player>(L, player);
	return 1;
}

int Player::Destroy(lua_State* L)
{
	Player* player = GetEntry<Player>(L, 1);
	if (!player) return 0;
	player->ReleaseRef();
	return 0;
}

int Player::GetUserId(lua_State* L)
{
	Player* player = GetEntry<Player>(L, 1);
	lua_pushinteger(L, player->GetUserId());
	return 1;
}

int Player::GetUserName(lua_State* L)
{
	Player* player = GetEntry<Player>(L, 1);
	lua_pushstring(L, player->GetUserName().CString());
	return 1;
}

int Player::GetControlUnit(lua_State* L)
{
	Player* player = GetEntry<Player>(L, 1);
	SetEntry<Unit>(L, player->GetControlUnit());
	return 1;
}

int Player::GetController(lua_State* L)
{
	Player* player = GetEntry<Player>(L, 1);
	luaex_pushusertyperef(L, "Controller", player->GetController());
	return 1;
}

int Player::SetUserId(lua_State* L)
{
	Player* player = GetEntry<Player>(L, 1);
	player->SetUserId(lua_tointeger(L, 2));
	return 0;
}

int Player::SetUserName(lua_State* L)
{
	Player* player = GetEntry<Player>(L, 1);
	player->SetUserName(lua_tostring(L, 2));
	return 0;
}

int Player::SetControlUnit(lua_State* L)
{
	Player* player = GetEntry<Player>(L, 1);
	Unit* unit = GetEntry<Unit>(L, 2);
	player->SetControlUnit(unit);
	return 0;
}

int Player::SetController(lua_State* L)
{
	Player* player = GetEntry<Player>(L, 1);
	Controller* controller = (Controller*)luaex_tousertype(L, 2, "Controller");
	player->SetController(controller);
	return 0;
}

}
