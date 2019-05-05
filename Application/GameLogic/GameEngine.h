#pragma once

#include <Core/EventDefine.h>
#include <Graphics/Window.h>
#include <Graphics/Texture2D.h>
#include <Graphics/Model.h>
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

class GameEngine
{
public:
	void SetFrameRate(float rate);

	virtual void Run();

protected:
	virtual void Start();

	virtual void Stop();

	void OnKeyUp(KeyState* keyState, unsigned keyCode);

	SharedPtr<Context> context_;

	SharedPtr<Input> input_;

	SharedPtr<ResourceCache> cache_;

	Vector<SharedPtr<Viewport>> viewports_;

	SharedPtr<Window> window_;

	SharedPtr<RenderContext> renderContext_;

	SharedPtr<CameraOperation> cameraOpt_;

	SharedPtr<Texture2D> renderTexture[2];

	SharedPtr<Model> model_;

	SystemHelper::Timer timer_;

	float frameRate_{ 99999.0f }; // 先设置成不限帧数把
};