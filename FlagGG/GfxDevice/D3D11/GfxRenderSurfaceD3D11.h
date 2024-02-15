//
// D3D11图形层RenderSurface
//

#pragma once

#include "GfxDevice/GfxRenderSurface.h"

#include <d3d11.h>

namespace FlagGG
{

class GfxRenderSurfaceD3D11 : public GfxRenderSurface
{
	OBJECT_OVERRIDE(GfxRenderSurfaceD3D11, GfxRenderSurface);
public:
	explicit GfxRenderSurfaceD3D11(GfxTexture* ownerTexture);

	explicit GfxRenderSurfaceD3D11(GfxSwapChain* ownerSwapChain);

	~GfxRenderSurfaceD3D11() override;

	void SetRenderTargetView(ID3D11RenderTargetView* renderTargetView);

	void SetDepthStencilView(ID3D11DepthStencilView* depthStencilView);


	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView_; }

	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView_; }

private:
	ID3D11RenderTargetView* renderTargetView_{};

	ID3D11DepthStencilView* depthStencilView_{};
};

}
