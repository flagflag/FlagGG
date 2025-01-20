//
// Vulkan图形层纹理
//

#pragma once

#include "GfxDevice/GfxTexture.h"
#include "Container/Vector.h"
#include "Container/Ptr.h"

#include <vulkan-local/vulkan.h>

namespace FlagGG
{

class GfxRenderSurfaceVulkan;

class GfxTextureVulkan : public GfxTexture
{
	OBJECT_OVERRIDE(GfxTextureVulkan, GfxTexture);
public:
	explicit GfxTextureVulkan();

	~GfxTextureVulkan() override;

	// 应用当前设置
	void Apply(const void* initialDataPtr) override;

	// 更新纹理
	void UpdateTexture(const void* dataPtr) override;

	// 更新2D纹理区域
	void UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 width, UInt32 height) override;

	// 更新3D纹理区域
	void UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 z, UInt32 width, UInt32 height, UInt32 depth) override;

	// 更新纹理数据
	void UpdateTexture(GfxTexture* gfxTexture) override;


	// 获取texute2d的render surface
	GfxRenderSurface* GetRenderSurface() const override;

	// 获取render surface
	// 1.TextureArray，index传入array的下标
	// 2.TextureCube，index传入cube的face
	GfxRenderSurface* GetRenderSurface(UInt32 index) const override;

	// 内存同步
	void ImageMemoryBarrier(VkCommandBuffer vkCmdBuffer, VkImageLayout vkImageLayout);

	// 
	static void SetImageMemoryBarrier(VkCommandBuffer _commandBuffer, VkImage _image, VkImageAspectFlags _aspectMask, VkImageLayout _oldLayout, VkImageLayout _newLayout, uint32_t _levelCount, uint32_t _layerCount);

	// 获取vulkan纹理格式
	VkFormat GetVulkanFormat() const { return vkFormat_; }

	//
	VkImageAspectFlags GetVulkanImageAspect() const { return vkImageAspectMask_; }

	//
	VkImageLayout GetVulkanImageLayout() const { return vkImageLayout_; }

	// 获取纹理
	VkImage GetVulkanImage() const { return vkImage_; }

	// 获取纹理视图
	VkImageView GetVulkanImageView() const { return vkImageView_; }

	// 获取采样器视图
	VkImageView GetVulkanSamplerView();

	// 获取ComputeWrite视图
	VkImageView GetVulkanStorageView() const { return vkStorageView_; }

private:
	// 
	VkFormat vkFormat_;

	//
	VkImageAspectFlags vkImageAspectMask_;

	//
	VkImageLayout vkImageLayout_;

	// 
	VkImage vkImage_;

	// 
	VkDeviceMemory vkMemory_;

	// 纹理采样视图
	VkImageView vkImageView_;

	// 采样深度纹理 => DepthView
	VkImageView vkDepthView_;

	// Compute写 => StorageView
	VkImageView vkStorageView_;

	// render surfaces
	Vector<SharedPtr<GfxRenderSurfaceVulkan>> gfxRenderSurfaces_;
};

}
