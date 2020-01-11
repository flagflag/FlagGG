#include "Unit.h"
#include "Common.h"

namespace LuaGameEngine
{
	Unit::Unit()
	{

	}

	Unit::~Unit()
	{

	}

	void Unit::SetName(const FlagGG::Container::String& name)
	{
		name_ = name;
	}

	void Unit::SetPosition(const FlagGG::Math::Vector3& position)
	{
		position_ = position;
	}

	void Unit::SetRotation(const FlagGG::Math::Quaternion& rotation)
	{
		rotation_ = rotation;
	}

	void Unit::SetScale(const FlagGG::Math::Vector3& scale)
	{
		scale_ = scale;
	}

	int Unit::Create(lua_State* L)
	{
		lua_newtable(L);
		lua_getmetatable(L, 1);
		lua_setmetatable(L, -2);
		SetEntry(L, -1, new Unit());
		return 1;
	}

	int Unit::Destroy(lua_State* L)
	{
		Unit* unit = GetEntry<Unit>(L, 1);
		delete unit;
		return 0;
	}

	int Unit::GetName(lua_State* L)
	{
		Unit* unit = GetEntry<Unit>(L, 1);
		lua_pushlstring(L, unit->GetName().CString(), unit->GetName().Length());
		return 1;
	}

	int Unit::GetPosition(lua_State* L)
	{
		Unit* unit = GetEntry<Unit>(L, 1);
		const auto& pos = unit->GetPosition();
		lua_pushnumber(L, pos.x_);
		lua_pushnumber(L, pos.y_);
		lua_pushnumber(L, pos.z_);
		return 3;
	}

	int Unit::GetRotation(lua_State* L)
	{
		Unit* unit = GetEntry<Unit>(L, 1);
		FlagGG::Math::Vector3 rot = unit->GetRotation() * FlagGG::Math::Vector3::FORWARD;
		lua_pushnumber(L, rot.x_);
		lua_pushnumber(L, rot.y_);
		lua_pushnumber(L, rot.z_);
		return 3;
	}

	int Unit::GetScale(lua_State* L)
	{
		Unit* unit = GetEntry<Unit>(L, 1);
		const auto& scale = unit->GetScale();
		lua_pushnumber(L, scale.x_);
		lua_pushnumber(L, scale.y_);
		lua_pushnumber(L, scale.z_);
		return 3;
	}

	int Unit::SetName(lua_State* L)
	{
		Unit* unit = GetEntry<Unit>(L, 1);
		unit->SetName(lua_tostring(L, 2));
		return 0;
	}

	int Unit::SetPosition(lua_State* L)
	{
		Unit* unit = GetEntry<Unit>(L, 1);
		unit->SetPosition(FlagGG::Math::Vector3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
		return 0;
	}

	int Unit::SetRotation(lua_State* L)
	{
		Unit* unit = GetEntry<Unit>(L, 1);
		unit->SetRotation(FlagGG::Math::Quaternion(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
		return 0;
	}

	int Unit::SetScale(lua_State* L)
	{
		Unit* unit = GetEntry<Unit>(L, 1);
		unit->SetScale(FlagGG::Math::Vector3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
		return 0;
	}
}
