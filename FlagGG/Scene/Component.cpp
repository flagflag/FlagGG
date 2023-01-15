#include "Scene/Component.h"
#include "Scene/Node.h"

namespace FlagGG
{

Component::~Component()
{

}

void Component::SetNode(Node* node)
{
	node_ = node;
}

void Component::SetViewMask(UInt32 viewMask)
{
	viewMask_ = viewMask;
}

}

