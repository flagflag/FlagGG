//
// Metal图形层SwapChain
//

#pragma once

#include "GfxDevice/GfxSwapChain.h"
#include "GfxDevice/Metal/GfxRenderSurfaceMetal.h"
#include "Container/Ptr.h"
#include "mtlpp/mtlpp.hpp"

namespace FlagGG
{

class GfxSwapChainMetal : public GfxSwapChain
{
	OBJECT_OVERRIDE(GfxSwapChainMetal, GfxSwapChain);
public:
	explicit GfxSwapChainMetal(Window* window);

	~GfxSwapChainMetal() override;

	// Resize
	void Resize(UInt32 width, UInt32 height) override;

	// 拷贝数据到backbuffer
	void CopyData(GfxTexture* gfxTexture) override;

	// 获取RT
	GfxRenderSurface* GetRenderTarget() override { return renderTarget_.Get(); }

	// 获取depth stencil
	GfxRenderSurface* GetDepthStencil() override { return depthStencil_.Get(); }

	// Swap buffer
	void Present() override;

private:
	UInt32 multiSample_{ 1 };

	bool sRGB_{};

	mtlpp::Texture mtlRenderTexture_;

	SharedPtr<GfxRenderSurfaceMetal> renderTarget_;
	SharedPtr<GfxRenderSurfaceMetal> depthStencil_;
};

}
