#pragma once

#include "Core/Object.h"
#include "GfxDevice/Vulkan/GfxBufferVulkan.h"
#include "GfxDevice/Vulkan/GfxShaderVulkan.h"

#include <vulkan-local/vulkan.h>

namespace FlagGG
{

class ShaderParameters;

class VulkanDynamicUniformBuffer : public Object
{
	OBJECT_OVERRIDE(VulkanDynamicUniformBuffer, Object);
public:
	explicit VulkanDynamicUniformBuffer(UInt32 size);

	~VulkanDynamicUniformBuffer() override;

	// 开始写
	void BeginWrite();

	// 结束写
	void EndWrite();

	// 开始新一段参数内存
	void BeginSegment(UInt32 size);

	// 结束当前段参数内存
	void EndSegment();

	// 拷贝参数到buffer
	void CopyShaderParameters(ShaderParameters* shaderParameters, const VulkanConstantBufferVariableDesc& variableDesc);

	// 当前段的开始偏移
	UInt32 GetSegmentStart() const { return currentSegmentStart_; }

	// 获取vulkan buffer
	VkBuffer GetVulkanBuffer() { return bufferVulkan_.GetVulkanBuffer(); }

private:
	GfxBufferVulkan bufferVulkan_;

	char* data_;

	UInt32 currentSegmentStart_;

	UInt32 currentSegmentSize_;
};

}
