#include "GfxShaderVulkan.h"
#include "GfxDeviceVulkan.h"
#include "GfxDevice/Shader/ShaderTranslation.h"
#include "Log.h"
#include "VulkanDefines.h"
#include "Memory/Memory.h"

#include <SPIRV-Reflect/spirv_reflect.h>

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
	if (!CompileShader(COMPILE_SHADER_VULKAN, *shaderInfo_, shaderType_, defines_, vkShaderCode_))
		return false;

	AnalysisReflection();

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

const VulkanInputDesc* GfxShaderVulkan::FindInputDesc(const char* semanticName, UInt32 semanticIndex) const
{
	for (auto& it : inputDescs_)
	{
		if (it.semanticName_ == semanticName && it.semanticIndex_ == semanticIndex)
			return &it;
	}

	return nullptr;
}

void GfxShaderVulkan::AnalysisReflection()
{
	spv_reflect::ShaderModule reflection(vkShaderCode_.Length(), vkShaderCode_.CString());
	if (reflection.GetResult() != SPV_REFLECT_RESULT_SUCCESS)
	{
		throw "analysis reflection error!";
	}

	const SpvReflectShaderModule& shaderModule = reflection.GetShaderModule();

	for (UInt32 i = 0; i < shaderModule.input_variable_count; ++i)
	{
		auto* desc = shaderModule.input_variables[i];
		auto& inputDesc = inputDescs_.EmplaceBack();
		inputDesc.name_ = desc->name;
		String semantic = inputDesc.name_.Substring(3);
		UInt32 numerStartPos = semantic.Length();
		while (numerStartPos > 0)
		{
			if (::isdigit(semantic[numerStartPos - 1]))
			{
				--numerStartPos;
			}
			else
			{
				break;
			}
		}
		inputDesc.semanticName_ = semantic.Substring(0, numerStartPos);
		inputDesc.semanticIndex_ = ToInt(semantic.Substring(numerStartPos));
		inputDesc.location_ = desc->location;
	}

	vkBindings_.Clear();

	for (UInt32 i = 0; i < shaderModule.descriptor_binding_count; ++i)
	{
		const auto& desc = shaderModule.descriptor_bindings[i];

		// Uniform buffer
		if (desc.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
		{
			const auto& block = desc.block;
			const UInt32 engineRegister = desc.binding - 32 - (shaderType_ == PS ? 48 : 0);
			auto& uboDesc = constantBufferDescs_[engineRegister];
			uboDesc.name_ = desc.name;
			uboDesc.size_ = 0;
			uboDesc.binding_ = desc.binding;

			uboDesc.variableDescs_.Resize(block.member_count);

			for (UInt32 j = 0; j < block.member_count; ++j)
			{
				const auto& var = block.members[j];
				auto& uniformDesc = uboDesc.variableDescs_[j];
				uniformDesc.name_ = var.name;
				uniformDesc.nameHash_ = StringHash(uniformDesc.name_);
				uniformDesc.offset_ = var.offset;
				uniformDesc.size_ = var.size;
				// 内存可能出现直接对齐，所以取最大的offset+size
				if (var.offset + var.size > uboDesc.size_)
					uboDesc.size_ = var.offset + var.size;
				if (var.type_description->op == SpvOpTypeInt ||
					var.type_description->op == SpvOpTypeFloat ||
					var.type_description->op == SpvOpTypeVector ||
					var.type_description->op == SpvOpTypeMatrix)
				{
					uniformDesc.setterFunc_ = [](const VulkanConstantBufferVariableDesc& desc, const void* uniformValue, void* targetData)
					{
						Memory::Memcpy(targetData, uniformValue, desc.size_);
					};
				}
			}
		}
		// Texture
		else if (desc.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
		{
			const UInt32 engineRegister = desc.binding - 16 - (shaderType_ == PS ? 48 : 0);
			auto& texDesc = textureDescs_[engineRegister];
			texDesc.textureName_ = desc.name;
			texDesc.textureBinding_ = desc.binding;
		}
		// Sampler
		else if (desc.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER)
		{
			const UInt32 engineRegister = desc.binding - (shaderType_ == PS ? 48 : 0);
			auto& texDesc = textureDescs_[engineRegister];
			texDesc.samplerName_ = desc.name;
			texDesc.samplerBinding_ = desc.binding;
		}

		vkBindings_.Resize(vkBindings_.Size() + 1);
		VkDescriptorSetLayoutBinding& vkBinding = vkBindings_.Back();
		vkBinding.binding = desc.binding;
		// 枚举类型SpvReflectDescriptorType和VkDescriptorType的值是一一对应的
		vkBinding.descriptorType = (VkDescriptorType)desc.descriptor_type;
		vkBinding.descriptorCount = 1;
		if (shaderType_ == VS)
			vkBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		else if (shaderType_ == PS)
			vkBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		else if (shaderType_ == CS)
			vkBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		vkBinding.pImmutableSamplers = nullptr;
	}
}

}
