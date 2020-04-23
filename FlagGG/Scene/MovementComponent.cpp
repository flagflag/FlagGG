#include "Scene/MovementComponent.h"

namespace FlagGG
{
	namespace Scene
	{
		void MovementComponent::AddMovement(BaseMovement* baseMovment)
		{
			Container::SharedPtr<BaseMovement> movement(baseMovment);
			if (!movements_.Contains(movement))
				movements_.Push(movement);
		}

		void MovementComponent::RemoveMovement(BaseMovement* baseMovment)
		{
			Container::SharedPtr<BaseMovement> movement(baseMovment);
			auto it = movements_.Find(movement);
			if (it != movements_.End())
				movements_.Erase(it);
		}

		void MovementComponent::RemoveMovement(Container::StringHash className)
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

			Math::Vector3 finalPosDelta = Math::Vector3::ZERO;
			Math::Quaternion finalRotDelta = Math::Quaternion::IDENTITY;

			for (auto it = movements_.Begin(); it != movements_.End(); ++it)
			{
				Math::Vector3 posDelta = Math::Vector3::ZERO;
				Math::Quaternion rotDelta = Math::Quaternion::IDENTITY;

				(*it)->Update(timeStep, posDelta, rotDelta);

				finalPosDelta += posDelta;
				finalRotDelta = finalRotDelta * rotDelta;
			}

			node_->SetWorldPosition(node_->GetWorldPosition() + finalPosDelta);
			node_->SetWorldRotation(node_->GetWorldRotation() * finalRotDelta);
		}
	}
}
