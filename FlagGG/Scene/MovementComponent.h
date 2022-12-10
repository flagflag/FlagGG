#pragma once

#include "Scene/Component.h"
#include "Scene/BaseMovement.h"
#include "Container/List.h"

namespace FlagGG
{

class FlagGG_API MovementComponent : public Component
{
	OBJECT_OVERRIDE(MovementComponent, Component);
public:
	void AddMovement(BaseMovement* baseMovment);

	void RemoveMovement(BaseMovement* baseMovment);

	void RemoveMovement(StringHash className);

	void RemoveAllMovement();

protected:
	void Update(Real timeStep) override;

	List<SharedPtr<BaseMovement>> movements_;
};

}

