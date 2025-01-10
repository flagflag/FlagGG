#include "GfxTextureVulkan.h"
#include "GfxDeviceVulkan.h"
#include "GfxRenderSurfaceVulkan.h"
#include "VulkanDefines.h"
#include "Log.h"
#include "Memory/Memory.h"

namespace FlagGG
{

GfxTextureVulkan::GfxTextureVulkan()
{

}

GfxTextureVulkan::~GfxTextureVulkan()
{

}

void GfxTextureVulkan::Apply(const void* initialDataPtr)
{
	if (!textureDesc_.width_ || !textureDesc_.height_)
	{
		FLAGGG_LOG_ERROR("Texture's width or height is invalid.");
		return;
	}

	textureDesc_.levels_ = GfxTextureUtils::CheckMaxLevels(textureDesc_.width_, textureDesc_.height_, textureDesc_.requestedLevels_);

	auto* deviceVulkan = GetSubsystem<GfxDeviceVulkan>();

// 创建vulkan image
	VkImageCreateInfo vkICI;
	vkICI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	vkICI.pNext = nullptr;
	vkICI.arrayLayers = textureDesc_.layers_;
	vkICI.extent.width = textureDesc_.width_;
	vkICI.extent.height = textureDesc_.height_;
	vkICI.extent.depth = textureDesc_.depth_;
	vkICI.flags = 0;
	vkICI.format = VK_FORMAT_R8G8B8A8_SRGB;
	if (textureDesc_.depth_ == 1)
		vkICI.imageType = VK_IMAGE_TYPE_2D;
	else
		vkICI.imageType = VK_IMAGE_TYPE_3D;
	vkICI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	vkICI.mipLevels = textureDesc_.levels_;
	vkICI.pQueueFamilyIndices = nullptr;
	vkICI.queueFamilyIndexCount = 0;
	vkICI.samples = VK_SAMPLE_COUNT_1_BIT;
	vkICI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vkICI.tiling = VK_IMAGE_TILING_OPTIMAL;
	vkICI.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	if (textureDesc_.usage_ == TEXTURE_RENDERTARGET)
		vkICI.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	else if (textureDesc_.usage_ == TEXTURE_DEPTHSTENCIL)
		vkICI.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	if (textureDesc_.bindFlags_ & TEXTURE_BIND_COMPUTE_WRITE)
		vkICI.usage |= VK_IMAGE_USAGE_STORAGE_BIT;

	vkCreateImage(deviceVulkan->GetVulkanDevice(), &vkICI, &deviceVulkan->GetVulkanAllocCallback(), &vkImage_);

// 分配显存
	VkMemoryRequirements vkMR;
	vkGetImageMemoryRequirements(deviceVulkan->GetVulkanDevice(), vkImage_, &vkMR);

	VkMemoryAllocateInfo vkMAI;
	vkMAI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMAI.pNext = nullptr;
	vkMAI.allocationSize = vkMR.size;
	vkMAI.memoryTypeIndex = deviceVulkan->GetVulkanMemoryTypeIndex(vkMR.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VULKAN_CHECK(vkAllocateMemory(deviceVulkan->GetVulkanDevice(), &vkMAI, &deviceVulkan->GetVulkanAllocCallback(), &vkMemory_));

// 关联显存
	VULKAN_CHECK(vkBindImageMemory(deviceVulkan->GetVulkanDevice(), vkImage_, vkMemory_, 0));

	if (initialDataPtr)
		UpdateTexture(initialDataPtr);

	gfxRenderSurfaces_.Clear();
	gfxRenderSurfaces_.Push(MakeShared<GfxRenderSurfaceVulkan>(this));
}

void GfxTextureVulkan::UpdateTexture(const void* dataPtr)
{

}

void GfxTextureVulkan::UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 width, UInt32 height)
{
	if (!vkImage_)
	{
		FLAGGG_LOG_ERROR("Gfx texture invalid or format conflict.");
		return;
	}

	if (!dataPtr)
	{
		FLAGGG_LOG_ERROR("Texture2D ==> set nullptr data.");
		return;
	}

	if (level >= textureDesc_.levels_)
	{
		FLAGGG_LOG_ERROR("Texture2D ==> illegal mip level.");
		return;
	}

	const TextureMipInfo mipInfo = GetMipInfo(level);
	if (x < 0 || x + width > mipInfo.width_ || y < 0 || y + height > mipInfo.height_ || width <= 0 || height <= 0)
	{
		FLAGGG_LOG_ERROR("Texture2D ==> illegal dimensions.");
		return;
	}

	auto* deviceVulkan = GetSubsystem<GfxDeviceVulkan>();

	UInt32 rowSize = GfxTextureUtils::GetRowDataSize(textureDesc_.format_, mipInfo.width_);
	UInt32 rowStart = GfxTextureUtils::GetRowDataSize(textureDesc_.format_, x);

	VkBuffer vkTempBuffer;
	VkDeviceMemory vkTempMemory;

// 创建临时buffer
	VkBufferCreateInfo vkBCI;
	vkBCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBCI.pNext = nullptr;
	vkBCI.pQueueFamilyIndices = nullptr;
	vkBCI.queueFamilyIndexCount = 0;
	vkBCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vkBCI.size = rowSize * height;
	vkBCI.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VULKAN_CHECK(vkCreateBuffer(deviceVulkan->GetVulkanDevice(), &vkBCI, &deviceVulkan->GetVulkanAllocCallback(), &vkTempBuffer));

// 分配CPU内存 => VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	VkMemoryRequirements vkMR;
	vkGetBufferMemoryRequirements(deviceVulkan->GetVulkanDevice(), vkTempBuffer, &vkMR);

	VkMemoryAllocateInfo vkMAI;
	vkMAI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMAI.pNext = nullptr;
	vkMAI.allocationSize = vkMR.size;
	vkMAI.memoryTypeIndex = deviceVulkan->GetVulkanMemoryTypeIndex(vkMR.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VULKAN_CHECK(vkAllocateMemory(deviceVulkan->GetVulkanDevice(), &vkMAI, &deviceVulkan->GetVulkanAllocCallback(), &vkTempMemory));

// 关联CPU内存，并且把shadowData_拷贝到
	VULKAN_CHECK(vkBindBufferMemory(deviceVulkan->GetVulkanDevice(), vkTempBuffer, vkTempMemory, 0));

//
	void* cpuMemory = nullptr;
	VULKAN_CHECK(vkMapMemory(deviceVulkan->GetVulkanDevice(), vkTempMemory, 0, rowSize * height, 0, &cpuMemory));
	Memory::Memcpy(cpuMemory, dataPtr, rowSize * height);
	vkUnmapMemory(deviceVulkan->GetVulkanDevice(), vkTempMemory);

	VkCommandBuffer vkCmdBuffer = deviceVulkan->BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// setImageMemoryBarrier
	VkBufferImageCopy vkBufferCopyInfo;
	vkBufferCopyInfo.bufferImageHeight = 0;
	vkBufferCopyInfo.bufferOffset = 0;
	vkBufferCopyInfo.bufferRowLength = 0;
	vkBufferCopyInfo.imageExtent = { width, height, 0 };
	vkBufferCopyInfo.imageOffset = { Int32(x), Int32(y), 0 };
	vkBufferCopyInfo.imageSubresource.aspectMask = 0;
	vkBufferCopyInfo.imageSubresource.mipLevel = 0;
	vkBufferCopyInfo.imageSubresource.baseArrayLayer = 0;
	vkBufferCopyInfo.imageSubresource.layerCount = 0;
	vkCmdCopyBufferToImage(vkCmdBuffer, vkTempBuffer, vkImage_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vkBufferCopyInfo);

	deviceVulkan->EndCommandBuffer(vkCmdBuffer, true);

	vkFreeMemory(deviceVulkan->GetVulkanDevice(), vkTempMemory, &deviceVulkan->GetVulkanAllocCallback());
	vkDestroyBuffer(deviceVulkan->GetVulkanDevice(), vkTempBuffer, &deviceVulkan->GetVulkanAllocCallback());
}

void GfxTextureVulkan::UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 z, UInt32 width, UInt32 height, UInt32 depth)
{

}

void GfxTextureVulkan::UpdateTexture(GfxTexture* gfxTexture)
{

}

GfxRenderSurface* GfxTextureVulkan::GetRenderSurface() const
{
	return gfxRenderSurfaces_.Size() ? gfxRenderSurfaces_[0] : nullptr;
}

GfxRenderSurface* GfxTextureVulkan::GetRenderSurface(UInt32 index) const
{
	return index < gfxRenderSurfaces_.Size() ? gfxRenderSurfaces_[index] : nullptr;
}

}
