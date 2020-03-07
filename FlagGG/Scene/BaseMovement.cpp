#include "Scene/BaseMovement.h"

namespace FlagGG
{
	namespace Scene
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
}

