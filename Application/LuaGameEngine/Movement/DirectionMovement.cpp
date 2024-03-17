#include "Movement/DirectionMovement.h"

#include <Scene/Node.h>

namespace LuaGameEngine
{

void DirectionMovement::Start()
{
	isActive_ = true;
}

void DirectionMovement::Stop()
{
	isActive_ = false;
}

bool DirectionMovement::IsActive() const
{
	return isActive_;
}

void DirectionMovement::Update(Real timeStep, Vector3& posDelta, Quaternion& rotDelta)
{
	if (!IsActive() || !owner_)
		return;

	const float moveSpeed = Max(owner_->GetAttribute("MOVE_SPEED").TryGet<float>(), 1.0f);
	posDelta = direction_ * moveSpeed * timeStep;
}

void DirectionMovement::SetMoveDirection(const Vector3& direction)
{
	direction_ = direction.Normalized();
}

}
