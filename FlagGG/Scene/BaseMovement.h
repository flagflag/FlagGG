#pragma once

#include "Core/Object.h"
#include "Scene/Node.h"
#include "Container/Ptr.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

namespace FlagGG
{

class BaseMovement : public Object
{
	OBJECT_OVERRIDE(BaseMovement, Object);
public:
	void SetOwner(Node* node);

	Node* GetOwner() const;

	virtual void Start() = 0;

	virtual void Stop() = 0;

	virtual void Update(Real timeStep, Vector3& posDelta, Quaternion& rotDelta) = 0;

	virtual bool IsActive() const = 0;

protected:
	WeakPtr<Node> owner_;
};

}

