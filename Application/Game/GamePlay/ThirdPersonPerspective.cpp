#include "GamePlay/ThirdPersonPerspective.h"

ThirdPersonPerspective::ThirdPersonPerspective(Context* context) :
	controlCamera_(new Camera()),
	lookupNode_(new Node()),
	isLocked_(false)
{
	lookupCamera_ = lookupNode_->CreateComponent<Camera>();

	context->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_MOVE, ThirdPersonPerspective::OnMouseMove, this));
	context->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, ThirdPersonPerspective::HandleUpdate, this));
}

void ThirdPersonPerspective::SetCamera(Camera* camera)
{
	camera_ = camera;
}

Camera* ThirdPersonPerspective::GetCamera() const
{
	return camera_;
}

void ThirdPersonPerspective::SetWindow(Window* window)
{
	window_ = window;
}

Window* ThirdPersonPerspective::GetWindow() const
{
	return window_;
}

void ThirdPersonPerspective::SetNode(Node* node)
{
	if (node_ != node)
	{
		if (node_)
		{
			node_->RemoveComponent(controlCamera_);
			node_->RemoveChild(lookupNode_);
		}

		if (node)
		{
			node->AddComponent(controlCamera_);
			node->AddChild(lookupNode_);
		}
	}

	node_ = node;
}

Node* ThirdPersonPerspective::GetNode() const
{
	return node_;
}

void ThirdPersonPerspective::Reset()
{
	if (camera_)
	{
		auto cameraNode = camera_->GetNode();
		cameraNode->SetPosition(Vector3(0, 2, -6.0f));
		lookupNode_->AddChild(cameraNode);
	}
}

void ThirdPersonPerspective::Lock()
{
	isLocked_ = true;
}

void ThirdPersonPerspective::Unlock()
{
	isLocked_ = false;
}

void ThirdPersonPerspective::OnMouseMove(KeyState* keyState, const Vector2& delta)
{
	static const float rate_{ 0.00005f };

	if (isLocked_)
		return;

	if (controlCamera_)
		controlCamera_->Yaw(-delta.x_ * rate_);

	if (lookupCamera_)
		lookupCamera_->Pitch(-delta.y_ * rate_);
}

void ThirdPersonPerspective::HandleUpdate(float timeStep)
{
	if (!window_ || !controlCamera_)
		return;

	if (!window_->IsForegroundWindow())
		return;

	static const float walkSpeed_{ 5.0f };
	float walkDelta = timeStep * walkSpeed_;

	if (GetKeyState('W') < 0 || GetKeyState('w') < 0)
		controlCamera_->Walk(walkDelta);

	if (GetKeyState('S') < 0 || GetKeyState('s') < 0)
		controlCamera_->Walk(-walkDelta);

	if (GetKeyState('A') < 0 || GetKeyState('a') < 0)
		controlCamera_->Strafe(-walkDelta);

	if (GetKeyState('D') < 0 || GetKeyState('d') < 0)
		controlCamera_->Strafe(walkDelta);

	if (GetKeyState('E') < 0 || GetKeyState('e') < 0)
		controlCamera_->Fly(walkDelta);

	if (GetKeyState('Q') < 0 || GetKeyState('Q') < 0)
		controlCamera_->Fly(-walkDelta);

	if (GetKeyState('N') < 0 || GetKeyState('n') < 0)
		controlCamera_->Roll(0.000020);

	if (GetKeyState('M') < 0 || GetKeyState('m') < 0)
		controlCamera_->Roll(-0.000020);
}
