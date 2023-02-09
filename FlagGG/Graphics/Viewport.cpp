#include "Viewport.h"
#include "RenderEngine.h"
#include "GfxDevice/GfxRenderSurface.h"
#include "GfxDevice/GfxDevice.h"

namespace FlagGG
{

Viewport::~Viewport()
{
}

void Viewport::Resize(const IntRect& rect)
{
	rect_ = rect;
}

void Viewport::SetRenderTarget(GfxRenderSurface* renderTarget)
{
	renderTarget_ = renderTarget;
}

void Viewport::SetDepthStencil(GfxRenderSurface* depthStencil)
{
	depthStencil_ = depthStencil;
}

void Viewport::SetCamera(Camera* camera)
{
	camera_ = camera;
}

void Viewport::SetScene(Scene* scene)
{
	scene_ = scene;
}

RenderView* Viewport::GetOrCreateRenderView()
{
	if (!renderView_)
	{
		renderView_ = new RenderView();
	}

	return renderView_;
}

void Viewport::ResetRenderView()
{
	renderView_.Reset();
}

}

