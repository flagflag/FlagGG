//
// Vulkan图形层SwapChain
//

#pragma once

#include "GfxDevice/GfxSwapChain.h"
#include "GfxDevice/Vulkan/GfxRenderSurfaceVulkan.h"
#include "Container/Ptr.h"

namespace FlagGG
{

class GfxSwapChainVulkan : public GfxSwapChain
{
	OBJECT_OVERRIDE(GfxSwapChainVulkan, GfxSwapChain);
public:
	explicit GfxSwapChainVulkan(Window* window);

	~GfxSwapChainVulkan() override;

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

	SharedPtr<GfxRenderSurfaceVulkan> renderTarget_;
	SharedPtr<GfxRenderSurfaceVulkan> depthStencil_;
};

}
