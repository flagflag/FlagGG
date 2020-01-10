#pragma once

#include <Container/Str.h>
#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <lua.hpp>

using namespace FlagGG::Container;
using namespace FlagGG::Math;

class Unit
{
public:
	Unit();

	virtual ~Unit();

	const String& GetName() const { return name_; }

	const Vector3& GetPosition() const { return position_; }

	const Quaternion& GetRotation() const { return rotation_; }

	const Vector3& GetScale() const { return scale_; }


	void SetName(const String& name);

	void SetPosition(const Vector3& position);

	void SetRotation(const Quaternion& rotation);

	void SetScale(const Vector3& scale);


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
	String name_;

	Vector3 position_;
	Quaternion rotation_;
	Vector3 scale_;
};

