#include "Viewport.h"
#include "RenderEngine.h"
#include "Graphics/RenderView.h"
#include "Graphics/RenderPipline.h"
#include "GfxDevice/GfxRenderSurface.h"
#include "GfxDevice/GfxDevice.h"

namespace FlagGG
{

Viewport::Viewport()
{

}

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

void Viewport::SetRenderPipline(RenderPipline* renderPipline)
{
	renderPipline_ = renderPipline;
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

