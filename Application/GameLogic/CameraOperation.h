#pragma once

#include <Graphics/Camera.h>
#include <Core/DeviceEvent.h>
#include <Container/Ptr.h>
#include <Core/Contex.h>
#include <Math/Vector2.h>

using namespace FlagGG::Core;
using namespace FlagGG::Graphics;
using namespace FlagGG::Container;
using namespace FlagGG::Math;

class CameraOperation : public RefCounted
{
public:
	CameraOperation(FlagGG::Core::Context* context);

	void OnKeyDown(KeyState* keyState, unsigned keyCode);

	void OnKeyUp(KeyState* keyState, unsigned keyCode);

	void OnMouseDown(KeyState* keyState, MouseKey mouseKey);

	void OnMouseUp(KeyState* keyState, MouseKey mouseKey);

	void OnMouseMove(KeyState* keyState, const Vector2& delta);

	void HandleUpdate(float timeStep);

	SharedPtr<Camera> camera_;

private:
	bool mouseDown_{ false };

	float rate_{ 0.00005 };

	float walkSpeed_{ 5.0f };
};