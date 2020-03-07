#pragma once

#include <Container/Str.h>
#include <Container/Ptr.h>
#include <Container/Vector.h>
#include <Container/List.h>
#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <lua.hpp>

#include "LGEExport.h"
#include "Movement.h"
#include "Spell.h"
#include "Buff.h"
#include "EngineObject.h"

namespace LuaGameEngine
{
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
		FlagGG::Math::Vector3 position_;
		FlagGG::Math::Quaternion rotation_;
		FlagGG::Math::Vector3 scale_;
	};

	struct LuaGameEngine_API Attribute
	{
		float moveSpeed_;
		float attackSpeed_;
	};

	class LuaGameEngine_API Unit : public EngineObject
	{
		OBJECT_OVERRIDE(Unit)
	public:
		Unit();

		~Unit() override;

		void Update();

		Int64 GetId() const { return unitId_; }

		const FlagGG::Container::String& GetName() const { return name_; }

		const FlagGG::Math::Vector3& GetPosition() const { return position_; }

		const FlagGG::Math::Quaternion& GetRotation() const { return rotation_; }

		const FlagGG::Math::Vector3& GetScale() const { return scale_; }

		Int32 GetStatus() const { return status_; }

		const Attribute& GetAttribute() const { return attribute_; }

		Movement* GetCurrentMovement() const { return *movements_.Begin(); }

		Spell* GetCurrentSpell() const { return spell_; }

		const UInt32 GetNumBuff() const { return buffs_.Size(); }

		Buff* GetBuff(UInt32 index) const { return buffs_[index]; }

		void SetName(const FlagGG::Container::String& name);

		void SetPosition(const FlagGG::Math::Vector3& position);

		void SetRotation(const FlagGG::Math::Quaternion& rotation);

		void SetScale(const FlagGG::Math::Vector3& scale);

		void SetStatus(Int32 status);

		void AddMovement(Movement* movement);

		void RemoveMovement(Movement* movement);

		static int Create(lua_State* L);
		static int Destroy(lua_State* L);
		static int GetName(lua_State* L);
		static int GetPosition(lua_State* L);
		static int GetRotation(lua_State* L);
		static int GetScale(lua_State* L);
		static int SetName(lua_State* L);
		static int SetPosition(lua_State* L);
		static int SetRotation(lua_State* L);
		static int SetScale(lua_State* L);
		static int AddMovement(lua_State* L);
		static int RemoveMovement(lua_State* L);

	private:
		static Int64 unitIdCount_;

		Int64 unitId_;
		FlagGG::Container::String name_;

		FlagGG::Math::Vector3 position_;
		FlagGG::Math::Quaternion rotation_;
		FlagGG::Math::Vector3 scale_;

		Int32 status_;
		Attribute attribute_;

		FlagGG::Container::List<Movement*> movements_;
		Spell* spell_;
		FlagGG::Container::PODVector<Buff*> buffs_;
	};
}

