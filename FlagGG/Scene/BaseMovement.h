#pragma once

#include "Core/Object.h"
#include "Scene/Node.h"
#include "Container/Ptr.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

namespace FlagGG
{
	namespace Scene
	{
		class BaseMovement : public Core::Object
		{
			OBJECT_OVERRIDE(BaseMovement);
		public:
			void SetOwner(Node* node);

			Node* GetOwner() const;

			virtual void Start() = 0;

			virtual void Stop() = 0;

			virtual void Update(Real timeStep, Math::Vector3& posDelta, Math::Quaternion& rotDelta) = 0;

			virtual bool IsActive() const = 0;

		protected:
			Container::WeakPtr<Node> owner_;
		};
	}
}

