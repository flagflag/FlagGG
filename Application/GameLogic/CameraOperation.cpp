#ifdef WIN32
#include "CameraOperation.h"

#include <Windows.h>

CameraOperation::CameraOperation(FlagGG::Core::Context* context, Camera* inCamera) :
	camera_(inCamera)
{
	camera_->Walk(-5.0);
	camera_->Fly(1.0);
	camera_->Strafe(1.0f);
	camera_->Pitch(-0.005f);
	camera_->Yaw(0.0025f);

	context->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_DOWN, CameraOperation::OnKeyDown, this));
	context->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_UP, CameraOperation::OnKeyUp, this));
	context->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_DOWN, CameraOperation::OnMouseDown, this));
	context->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_UP, CameraOperation::OnMouseUp, this));
	context->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_MOVE, CameraOperation::OnMouseMove, this));
	context->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, CameraOperation::HandleUpdate, this));
}

SharedPtr<Camera> CameraOperation::GetCamera()
{
	return camera_;
}

void CameraOperation::OnKeyDown(KeyState* keyState, unsigned keyCode)
{
	// printf("OnKeyDown keyCode = %u\n", keyCode);

	//if (keyCode == 'W' || keyCode == 'w')
	//{
	//	camera_->Walk(0.1f);
	//}

	//if (keyCode == 'S' || keyCode == 's')
	//{
	//	camera_->Walk(-0.1f);
	//}

	//if (keyCode == 'A' || keyCode == 'a')
	//{
	//	camera_->Strafe(-0.1f);
	//}

	//if (keyCode == 'D' || keyCode == 'd')
	//{
	//	camera_->Strafe(0.1f);
	//}

	//if (keyCode == 'R' || keyCode == 'r')
	//{
	//	camera_->Fly(0.1f);
	//}

	//if (keyCode == 'F' || keyCode == 'f')
	//{
	//	camera_->Fly(-0.1f);
	//}



	//if (keyCode == VK_UP)
	//{
	//	camera_->Pitch(0.0025);
	//}

	//if (keyCode == VK_DOWN)
	//{
	//	camera_->Pitch(-0.0025);
	//}

	//if (keyCode == VK_LEFT)
	//{
	//	camera_->Yaw(0.0025);
	//}

	//if (keyCode == VK_RIGHT)
	//{
	//	camera_->Yaw(-0.0025);
	//}

	//if (keyCode == 'N' || keyCode == 'n')
	//{
	//	camera_->Roll(0.0025);
	//}

	//if (keyCode == 'M' || keyCode == 'm')
	//{
	//	camera_->Roll(-0.0025);
	//}
}

void CameraOperation::OnKeyUp(KeyState* keyState, unsigned keyCode)
{
	// printf("OnKeyUp keyCode = %u\n", keyCode);
}

void CameraOperation::OnMouseDown(KeyState* keyState, MouseKey mouseKey)
{
	if (mouseKey == MOUSE_LEFT)
	{
		mouseDown_ = true;
	}
}

void CameraOperation::OnMouseUp(KeyState* keyState, MouseKey mouseKey)
{
	if (mouseKey == MOUSE_LEFT)
	{
		mouseDown_ = false;
	}
}

void CameraOperation::OnMouseMove(KeyState* keyState, const Vector2& delta)
{
	if (mouseDown_)
	{
		// printf("mouse delta (%lf, %lf)\n", delta.x_, delta.y_);

		camera_->Yaw(-delta.x_ * rate_);
		camera_->Pitch(-delta.y_ * rate_);
	}
}

void CameraOperation::HandleUpdate(float timeStep)
{
	float walkDelta = timeStep * walkSpeed_;

	if (GetKeyState('W') < 0 || GetKeyState('w') < 0)
	{
		camera_->Walk(walkDelta);
	}

	if (GetKeyState('S') < 0 || GetKeyState('s') < 0)
	{
		camera_->Walk(-walkDelta);
	}

	if (GetKeyState('A') < 0 || GetKeyState('a') < 0)
	{
		camera_->Strafe(-walkDelta);
	}

	if (GetKeyState('D') < 0 || GetKeyState('d') < 0)
	{
		camera_->Strafe(walkDelta);
	}

	if (GetKeyState('E') < 0 || GetKeyState('e') < 0)
	{
		camera_->Fly(walkDelta);
	}

	if (GetKeyState('Q') < 0 || GetKeyState('Q') < 0)
	{
		camera_->Fly(-walkDelta);
	}

	if (GetKeyState('N') < 0 || GetKeyState('n') < 0)
	{
		camera_->Roll(0.000020);
	}

	if (GetKeyState('M') < 0 || GetKeyState('m') < 0)
	{
		camera_->Roll(-0.000020);
	}
}
#endif
