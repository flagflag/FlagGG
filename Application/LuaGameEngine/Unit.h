#pragma once

#include <Container/Str.h>
#include <Container/Ptr.h>
#include <Container/Vector.h>
#include <Container/List.h>
#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <Scene/Node.h>
#include <Scene/BaseMovement.h>
#include <lua.hpp>

#include "LGEExport.h"
#include "Spell.h"
#include "Buff.h"
#include "EngineContextObject.h"

namespace LuaGameEngine
{

struct EngineContext;

enum Status
{
	Idle		= 1 << 0,
	Relax		= 1 << 1,
	Walk		= 1 << 2,
	Running		= 1 << 3,
	Jump		= 1 << 4,
	Attack		= 1 << 5,
	CastSpell	= 1 << 6,
};

struct LuaGameEngine_API Transform
{
	Vector3 position_;
	Quaternion rotation_;
	Vector3 scale_;
};

struct LuaGameEngine_API Attribute
{
	float moveSpeed_;
	float attackSpeed_;
};

class LuaGameEngine_API Unit : public EngineContextObject
{
	OBJECT_OVERRIDE(Unit, EngineContextObject);
public:
	explicit Unit(EngineContext* engineContext);

	~Unit() override;

	void Update(Real timeStep);

	Int64 GetId() const { return unitId_; }

	const String& GetName() const { return name_; }

	const Vector3& GetPosition() const { return node_->GetPosition(); }

	const Quaternion& GetRotation() const { return node_->GetRotation(); }

	const Vector3& GetScale() const { return node_->GetPosition(); }

	Int32 GetStatus() const { return status_; }

	Int32 GetAssetId() const { return assetId_; }

	const Attribute& GetAttribute() const { return attribute_; }

	const List<SharedPtr<BaseMovement>>* GetAllMovements() const;

	Spell* GetCurrentSpell() const { return spell_; }

	const UInt32 GetNumBuff() const { return buffs_.Size(); }

	Buff* GetBuff(UInt32 index) const { return buffs_[index]; }

	void SetName(const String& name);

	void SetPosition(const Vector3& position);

	void SetRotation(const Quaternion& rotation);

	void SetScale(const Vector3& scale);

	void SetStatus(Int32 status);

	void SetAssetId(Int32 assetId);

	void AddMovement(BaseMovement* movement);

	void RemoveMovement(BaseMovement* movement);

	static int Create(lua_State* L);
	static int Destroy(lua_State* L);
	static int GetName(lua_State* L);
	static int GetPosition(lua_State* L);
	static int GetRotation(lua_State* L);
	static int GetScale(lua_State* L);
	static int GetAssetId(lua_State* L);
	static int SetName(lua_State* L);
	static int SetPosition(lua_State* L);
	static int SetRotation(lua_State* L);
	static int SetScale(lua_State* L);
	static int SetAssetId(lua_State* L);
	static int AddMovement(lua_State* L);
	static int RemoveMovement(lua_State* L);

private:
	static Int64 unitIdCount_;

	Int64 unitId_;
	String name_;

	SharedPtr<Node> node_;

	Int32 assetId_;

	Int32 status_;
	Attribute attribute_;

	Spell* spell_;
	PODVector<Buff*> buffs_;
};

}

