#pragma once

#include "LGEExport.h"

#include <Container/RefCounted.h>
#include <Math/Vector3.h>
#include <lua.hpp>

using namespace FlagGG;

namespace LuaGameEngine
{

class LuaGameEngine_API Controller : public RefCounted
{
public:
	virtual void StartDirectionMove(const Vector3& direction) = 0;

	virtual void StopDirectionMove() = 0;
};

}
