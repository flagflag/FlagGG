//
// Vulkan图形层Shader
//

#pragma once

#include "GfxDevice/GfxShader.h"
#include "Container/Vector.h"
#include "Container/HashMap.h"

#include <vulkan-local/vulkan.h>

namespace FlagGG
{

struct VulkanConstantBufferVariableDesc;

typedef void(*SetUniformToBuffer)(const VulkanConstantBufferVariableDesc& desc, const void* uniformValue, void* targetData);

struct VulkanConstantBufferVariableDesc
{
	String name_;
	StringHash nameHash_;
	UInt32 offset_;
	UInt32 size_;
	SetUniformToBuffer setterFunc_;
};

struct VulkanConstanceBufferDesc
{
	String name_;
	UInt32 size_;
	UInt32 binding_;
	Vector<VulkanConstantBufferVariableDesc> variableDescs_;
};

struct VulkanShaderTextureDesc
{
	String textureName_;
	String samplerName_;
	UInt32 textureBinding_;
	UInt32 samplerBinding_;
};

struct VulkanInputDesc
{
	String name_;
	String semanticName_;
	UInt32 semanticIndex_;
	UInt32 location_;
};

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

	//
	const PODVector<VkDescriptorSetLayoutBinding>& GetVulkanBindings() const { return vkBindings_; }

	// 
	const VulkanInputDesc* FindInputDesc(const char* semanticName, UInt32 semanticIndex) const;

	// 获取输入描述
	const Vector<VulkanInputDesc>& GetInputDescs() const { return inputDescs_; }

	// 获取ConstantBuffer描述
	const HashMap<UInt32, VulkanConstanceBufferDesc>& GetContantBufferVariableDesc() const { return constantBufferDescs_; }

	// 获取纹理描述
	const HashMap<UInt32, VulkanShaderTextureDesc>& GetTextureDesc() const { return textureDescs_; }

protected:
	void AnalysisReflection();

private:
	// Spir-V code
	String vkShaderCode_;

	// vulkan shader
	VkShaderModule vkShader_;

	PODVector<VkDescriptorSetLayoutBinding> vkBindings_;

	// 输入变量
	Vector<VulkanInputDesc> inputDescs_;
	// 常量buffer描述
	HashMap<UInt32, VulkanConstanceBufferDesc> constantBufferDescs_;
	// 纹理描述
	HashMap<UInt32, VulkanShaderTextureDesc> textureDescs_;
};

}
