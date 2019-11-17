#ifdef _WIN32
#pragma once

#include <Scene/Camera.h>
#include <Core/DeviceEvent.h>
#include <Container/Ptr.h>
#include <Core/Context.h>
#include <Math/Vector2.h>

using namespace FlagGG::Core;
using namespace FlagGG::Scene;
using namespace FlagGG::Container;
using namespace FlagGG::Math;

class CameraOperation : public RefCounted
{
public:
	CameraOperation(FlagGG::Core::Context* context, Camera* inCamera);

	SharedPtr<Camera> GetCamera();

	void OnKeyDown(KeyState* keyState, unsigned keyCode);

	void OnKeyUp(KeyState* keyState, unsigned keyCode);

	void OnMouseDown(KeyState* keyState, MouseKey mouseKey);

	void OnMouseUp(KeyState* keyState, MouseKey mouseKey);

	void OnMouseMove(KeyState* keyState, const Vector2& delta);

	void HandleUpdate(float timeStep);

private:
	SharedPtr<Camera> camera_;

	bool mouseDown_{ false };

	float rate_{ 0.00005f };

	float walkSpeed_{ 5.0f };
};
#endif
