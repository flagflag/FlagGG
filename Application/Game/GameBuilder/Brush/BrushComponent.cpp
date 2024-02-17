#include "BrushComponent.h"
#include "Brush.h"

BrushComponent::BrushComponent()
	: ownerBrush_(nullptr)
{

}

BrushComponent::~BrushComponent()
{

}

void BrushComponent::Detach()
{
	if (ownerBrush_)
	{
		ownerBrush_->DetachComponent(this);
		ownerBrush_ = nullptr;
	}
}

void BrushComponent::SetOwnerBrush(Brush* ownerBrush)
{
	ownerBrush_ = ownerBrush;
}
