#include "Viewport.h"
#include "RenderEngine.h"
#include "GfxDevice/GfxRenderSurface.h"
#include "GfxDevice/GfxDevice.h"

namespace FlagGG
{

Viewport::~Viewport()
{
}

UInt32 Viewport::GetX() const
{
	return rect_.left_;
}

UInt32 Viewport::GetY() const
{
	return rect_.top_;
}

UInt32 Viewport::GetWidth() const
{
	return rect_.Width();
}

UInt32 Viewport::GetHeight() const
{
	return rect_.Height();
}

void Viewport::Resize(const IntRect& rect)
{
	rect_ = rect;
}

const IntRect& Viewport::GetSize() const
{
	return rect_;
}

GfxRenderSurface* Viewport::GetRenderTarget() const
{
	return renderTarget_;
}

void Viewport::SetRenderTarget(GfxRenderSurface* renderTarget)
{
	renderTarget_ = renderTarget;
}

GfxRenderSurface* Viewport::GetDepthStencil() const
{
	return depthStencil_;
}

void Viewport::SetDepthStencil(GfxRenderSurface* depthStencil)
{
	depthStencil_ = depthStencil;
}

void Viewport::SetViewport()
{
	GfxDevice::GetDevice()->SetViewport(rect_);
}

Camera* Viewport::GetCamera() const
{
	return camera_;
}

void Viewport::SetCamera(Camera* camera)
{
	camera_ = camera;
}

Scene* Viewport::GetScene() const
{
	return scene_;
}

void Viewport::SetScene(Scene* scene)
{
	scene_ = scene;
}

}

