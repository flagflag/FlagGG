#include "LightBrushComponent.h"
#include "GameBuilder/Brush/Brush.h"

#include <Scene/Node.h>
#include <Scene/Scene.h>
#include <Scene/Light.h>

LightBrushComponent::LightBrushComponent()
{

}

LightBrushComponent::~LightBrushComponent()
{

}

void LightBrushComponent::OnAttach()
{
	controlNode_ = new Node();
	ownerBrush_->GetOwnerScene()->AddChild(controlNode_);

	controlNode_->SetPosition(Vector3(10, 0, 5));
	auto* light = controlNode_->CreateComponent<Light>();
	light->SetLightType(LIGHT_TYPE_POINT);
	light->SetLightUnit(LU_CANDELAS);
	light->SetBrightness(8.0f);
	light->SetRange(1000.0f);
}

void LightBrushComponent::OnDetach()
{
	controlNode_->RemoveFromParent();
	controlNode_.Reset();
}

void LightBrushComponent::OnMouseDown(KeyState* keyState, MouseKey mouseKey)
{

}

void LightBrushComponent::OnMouseUp(KeyState* keyState, MouseKey mouseKey)
{

}

void LightBrushComponent::OnMouseMove(KeyState* keyState, const Vector2& delta)
{

}

void LightBrushComponent::OnWheel()
{

}

void LightBrushComponent::OnUpdate(float timeStep)
{

}
