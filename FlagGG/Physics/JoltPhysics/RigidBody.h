#pragma once

#include "Physics/JoltPhysics/Rigid.h"

namespace FlagGG
{

class FlagGG_API RigidBody : public Rigid
{
	OBJECT_OVERRIDE(RigidBody, Rigid);
public:
	RigidBody();

	~RigidBody() override;

private:

};

}
