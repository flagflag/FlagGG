#include "GfxRenderSurfaceD3D11.h"
#include "GfxD3D11Defines.h"

namespace FlagGG
{

GfxRenderSurfaceD3D11::GfxRenderSurfaceD3D11(GfxTexture* ownerTexture, UInt32 surfaceWidth, UInt32 surfaceHeight)
	: GfxRenderSurface(ownerTexture, surfaceWidth, surfaceHeight)
{

}

GfxRenderSurfaceD3D11::GfxRenderSurfaceD3D11(GfxSwapChain* ownerSwapChain, UInt32 surfaceWidth, UInt32 surfaceHeight)
	: GfxRenderSurface(ownerSwapChain, surfaceWidth, surfaceHeight)
{

}

void GfxRenderSurfaceD3D11::SetGpuTag(const String& gpuTag)
{
	if (renderTargetView_)
	{
		renderTargetView_->SetPrivateData(WKPDID_D3DDebugObjectName, gpuTag.Length(), gpuTag.CString());
	}

	if (depthStencilView_)
	{
		depthStencilView_->SetPrivateData(WKPDID_D3DDebugObjectName, gpuTag.Length(), gpuTag.CString());
	}
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
