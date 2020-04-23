#pragma once

#include "Scene/Component.h"
#include "Scene/BaseMovement.h"
#include "Container/List.h"

namespace FlagGG
{
	namespace Scene
	{
		class FlagGG_API MovementComponent : public Component
		{
			OBJECT_OVERRIDE(MovementComponent, Component);
		public:
			void AddMovement(BaseMovement* baseMovment);

			void RemoveMovement(BaseMovement* baseMovment);

			void RemoveMovement(Container::StringHash className);

			void RemoveAllMovement();

		protected:
			void Update(Real timeStep) override;

			Container::List<Container::SharedPtr<BaseMovement>> movements_;
		};
	}
}

