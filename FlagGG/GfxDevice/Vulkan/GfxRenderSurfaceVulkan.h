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
	explicit GfxRenderSurfaceVulkan(GfxTextureVulkan* ownerTexture, VkImageView vkImageView);

	explicit GfxRenderSurfaceVulkan(GfxSwapChainVulkan* ownerSwapChain, VkFormat vkFormat, VkImageView vkImageView);

	~GfxRenderSurfaceVulkan() override;

	// 获取ImageView宽
	UInt32 GetWidth() const { return width_; }

	// 获取ImageView高
	UInt32 GetHeight() const { return height_; }

	// 获取format
	VkFormat GetVulkanFormat() { return vkFormat_; }

	// 获取ImageView
	VkImageView GetVulkanImageView() { return vkImageView_; }

	// Call by GfxSwapChainVulkan
	void UpdateImageView(VkImageView vkImageView);

private:
	UInt32 width_;

	UInt32 height_;

	// Surface format
	VkFormat vkFormat_;

	// Surface image view
	VkImageView vkImageView_;
};

}
