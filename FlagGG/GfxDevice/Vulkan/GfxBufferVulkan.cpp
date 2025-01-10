#include "GfxBufferVulkan.h"
#include "GfxDeviceVulkan.h"
#include "VulkanDefines.h"
#include "Memory/Memory.h"

namespace FlagGG
{

GfxBufferVulkan::GfxBufferVulkan()
	: lastWriteBeginOffset_(0)
	, vkBuffer_(VK_NULL_HANDLE)
	, vkMemory_(VK_NULL_HANDLE)
	, vkWriteBuffer_(VK_NULL_HANDLE)
	, vkWriteMemory_(VK_NULL_HANDLE)
{

}

GfxBufferVulkan::~GfxBufferVulkan()
{
	auto* deviceVulkan = GetSubsystem<GfxDeviceVulkan>();

	if (vkMemory_)
	{
		vkFreeMemory(deviceVulkan->GetVulkanDevice(), vkMemory_, &deviceVulkan->GetVulkanAllocCallback());
		vkMemory_ = VK_NULL_HANDLE;
	}

	if (vkBuffer_)
	{
		vkDestroyBuffer(deviceVulkan->GetVulkanDevice(), vkBuffer_, &deviceVulkan->GetVulkanAllocCallback());
		vkBuffer_ = VK_NULL_HANDLE;
	}

	if (vkWriteMemory_)
	{
		vkFreeMemory(deviceVulkan->GetVulkanDevice(), vkWriteMemory_, &deviceVulkan->GetVulkanAllocCallback());
		vkWriteMemory_ = VK_NULL_HANDLE;
	}

	if (vkWriteBuffer_)
	{
		vkDestroyBuffer(deviceVulkan->GetVulkanDevice(), vkWriteBuffer_, &deviceVulkan->GetVulkanAllocCallback());
		vkWriteBuffer_ = VK_NULL_HANDLE;
	}
}

void GfxBufferVulkan::Apply(const void* initialDataPtr)
{
	auto* deviceVulkan = GetSubsystem<GfxDeviceVulkan>();

// 创建vulkan buffer
	VkBufferCreateInfo vkBCI;
	vkBCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBCI.pNext = nullptr;
	vkBCI.flags = 0;
	vkBCI.size = gfxBufferDesc_.size_;
	vkBCI.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	if (gfxBufferDesc_.bindFlags_ & BUFFER_BIND_VERTEX)
		vkBCI.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	if (gfxBufferDesc_.bindFlags_ & BUFFER_BIND_INDEX)
		vkBCI.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	// 通常DrawIndirect配合ComputeShader使用，由Compute计算（例如：GpuDriven）
	if (gfxBufferDesc_.bindFlags_ & (BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE | BUFFER_BIND_DRAW_INDIRECT))
		vkBCI.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	if (gfxBufferDesc_.bindFlags_ & BUFFER_BIND_DRAW_INDIRECT)
		vkBCI.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
	vkBCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vkBCI.queueFamilyIndexCount = 0;
	vkBCI.pQueueFamilyIndices = nullptr;

	VULKAN_CHECK(vkCreateBuffer(deviceVulkan->GetVulkanDevice(), &vkBCI, &deviceVulkan->GetVulkanAllocCallback(), &vkBuffer_));

// 分配显存
	VkMemoryRequirements vkMR;
	vkGetBufferMemoryRequirements(deviceVulkan->GetVulkanDevice(), vkBuffer_, &vkMR);

	VkMemoryPropertyFlags vkMemPropFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	if (gfxBufferDesc_.accessFlags_ & BUFFER_ACCESS_READ)
		vkMemPropFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	if (gfxBufferDesc_.accessFlags_ & BUFFER_ACCESS_WRITE)
		vkMemPropFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	VkMemoryAllocateInfo vkMAI;
	vkMAI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMAI.pNext = nullptr;
	vkMAI.allocationSize = vkMR.size;
	vkMAI.memoryTypeIndex = deviceVulkan->GetVulkanMemoryTypeIndex(vkMR.memoryTypeBits, vkMemPropFlags);
	VULKAN_CHECK(vkAllocateMemory(deviceVulkan->GetVulkanDevice(), &vkMAI, &deviceVulkan->GetVulkanAllocCallback(), &vkMemory_));

// 关联显存
	VULKAN_CHECK(vkBindBufferMemory(deviceVulkan->GetVulkanDevice(), vkBuffer_, vkMemory_, 0));

	if (initialDataPtr)
	{
		UpdateBuffer(initialDataPtr);
	}
}

void GfxBufferVulkan::UpdateBuffer(const void* dataPtr)
{
	auto* cpuMemory = BeginWrite(0, gfxBufferDesc_.size_);
	Memory::Memcpy(cpuMemory, dataPtr, gfxBufferDesc_.size_);
	EndWrite(gfxBufferDesc_.size_);
}

void GfxBufferVulkan::UpdateBufferRange(const void* dataPtr, UInt32 offset, UInt32 size)
{
	auto* cpuMemory = BeginWrite(offset, size);
	Memory::Memcpy(cpuMemory, dataPtr, size);
	EndWrite(size);
}

void* GfxBufferVulkan::BeginWrite(UInt32 offset, UInt32 size)
{
	if (!vkBuffer_)
	{
		ASSERT_MESSAGE(false, "vkBuffer_ is nullptr");
		return nullptr;
	}

	if (offset + size > gfxBufferDesc_.size_)
	{
		ASSERT_MESSAGE(false, "The size is too large");
		return nullptr;
	}

	if (gfxBufferDesc_.accessFlags_ & BUFFER_ACCESS_WRITE)
	{
		void* cpuMemory = nullptr;
		VULKAN_CHECK(vkMapMemory(GetSubsystem<GfxDeviceVulkan>()->GetVulkanDevice(), vkMemory_, offset, size, 0, &cpuMemory), nullptr);
		return cpuMemory;
	}
	else
	{
		return CreateWriteBuffer(offset, size);
	}
}

void GfxBufferVulkan::EndWrite(UInt32 bytesWritten)
{
	if (!vkBuffer_)
	{
		ASSERT_MESSAGE(false, "vkBuffer_ is nullptr");
		return;
	}

	if (gfxBufferDesc_.accessFlags_ & BUFFER_ACCESS_WRITE)
	{
		vkUnmapMemory(GetSubsystem<GfxDeviceVulkan>()->GetVulkanDevice(), vkMemory_);
	}
	else
	{
		SubmitWriteBuffer(bytesWritten);
	}
}

void* GfxBufferVulkan::CreateWriteBuffer(UInt32 offset, UInt32 size)
{
	auto* deviceVulkan = GetSubsystem<GfxDeviceVulkan>();

	lastWriteBeginOffset_ = offset;

// 创建临时buffer
	VkBufferCreateInfo vkBCI;
	vkBCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBCI.pNext = nullptr;
	vkBCI.flags = 0;
	vkBCI.size = size;
	vkBCI.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	vkBCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vkBCI.queueFamilyIndexCount = 0;
	vkBCI.pQueueFamilyIndices = nullptr;
	VULKAN_CHECK(vkCreateBuffer(deviceVulkan->GetVulkanDevice(), &vkBCI, &deviceVulkan->GetVulkanAllocCallback(), &vkWriteBuffer_), nullptr);

// 分配CPU内存 => VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	VkMemoryRequirements vkMR;
	vkGetBufferMemoryRequirements(deviceVulkan->GetVulkanDevice(), vkWriteBuffer_, &vkMR);
	  
	VkMemoryAllocateInfo vkMAI;
	vkMAI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMAI.pNext = nullptr;
	vkMAI.allocationSize = vkMR.size;
	vkMAI.memoryTypeIndex = deviceVulkan->GetVulkanMemoryTypeIndex(vkMR.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VULKAN_CHECK(vkAllocateMemory(deviceVulkan->GetVulkanDevice(), &vkMAI, &deviceVulkan->GetVulkanAllocCallback(), &vkWriteMemory_), nullptr);

// 关联CPU内存，并且把shadowData_拷贝到
	VULKAN_CHECK(vkBindBufferMemory(deviceVulkan->GetVulkanDevice(), vkWriteBuffer_, vkWriteMemory_, 0), nullptr);

	void* cpuMemory = nullptr;
	VULKAN_CHECK(vkMapMemory(deviceVulkan->GetVulkanDevice(), vkWriteMemory_, 0, size, 0, &cpuMemory), nullptr);

	return cpuMemory;
}

void GfxBufferVulkan::SubmitWriteBuffer(UInt32 bytesWritten)
{
	auto* deviceVulkan = GetSubsystem<GfxDeviceVulkan>();

	vkUnmapMemory(deviceVulkan->GetVulkanDevice(), vkWriteMemory_);

// 创建命令buffer，把cpu内存拷贝到vkBuffer_对应的GPU内存上
	VkCommandBuffer vkCmdBuffer = deviceVulkan->BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkBufferCopy vkBC;
	vkBC.srcOffset = 0;
	vkBC.dstOffset = lastWriteBeginOffset_;
	vkBC.size = bytesWritten;
	vkCmdCopyBuffer(vkCmdBuffer, vkWriteBuffer_, vkBuffer_, 1, &vkBC);

	deviceVulkan->EndCommandBuffer(vkCmdBuffer, true);

	vkFreeMemory(deviceVulkan->GetVulkanDevice(), vkWriteMemory_, &deviceVulkan->GetVulkanAllocCallback());
	vkDestroyBuffer(deviceVulkan->GetVulkanDevice(), vkWriteBuffer_, &deviceVulkan->GetVulkanAllocCallback());

	vkWriteMemory_ = nullptr;
	vkWriteBuffer_ = nullptr;
}

}
