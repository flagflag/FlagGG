#include "GfxProgramVulkan.h"
#include "GfxShaderVulkan.h"
#include "GfxDeviceVulkan.h"
#include "VulkanDefines.h"
#include "Log.h"

namespace FlagGG
{

GfxProgramVulkan::GfxProgramVulkan()
	: vkDescSetLayout_(VK_NULL_HANDLE)
	, vkPipelineLayout_(VK_NULL_HANDLE)
{

}

GfxProgramVulkan::~GfxProgramVulkan()
{

}

void GfxProgramVulkan::Link(GfxShader* vertexShader, GfxShader* pixelShader)
{
	if (!vertexShader || !pixelShader)
	{
		FLAGGG_LOG_ERROR("Vertex shader or pixel shader is null.");
		return;
	}

	GfxShaderVulkan* vulkanVS = vertexShader->Cast<GfxShaderVulkan>();
	GfxShaderVulkan* vulkanPS = pixelShader->Cast<GfxShaderVulkan>();

	if (!vulkanVS || !vulkanPS)
	{
		FLAGGG_LOG_ERROR("Not opengl shader.");
		return;
	}

	GfxProgram::Link(vertexShader, pixelShader);

	auto* deviceVulkan = GetSubsystem<GfxDeviceVulkan>();

	const auto& vertexBindings = vulkanVS->GetVulkanBindings();
	const auto& pixelBindings = vulkanPS->GetVulkanBindings();
	PODVector<VkDescriptorSetLayoutBinding> vkBindings;
	vkBindings += vertexBindings;
	vkBindings += pixelBindings;

	VkDescriptorSetLayoutCreateInfo vkDSLCI;
	vkDSLCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	vkDSLCI.pNext = nullptr;
	vkDSLCI.flags = 0;
	vkDSLCI.bindingCount = vkBindings.Size();
	vkDSLCI.pBindings = &vkBindings[0];
	VULKAN_CHECK(vkCreateDescriptorSetLayout(deviceVulkan->GetVulkanDevice(), &vkDSLCI, &deviceVulkan->GetVulkanAllocCallback(), &vkDescSetLayout_));

	VkPipelineLayoutCreateInfo vkPLCI;
	vkPLCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	vkPLCI.pNext = nullptr;
	vkPLCI.flags = 0;
	vkPLCI.setLayoutCount = 1;
	vkPLCI.pSetLayouts = &vkDescSetLayout_;
	vkPLCI.pushConstantRangeCount = 0;
	vkPLCI.pPushConstantRanges = nullptr;
	VULKAN_CHECK(vkCreatePipelineLayout(deviceVulkan->GetVulkanDevice(), &vkPLCI, &deviceVulkan->GetVulkanAllocCallback(), &vkPipelineLayout_));
}

void GfxProgramVulkan::LinkComputeShader(GfxShader* computeShader)
{
	if (!computeShader)
	{
		FLAGGG_LOG_ERROR("Compute shader is null.");
		return;
	}

	GfxShaderVulkan* metalCS = computeShader->Cast<GfxShaderVulkan>();

	if (!metalCS)
	{
		FLAGGG_LOG_ERROR("Not opengl shader.");
		return;
	}

	// TODO:

	GfxProgram::LinkComputeShader(computeShader);
}

}
