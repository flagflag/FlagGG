#include "GfxShaderVulkan.h"
#include "GfxDeviceVulkan.h"
#include "GfxDevice/Shader/ShaderTranslation.h"
#include "Log.h"
#include "VulkanDefines.h"

namespace FlagGG
{

GfxShaderVulkan::GfxShaderVulkan()
{

}

GfxShaderVulkan::~GfxShaderVulkan()
{

}

bool GfxShaderVulkan::Compile()
{
	if (!CompileShader(COMPILE_SHADER_VULKAN, shaderSource_.Get(), shaderSourceSize_, shaderType_, defines_, vkShaderCode_))
		return false;

	auto* deviceVuklan = GetSubsystem<GfxDeviceVulkan>();

	VkShaderModuleCreateInfo vkSMCI;
	vkSMCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vkSMCI.pNext = nullptr;
	vkSMCI.codeSize = vkShaderCode_.Length();
	vkSMCI.flags = 0;
	vkSMCI.pCode = (const uint32_t*)vkShaderCode_.CString();
	VULKAN_CHECK(vkCreateShaderModule(deviceVuklan->GetVulkanDevice(), &vkSMCI, &deviceVuklan->GetVulkanAllocCallback(), &vkShader_), false);
	
	return true;
}

}
