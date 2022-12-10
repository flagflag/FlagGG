#pragma once

#include "LGEExport.h"

#include <Math/Vector3.h>

using namespace FlagGG;

namespace LuaGameEngine
{

class LuaGameEngine_API Controler
{
public:
	virtual void StartDirectionMove(Int64 userId, const Vector3& direction) = 0;

	virtual void StopDirectionMove(Int64 userId) = 0;
};

}
