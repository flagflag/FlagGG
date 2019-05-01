#pragma once

#include <Core/EventDefine.h>
#include <Graphics/WinViewport.h>
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

	SharedPtr<Context> context_;

	SharedPtr<Input> input_;

	SharedPtr<ResourceCache> cache_;

	SharedPtr<WinViewport> viewport_;

	SharedPtr<RenderContext> renderContext_;

	SharedPtr<CameraOperation> cameraOpt_;
};