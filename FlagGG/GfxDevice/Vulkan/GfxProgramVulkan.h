//
// Vulkan图形层GPU程序
//

#pragma once

#include "GfxDevice/GfxProgram.h"
#include "Container/Vector.h"
#include "Container/HashMap.h"

namespace FlagGG
{

struct VulkanConstantBufferVariableDesc;

typedef void(*SetUniformToBuffer)(const VulkanConstantBufferVariableDesc& desc, const void* uniformValue, void* targetData);

struct VulkanConstantBufferVariableDesc
{
	String name_;
	UInt32 offset_;
	UInt32 dataType_; // 例如：移动端数据类型是HalfFloat，则需要做转换（CPU传过来的数据一定是float，需要转half）
	SetUniformToBuffer setterFunc_;
};

struct VulkanConstanceBufferDesc
{
	String name_;
	UInt32 size_;
	Vector<VulkanConstantBufferVariableDesc> variableDescs_;
};

struct VulkanShaderTextureDesc
{
	String textureName_;
	String samplerName_;
};

struct VulkanVertexInputDesc
{

};

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

	// 获取ConstantBuffer描述
	const HashMap<UInt32, VulkanConstanceBufferDesc>& GetContantBufferVariableDesc(ShaderType type) const { return constantBufferDescs_[type]; }

	// 获取纹理描述
	const HashMap<UInt32, VulkanShaderTextureDesc>& GetTextureDesc(ShaderType type) const { return textureDescs_[type]; }

	// 获取顶点输入描述
	const HashMap<StringHash, VulkanVertexInputDesc>& GetVertexInputDescs() const { return vertexInputDescs_; }

protected:
	void ApplyUniformSetter(VulkanConstantBufferVariableDesc& varDesc);

private:
	// 常量buffer描述
	HashMap<UInt32, VulkanConstanceBufferDesc> constantBufferDescs_[3];
	// 纹理描述
	HashMap<UInt32, VulkanShaderTextureDesc> textureDescs_[3];
	// Key: ElementSemName hash, Value: { location, variableName }
	HashMap<StringHash, VulkanVertexInputDesc> vertexInputDescs_;
};

}
