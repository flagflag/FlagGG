#include "Scene/BaseMovement.h"

namespace FlagGG
{

void BaseMovement::SetOwner(Node* node)
{
	owner_ = node;
}

Node* BaseMovement::GetOwner() const
{
	return owner_;
}

}

