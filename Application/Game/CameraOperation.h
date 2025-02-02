#ifdef _WIN32
#pragma once

#include <Scene/Camera.h>
#include <Core/DeviceEvent.h>
#include <Container/Ptr.h>
#include <Core/Context.h>
#include <Math/Vector2.h>

using namespace FlagGG;

class CameraOperation : public RefCounted
{
public:
	CameraOperation(Context* context, Camera* inCamera);

	SharedPtr<Camera> GetCamera();

	void OnKeyDown(KeyState* keyState, unsigned keyCode);

	void OnKeyUp(KeyState* keyState, unsigned keyCode);

	void OnMouseDown(KeyState* keyState, MouseKey mouseKey, const IntVector2& mousePos);

	void OnMouseUp(KeyState* keyState, MouseKey mouseKey, const IntVector2& mousePos);

	void OnMouseMove(KeyState* keyState, const IntVector2& mousePos, const Vector2& delta);

	void HandleUpdate(float timeStep);

private:
	SharedPtr<Camera> camera_;

	bool mouseDown_{ false };

	float rate_{ 0.00005f };

	float walkSpeed_{ 5.0f };
};
#endif
