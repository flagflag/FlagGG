#include "Scene/MovementComponent.h"

namespace FlagGG
{

void MovementComponent::AddMovement(BaseMovement* baseMovment)
{
	SharedPtr<BaseMovement> movement(baseMovment);
	if (!movements_.Contains(movement))
	{
		movements_.Push(movement);
		movement->SetOwner(node_);
	}
}

void MovementComponent::RemoveMovement(BaseMovement* baseMovment)
{
	SharedPtr<BaseMovement> movement(baseMovment);
	auto it = movements_.Find(movement);
	if (it != movements_.End())
	{
		movement->SetOwner(nullptr);
		movements_.Erase(it);
	}
}

void MovementComponent::RemoveMovement(StringHash className)
{
	for (auto it = movements_.Begin(); it != movements_.End();)
	{
		if ((*it)->IsInstanceOf(className))
			it = movements_.Erase(it);
		else
			++it;
	}
}

void MovementComponent::RemoveAllMovement()
{
	movements_.Clear();
}

void MovementComponent::Update(Real timeStep)
{
	if (movements_.Empty())
		return;

	Vector3 finalPosDelta = Vector3::ZERO;
	Quaternion finalRotDelta = Quaternion::IDENTITY;

	for (auto it = movements_.Begin(); it != movements_.End(); ++it)
	{
		Vector3 posDelta = Vector3::ZERO;
		Quaternion rotDelta = Quaternion::IDENTITY;

		(*it)->Update(timeStep, posDelta, rotDelta);

		finalPosDelta += posDelta;
		finalRotDelta = finalRotDelta * rotDelta;
	}

	node_->SetWorldPosition(node_->GetWorldPosition() + finalPosDelta);
	node_->SetWorldRotation(node_->GetWorldRotation() * finalRotDelta);
}

}
