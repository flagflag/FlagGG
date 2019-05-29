#include "Scene/Component.h"
#include "Scene/Node.h"

namespace FlagGG
{
	namespace Scene
	{
		void Component::SetNode(Node* node)
		{
			node_ = node;
		}

		Node* Component::GetNode() const
		{
			return node_;
		}
	}
}

