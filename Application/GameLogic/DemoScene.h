#pragma once

#include <Core/EventDefine.h>
#include <Graphics/Window.h>
#include <Graphics/Texture2D.h>
#include <Container/Ptr.h>
#include <Core/Contex.h>
#include <Core/DeviceEvent.h>
#include <Resource/ResourceCache.h>
#include "CameraOperation.h"

using namespace FlagGG::Core;
using namespace FlagGG::Container;
using namespace FlagGG::Graphics;
using namespace FlagGG::Resource;

namespace Frame
{
	DEFINE_EVENT(FRAME_BEGIN, void(float));
	DEFINE_EVENT(FRAME_END, void(float));
	DEFINE_EVENT(LOGIC_UPDATE, void(float));
	DEFINE_EVENT(RENDER_UPDATE, void(float));
}

class DemoScene
{
public:
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
};