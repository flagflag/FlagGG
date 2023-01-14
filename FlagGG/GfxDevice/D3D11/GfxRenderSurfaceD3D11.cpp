#include "GfxRenderSurfaceD3D11.h"
#include "GfxD3D11Defines.h"

namespace FlagGG
{

GfxRenderSurfaceD3D11::GfxRenderSurfaceD3D11()
	: GfxRenderSurface()
{

}

GfxRenderSurfaceD3D11::~GfxRenderSurfaceD3D11()
{
	D3D11_SAFE_RELEASE(renderTargetView_);
	D3D11_SAFE_RELEASE(depthStencilView_);
}

void GfxRenderSurfaceD3D11::SetRenderTargetView(ID3D11RenderTargetView* renderTargetView)
{
	renderTargetView_ = renderTargetView;
}

void GfxRenderSurfaceD3D11::SetDepthStencilView(ID3D11DepthStencilView* depthStencilView)
{
	depthStencilView_ = depthStencilView;
}

}
