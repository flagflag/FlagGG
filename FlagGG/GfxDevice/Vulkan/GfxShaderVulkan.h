//
// Vulkan图形层Shader
//

#pragma once

#include "GfxDevice/GfxShader.h"

#include <vulkan-local/vulkan.h>

namespace FlagGG
{

class GfxShaderVulkan : public GfxShader
{
	OBJECT_OVERRIDE(GfxShaderVulkan, GfxShader);
public:
	explicit GfxShaderVulkan();

	~GfxShaderVulkan() override;

	// 编译
	bool Compile() override;

	// 获取vulkan shader module
	VkShaderModule GetVulkanShader() { return vkShader_; }

private:
	String vkShaderCode_;

	// 
	VkShaderModule vkShader_;
};

}
