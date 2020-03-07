#pragma once 

#include "LGEExport.h"
#include "EngineObject.h"

#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <lua.hpp>

namespace LuaGameEngine
{
	class LuaGameEngine_API Movement : public EngineObject
	{
		OBJECT_OVERRIDE(Movement)
	public:
		virtual void Start() = 0;

		virtual void Stop() = 0;

		virtual void Update(Real timeStep, FlagGG::Math::Vector3& posDelta, FlagGG::Math::Quaternion& rotDelta) = 0;

		virtual bool IsActive() const = 0;


		static int Create(lua_State* L);
		static int Destroy(lua_State* L);
		static int Start(lua_State* L);
		static int Stop(lua_State* L);
		static int IsActive(lua_State* L);
	};
}
