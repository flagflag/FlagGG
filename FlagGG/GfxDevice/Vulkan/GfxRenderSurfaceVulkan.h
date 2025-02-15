//
// Vulkan图形层RenderSurface
//

#pragma once

#include "GfxDevice/GfxRenderSurface.h"

#include <vulkan-local/vulkan.h>

namespace FlagGG
{

class GfxTextureVulkan;
class GfxSwapChainVulkan;

class GfxRenderSurfaceVulkan : public GfxRenderSurface
{
	OBJECT_OVERRIDE(GfxRenderSurfaceVulkan, GfxRenderSurface);
public:
	explicit GfxRenderSurfaceVulkan(GfxTextureVulkan* ownerTexture, VkImageView vkImageView, UInt32 surfaceWidth, UInt32 surfaceHeight);

	explicit GfxRenderSurfaceVulkan(GfxSwapChainVulkan* ownerSwapChain, VkFormat vkFormat, VkImageView vkImageView, UInt32 surfaceWidth, UInt32 surfaceHeight);

	~GfxRenderSurfaceVulkan() override;

	// 获取format
	VkFormat GetVulkanFormat() { return vkFormat_; }

	// 获取ImageView
	VkImageView GetVulkanImageView() { return vkImageView_; }

	// Call by GfxSwapChainVulkan
	void UpdateImageView(VkImageView vkImageView);

private:
	// Surface format
	VkFormat vkFormat_;

	// Surface image view
	VkImageView vkImageView_;
};

}
