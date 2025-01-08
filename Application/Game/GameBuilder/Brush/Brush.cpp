#include "Brush.h"
#include "BrushComponent.h"

#include <Core/EventDefine.h>
#include <Core/EventManager.h>

Brush::Brush(Scene* scene)
	: scene_(scene)
{
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_DOWN, Brush::OnMouseDown, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_UP, Brush::OnMouseUp, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_MOVE, Brush::OnMouseMove, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, Brush::OnUpdate, this));
}

Brush::~Brush()
{

}

void Brush::AttachComponent(BrushComponent* brushComponent)
{
	brushComponents_.Push(SharedPtr<BrushComponent>(brushComponent));
	brushComponent->SetOwnerBrush(this);
	brushComponent->OnAttach();
}

void Brush::DetachComponent(BrushComponent* brushComponent)
{
	brushComponents_.Remove(SharedPtr<BrushComponent>(brushComponent));
	brushComponent->OnDetach();
	brushComponent->SetOwnerBrush(nullptr);
}


void Brush::OnMouseDown(KeyState* keyState, MouseKey mouseKey)
{
	for (auto& brushComponent : brushComponents_)
	{
		brushComponent->OnMouseDown(keyState, mouseKey);
	}
}

void Brush::OnMouseUp(KeyState* keyState, MouseKey mouseKey)
{
	for (auto& brushComponent : brushComponents_)
	{
		brushComponent->OnMouseUp(keyState, mouseKey);
	}
}

void Brush::OnMouseMove(KeyState* keyState, const Vector2& delta)
{
	for (auto& brushComponent : brushComponents_)
	{
		brushComponent->OnMouseMove(keyState, delta);
	}
}

void Brush::OnWheel()
{

}

void Brush::OnUpdate(float timeStep)
{
	for (auto& brushComponent : brushComponents_)
	{
		brushComponent->OnUpdate(timeStep);
	}
}
