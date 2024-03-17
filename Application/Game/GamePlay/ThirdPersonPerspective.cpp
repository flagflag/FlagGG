#include "GamePlay/ThirdPersonPerspective.h"
#ifdef FLAGGG_PROTO
#include "Proto/Game.pb.h"
#endif

#include <Core/EventManager.h>
#include <Scene/AnimationComponent.h>
#include <Scene/Animation.h>
#include <Resource/ResourceCache.h>

ThirdPersonPerspective::ThirdPersonPerspective(bool moveCameraWhenMouseDown) :
	controlCamera_(new Camera()),
	lookupNode_(new Node()),
	isLocked_(false),
	moveCameraWhenMouseDown_(moveCameraWhenMouseDown),
	mouseDown_(false),
	syncMode_(SyncMode_Local),
	dir_{ 0 },
	stop_(Quaternion::IDENTITY),
	currentRot_(0.0f, 0.0f, 0.0f, 0.0f)
{
	network_ = GetSubsystem<Context>()->GetVariable<Network>(NETWORK_TYPE_NAME[NETWORK_TYPE_UDP]);

	lookupCamera_ = lookupNode_->CreateComponent<Camera>();

	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_DOWN, ThirdPersonPerspective::OnMouseDown, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_UP, ThirdPersonPerspective::OnMouseUp, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_MOVE, ThirdPersonPerspective::OnMouseMove, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, ThirdPersonPerspective::HandleUpdate, this));

	//        W  S  A  D
	rotation_[0][0][0][0] = 0.0f;
	rotation_[0][0][0][1] = 90.0f;
	rotation_[0][0][1][0] = -90.0f;
	rotation_[0][0][1][1] = 0.0f;
	rotation_[0][1][0][0] = 180.0f;
	rotation_[0][1][0][1] = 135.0f;
	rotation_[0][1][1][0] = -135.0f;
	rotation_[0][1][1][1] = 180.0f;
	rotation_[1][0][0][0] = 0.0f;
	rotation_[1][0][0][1] = 45.0f;
	rotation_[1][0][1][0] = -45.0f;
	rotation_[1][0][1][1] = 0.0f;
	rotation_[1][1][0][0] =  0.0f;
	rotation_[1][1][0][1] = 90.0f;
	rotation_[1][1][1][0] = -90.0f;
	rotation_[1][1][1][1] = 0.0f;
}

ThirdPersonPerspective::~ThirdPersonPerspective()
{

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

	if (node_)
	{
		animComp_ = node_->GetComponentRecursive<AnimationComponent>();
		idleAnim_ = GetSubsystem<ResourceCache>()->GetResource<Animation>("characters1/jianke_c96b/anim/idle.ani");
		moveAnim_ = GetSubsystem<ResourceCache>()->GetResource<Animation>("characters1/jianke_c96b/anim/move.ani");
	}
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
		cameraNode->SetPosition(Vector3(-6.0f, 0, 2));
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

void ThirdPersonPerspective::OnMouseDown(KeyState* keyState, MouseKey mouseKey)
{
	if (mouseKey == MOUSE_RIGHT)
		mouseDown_ = true;
	GetSubsystem<Input>()->HideMouse();
}

void ThirdPersonPerspective::OnMouseUp(KeyState* keyState, MouseKey mouseKey)
{
	if (mouseKey == MOUSE_RIGHT)
		mouseDown_ = false;
	GetSubsystem<Input>()->ShowMouse();
}

void ThirdPersonPerspective::OnMouseMove(KeyState* keyState, const Vector2& delta)
{
	static const float rate_{ 0.00005f };

	if (isLocked_)
		return;

	if (moveCameraWhenMouseDown_ && !mouseDown_)
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
		bool isMoving = false;

		if (GetKeyState('W') < 0 || GetKeyState('w') < 0)
		{
			controlCamera_->Walk(walkDelta);
			isMoving = true;
		}

		if (GetKeyState('S') < 0 || GetKeyState('s') < 0)
		{
			controlCamera_->Walk(-walkDelta);
			isMoving = true;
		}

		if (GetKeyState('A') < 0 || GetKeyState('a') < 0)
		{
			controlCamera_->Strafe(-walkDelta);
			isMoving = true;
		}

		if (GetKeyState('D') < 0 || GetKeyState('d') < 0)
		{
			controlCamera_->Strafe(walkDelta);
			isMoving = true;
		}

		if (GetKeyState('E') < 0 || GetKeyState('e') < 0)
			controlCamera_->Fly(walkDelta);

		if (GetKeyState('Q') < 0 || GetKeyState('Q') < 0)
			controlCamera_->Fly(-walkDelta);

		if (GetKeyState('N') < 0 || GetKeyState('n') < 0)
			controlCamera_->Roll(0.000020);

		if (GetKeyState('M') < 0 || GetKeyState('m') < 0)
			controlCamera_->Roll(-0.000020);

		dir_[0] = GetKeyState('W') < 0 || GetKeyState('w') < 0 ? 1 : 0;
		dir_[1] = GetKeyState('S') < 0 || GetKeyState('s') < 0 ? 1 : 0;
		dir_[2] = GetKeyState('A') < 0 || GetKeyState('a') < 0 ? 1 : 0;
		dir_[3] = GetKeyState('D') < 0 || GetKeyState('d') < 0 ? 1 : 0;

		if (animComp_)
		{
			if (isMoving_ != isMoving)
			{
				isMoving_ = isMoving;
				if (isMoving_)
				{
					animComp_->SetAnimation(moveAnim_);
					animComp_->Play(true);
				}
				else
				{
					animComp_->SetAnimation(idleAnim_);
					animComp_->Play(true);
				}
			}

			float facing = rotation_[dir_[0]][dir_[1]][dir_[2]][dir_[3]];

			if (isMoving_)
			{
				if (facing_ < 0.0f && facing == 180.0f)
					facing = -facing;
				else if (facing_ == -180.0f && facing > 0.0f)
					facing_ = -facing_;
				else if(facing_ == 180.0f && facing < 0.0f)
					facing_ = -facing_;

				if (facing_ > facing)
					facing_ = Max(facing_ - timeStep * 720.0f, facing);
				else
					facing_ = Min(facing_ + timeStep * 720.0f, facing);
				const Vector3 rotator = lookupNode_->GetRotation().EulerAngles();
				animComp_->GetNode()->SetRotation(Quaternion(0, 0, facing_));
			}
		}
	}
	else
	{
		dir_[0] = GetKeyState('W') < 0 || GetKeyState('w') < 0 ? 1 : 0;
		dir_[1] = GetKeyState('S') < 0 || GetKeyState('s') < 0 ? 1 : 0;
		dir_[2] = GetKeyState('A') < 0 || GetKeyState('a') < 0 ? 1 : 0;
		dir_[3] = GetKeyState('D') < 0 || GetKeyState('d') < 0 ? 1 : 0;

		Quaternion rot = Quaternion(0.0f, 0.0f, rotation_[dir_[0]][dir_[1]][dir_[2]][dir_[3]]);

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
