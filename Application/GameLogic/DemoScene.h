#pragma once

#include <Core/EventDefine.h>
#include <Graphics/WinViewport.h>
#include <Container/Ptr.h>
#include "CameraOperation.h"

#include <Core/Contex.h>

using namespace FlagGG::Core;
using namespace FlagGG::Container;
using namespace FlagGG::Graphics;

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

	SharedPtr<WinViewport> viewport_;

	SharedPtr<RenderContext> renderContext_;

	SharedPtr<CameraOperation> cameraOpt_;
};