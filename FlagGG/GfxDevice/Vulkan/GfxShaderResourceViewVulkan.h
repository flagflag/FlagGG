//
// Vulkan图形层TextureView
//

#pragma once

#include "GfxDevice/GfxShaderResourceView.h"

#include <vulkan-local/vulkan.h>

namespace FlagGG
{

class GfxShaderResourceViewVulkan : public GfxShaderResourceView
{
	OBJECT_OVERRIDE(GfxShaderResourceViewVulkan, GfxShaderResourceView);
public:
	GfxShaderResourceViewVulkan(GfxTexture* ownerTexture, VkImageView imageSamplerView);

	~GfxShaderResourceViewVulkan() override;

	// 获取Vulkan采样视图
	VkImageView GetVulkanSamplerView() const { return imageSamplerView_; }

private:
	// Vulkan采样视图
	VkImageView imageSamplerView_{};
};

}
