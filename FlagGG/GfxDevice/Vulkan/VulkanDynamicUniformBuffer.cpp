#include "VulkanDynamicUniformBuffer.h"
#include "GfxDeviceVulkan.h"
#include "Graphics/ShaderParameter.h"

namespace FlagGG
{

VulkanDynamicUniformBuffer::VulkanDynamicUniformBuffer(UInt32 size)
{
	bufferVulkan_.SetStride(4u);
	bufferVulkan_.SetSize(size);
	bufferVulkan_.SetBind(BUFFER_BIND_UNIFORM);
	bufferVulkan_.SetAccess(BUFFER_ACCESS_READ | BUFFER_ACCESS_WRITE);
	bufferVulkan_.SetUsage(BUFFER_USAGE_STATIC);
	bufferVulkan_.Apply(nullptr);
}

VulkanDynamicUniformBuffer::~VulkanDynamicUniformBuffer()
{
	
}

void VulkanDynamicUniformBuffer::BeginWrite()
{
	data_ = (char*)bufferVulkan_.BeginWrite(0, bufferVulkan_.GetDesc().size_);
	currentSegmentStart_ = 0u;
	currentSegmentSize_ = 0u;
}

void VulkanDynamicUniformBuffer::EndWrite()
{
	bufferVulkan_.EndWrite(currentSegmentStart_);
}

void VulkanDynamicUniformBuffer::BeginSegment(UInt32 size)
{
	currentSegmentSize_ = size;
}

void VulkanDynamicUniformBuffer::EndSegment()
{
	currentSegmentStart_ += currentSegmentSize_;
	currentSegmentSize_ = 0;
}

void VulkanDynamicUniformBuffer::CopyShaderParameters(ShaderParameters* shaderParameters, const VulkanConstantBufferVariableDesc& variableDesc)
{
	shaderParameters->ReadParameter(variableDesc.nameHash_, data_ + currentSegmentStart_ + variableDesc.offset_, variableDesc.size_);
}

}
