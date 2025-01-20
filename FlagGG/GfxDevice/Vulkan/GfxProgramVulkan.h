//
// Vulkan图形层GPU程序
//

#pragma once

#include "GfxDevice/GfxProgram.h"

#include <vulkan-local/vulkan.h>

namespace FlagGG
{

class GfxProgramVulkan : public GfxProgram
{
	OBJECT_OVERRIDE(GfxProgramVulkan, GfxProgram);
public:
	explicit GfxProgramVulkan();

	~GfxProgramVulkan() override;

	// link vs ps
	void Link(GfxShader* vertexShader, GfxShader* pixelShader) override;

	// link cs
	void LinkComputeShader(GfxShader* computeShader) override;

	//
	VkDescriptorSetLayout GetVulkanDescSetLayout() const { return vkDescSetLayout_; }

	//
	VkPipelineLayout GetVulkanPipelineLayout() { return vkPipelineLayout_; }

private:
	VkDescriptorSetLayout vkDescSetLayout_;
	
	VkPipelineLayout vkPipelineLayout_;
};

}
