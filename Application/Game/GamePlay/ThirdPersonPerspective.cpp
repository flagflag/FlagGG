#include "GamePlay/ThirdPersonPerspective.h"
#ifdef FLAGGG_PROTO
#include "Proto/Game.pb.h"
#endif

#include <Core/EventManager.h>

ThirdPersonPerspective::ThirdPersonPerspective() :
	controlCamera_(new Camera()),
	lookupNode_(new Node()),
	isLocked_(false),
	syncMode_(SyncMode_Local),
	dir_{ 0 },
	stop_(0.0f, 0.0f, 0.0f, 0.0f),
	currentRot_(0.0f, 0.0f, 0.0f, 0.0f)
{
	network_ = GetSubsystem<Context>()->GetVariable<Network>(NETWORK_TYPE_NAME[NETWORK_TYPE_UDP]);

	lookupCamera_ = lookupNode_->CreateComponent<Camera>();

	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_MOVE, ThirdPersonPerspective::OnMouseMove, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, ThirdPersonPerspective::HandleUpdate, this));

	//        W  S  A  D
	rotation_[0][0][0][0] = Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
	rotation_[0][0][0][1] = Quaternion(90.0f);
	rotation_[0][0][1][0] = Quaternion(-90.0f);
	rotation_[0][0][1][1] = Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
	rotation_[0][1][0][0] = Quaternion(180.0f);
	rotation_[0][1][0][1] = Quaternion(135.0f);
	rotation_[0][1][1][0] = Quaternion(-135.0f);
	rotation_[0][1][1][1] = Quaternion(180.0f);
	rotation_[1][0][0][0] = Quaternion(0.0f);
	rotation_[1][0][0][1] = Quaternion(45.0f);
	rotation_[1][0][1][0] = Quaternion(-45.0f);
	rotation_[1][0][1][1] = Quaternion(0.0f);
	rotation_[1][1][0][0] = Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
	rotation_[1][1][0][1] = Quaternion(90.0f);
	rotation_[1][1][1][0] = Quaternion(-90.0f);
	rotation_[1][1][1][1] = Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
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

void ThirdPersonPerspective::SetSyncMode(SyncMode syncMode)
{
	syncMode_ = syncMode;
}

SyncMode ThirdPersonPerspective::GetSyncMode() const
{
	return syncMode_;
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

	if (syncMode_ == SyncMode_Local)
	{
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
	else
	{
		dir_[0] = GetKeyState('W') < 0 || GetKeyState('w') < 0 ? 1 : 0;
		dir_[1] = GetKeyState('S') < 0 || GetKeyState('s') < 0 ? 1 : 0;
		dir_[2] = GetKeyState('A') < 0 || GetKeyState('a') < 0 ? 1 : 0;
		dir_[3] = GetKeyState('D') < 0 || GetKeyState('d') < 0 ? 1 : 0;

		Quaternion rot = rotation_[dir_[0]][dir_[1]][dir_[2]][dir_[3]];

		if (rot != currentRot_)
		{
			currentRot_ = rot;

#ifdef FLAGGG_PROTO
			if (rot != stop_)
			{
				rot = node_->GetWorldRotation() * rot;

				Proto::Game::RequestStartMove request;
				request.set_user_id(0u);
				Proto::Game::Quaternion* protoRot = new Proto::Game::Quaternion();
				protoRot->set_w(rot.w_);
				protoRot->set_x(rot.x_);
				protoRot->set_y(rot.y_);
				protoRot->set_z(rot.z_);
				request.set_allocated_rotation(protoRot);

				Proto::Game::MessageHeader header;
				header.set_message_type(Proto::Game::MessageType_RequestStartMove);
				header.set_message_body(request.SerializeAsString());

				const std::string& buffer = header.SerializeAsString();
				network_->Send(buffer.data(), buffer.length());
			}
			else
			{
				Proto::Game::RequestStopMove request;
				request.set_user_id(0u);
				
				Proto::Game::MessageHeader header;
				header.set_message_type(Proto::Game::MessageType_RequestStopMove);
				header.set_message_body(request.SerializeAsString());

				const std::string& buffer = header.SerializeAsString();
				network_->Send(buffer.data(), buffer.length());
			}
#endif
		}
	}
}
