#pragma once

#include "GfxDevice/GfxSwapChain.h"
#include "GfxDevice/D3D11/GfxRenderSurfaceD3D11.h"
#include "Container/Ptr.h"

#include <d3d11.h>

namespace FlagGG
{

class GfxSwapChainD3D11 : public GfxSwapChain
{
	OBJECT_OVERRIDE(GfxSwapChainD3D11, GfxSwapChain);
public:
	explicit GfxSwapChainD3D11(Window* window);

	~GfxSwapChainD3D11() override;

	// Resize
	void Resize(UInt32 width, UInt32 height) override;

	// 获取RT
	GfxRenderSurface* GetRenderTarget() override { return renderTarget_.Get(); }

	// 获取depth stencil
	GfxRenderSurface* GetDepthStencil() override { return depthStencil_.Get(); }

	// Swap buffer
	void Present() override;

private:
	UInt32 multiSample_{ 1 };

	bool sRGB_{ true };

	IDXGISwapChain* swapChain_{};
	ID3D11Texture2D* depthTexture_{};

	SharedPtr<GfxRenderSurfaceD3D11> renderTarget_;
	SharedPtr<GfxRenderSurfaceD3D11> depthStencil_;
};

}
