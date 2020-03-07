#pragma once

#include <Scene/BaseMovement.h>
#include <Math/Vector3.h>

using namespace FlagGG::Scene;
using namespace FlagGG::Math;

class DirectionMovement : public BaseMovement
{
	OBJECT_OVERRIDE(DirectionMovement);
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
