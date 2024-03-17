#pragma once

#include "LuaGameEngine/LGEExport.h"

#include <Scene/BaseMovement.h>
#include <Math/Vector3.h>

namespace LuaGameEngine
{

using namespace FlagGG;

class LuaGameEngine_API DirectionMovement : public BaseMovement
{
	OBJECT_OVERRIDE(DirectionMovement, BaseMovement);
public:
	void Start() override;

	void Stop() override;

	bool IsActive() const override;

	void Update(Real timeStep, Vector3& posDelta, Quaternion& rotDelta) override;

	void SetMoveDirection(const Vector3& direction);

private:
	bool isActive_;

	Vector3 direction_;
};

}
