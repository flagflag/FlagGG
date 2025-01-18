//
// Vulkan图形层SwapChain
//

#pragma once

#include "GfxDevice/GfxSwapChain.h"
#include "GfxDevice/Vulkan/GfxRenderSurfaceVulkan.h"
#include "Container/Ptr.h"

#include <vulkan-local/vulkan.h>

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

	// 获取backbuffer宽
	UInt32 GetBackbufferWidth() const { return backbufferWidth_; }

	// 获取backbuffer高
	UInt32 GetBackbufferHeight() const { return backbufferHeight_; }

	// 获取交换链
	VkSwapchainKHR GetVulkanSwapChain() { return vkSwapChain_; }

protected:
	void DestroyVKObject();

private:	
	Window* outputWindow_;

	// Backbuffer大小
	UInt32 backbufferWidth_;
	UInt32 backbufferHeight_;

	SharedPtr<GfxRenderSurfaceVulkan> renderTarget_;
	SharedPtr<GfxTextureVulkan> depthStencilTexture_;
	SharedPtr<GfxRenderSurfaceVulkan> depthStencil_;

	// vulkan surface
	VkSurfaceKHR vkSurface_;

	// vulkan swapchain
	VkSwapchainKHR vkSwapChain_;

	// color view
	VkImageView vkColorView_;
};

}
