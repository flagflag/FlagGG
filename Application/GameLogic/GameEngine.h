#pragma once

#include <Core/EventDefine.h>
#include <Graphics/Window.h>
#include <Graphics/Texture2D.h>
#include <Graphics/Model.h>
#include <Scene/Scene.h>
#include <Container/Ptr.h>
#include <Core/Context.h>
#include <Core/DeviceEvent.h>
#include <Resource/ResourceCache.h>
#include <Utility/SystemHelper.h>
#include "CameraOperation.h"

using namespace FlagGG::Core;
using namespace FlagGG::Container;
using namespace FlagGG::Graphics;
using namespace FlagGG::Resource;
using namespace FlagGG::Utility;
using namespace FlagGG::Scene;

class GameEngine
{
public:
	void SetFrameRate(float rate);

	virtual void Run();

protected:
	virtual void Start();

	virtual void Stop();

	void CreateCoreObject();

	SharedPtr<Node> CreateUnit();

	void CreateScene();

	void SetupWindow();

	void WindowClose(void* window);

	void OnKeyUp(KeyState* keyState, unsigned keyCode);

	SharedPtr<Context> context_;

	SharedPtr<Input> input_;

	SharedPtr<ResourceCache> cache_;

	Vector<SharedPtr<Viewport>> viewports_;

	SharedPtr<Scene> scene_;

	SharedPtr<Window> window_;

	SharedPtr<CameraOperation> cameraOpt_;

	SharedPtr<Texture2D> renderTexture[2];

	SystemHelper::Timer timer_;

	bool isRunning_{ false };

	float frameRate_{ 99999.0f }; // 先设置成不限帧数把
};