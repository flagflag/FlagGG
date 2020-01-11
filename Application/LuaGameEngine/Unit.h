#pragma once

#include <Container/Str.h>
#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <lua.hpp>

#include "LGEExport.h"

namespace LuaGameEngine
{
	class LuaGameEngine_API Unit
	{
	public:
		Unit();

		virtual ~Unit();

		const FlagGG::Container::String& GetName() const { return name_; }

		const FlagGG::Math::Vector3& GetPosition() const { return position_; }

		const FlagGG::Math::Quaternion& GetRotation() const { return rotation_; }

		const FlagGG::Math::Vector3& GetScale() const { return scale_; }


		void SetName(const FlagGG::Container::String& name);

		void SetPosition(const FlagGG::Math::Vector3& position);

		void SetRotation(const FlagGG::Math::Quaternion& rotation);

		void SetScale(const FlagGG::Math::Vector3& scale);


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

	private:
		FlagGG::Container::String name_;

		FlagGG::Math::Vector3 position_;
		FlagGG::Math::Quaternion rotation_;
		FlagGG::Math::Vector3 scale_;
	};
}

