#include "Unit.h"
#include "Common.h"
#include "Engine.h"

namespace LuaGameEngine
{

Int64 Unit::unitIdCount_ = 0;

Unit::Unit() :
	unitId_(++unitIdCount_)
{

}

Unit::~Unit()
{

}

void Unit::Update()
{
	for (auto it = movements_.Begin(); it != movements_.End();)
	{
		if (!(*it)->IsValid())
		{
			(*it)->ReleaseRef();
			it = movements_.Erase(it);
		}
		else
			++it;
	}

	for (auto it = buffs_.Begin(); it != buffs_.End(); ++it)
	{
		if (!(*it)->IsValid())
		{
			(*it)->ReleaseRef();
			it = buffs_.Erase(it);
		}
		else
			++it;
	}
}

void Unit::SetName(const String& name)
{
	name_ = name;
}

void Unit::SetPosition(const Vector3& position)
{
	position_ = position;
}

void Unit::SetRotation(const Quaternion& rotation)
{
	rotation_ = rotation;
}

void Unit::SetScale(const Vector3& scale)
{
	scale_ = scale;
}

void Unit::SetStatus(Int32 status)
{
	status_ = status;
}

void Unit::AddMovement(Movement* movement)
{
	if (!movements_.Contains(movement))
	{
		movements_.Push(movement);
		movement->AddRef();
	}
}

void Unit::RemoveMovement(Movement* movement)
{
	auto it = movements_.Find(movement);
	if (it != movements_.End())
	{
		movements_.Erase(it);
		movement->ReleaseRef();
	}
}

int Unit::Create(lua_State* L)
{		
	Engine* engine = GetEngine(L);
	Unit* unit = engine->CreateObject<Unit>();
	SetEntry<Unit>(L, unit);
	return 1;
}

int Unit::Destroy(lua_State* L)
{
	Unit* unit = GetEntry<Unit>(L, 1);
	unit->ReleaseRef();
	Engine* engine = GetEngine(L);
	engine->DestroyObject(unit);
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
	const auto& rot = unit->GetRotation();
	lua_pushnumber(L, rot.x_);
	lua_pushnumber(L, rot.y_);
	lua_pushnumber(L, rot.z_);
	lua_pushnumber(L, rot.w_);
	return 4;
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
	unit->SetPosition(Vector3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
	return 0;
}

int Unit::SetRotation(lua_State* L)
{
	Unit* unit = GetEntry<Unit>(L, 1);
	unit->SetRotation(Quaternion(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5)));
	return 0;
}

int Unit::SetScale(lua_State* L)
{
	Unit* unit = GetEntry<Unit>(L, 1);
	unit->SetScale(Vector3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
	return 0;
}

int Unit::AddMovement(lua_State* L)
{
	Unit* unit = GetEntry<Unit>(L, 1);
	Movement* movement = GetEntry<Movement>(L, 2);
	unit->AddMovement(movement);
	return 0;
}

int Unit::RemoveMovement(lua_State* L)
{
	Unit* unit = GetEntry<Unit>(L, 1);
	Movement* movement = GetEntry<Movement>(L, 2);
	unit->RemoveMovement(movement);
	return 0;
}

}
