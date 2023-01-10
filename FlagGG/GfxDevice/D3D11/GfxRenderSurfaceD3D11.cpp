#include "GfxRenderSurfaceD3D11.h"

namespace FlagGG
{

GfxRenderSurfaceD3D11::GfxRenderSurfaceD3D11()
	: GfxRenderSurface()
{

}

GfxRenderSurfaceD3D11::~GfxRenderSurfaceD3D11()
{

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
