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

private:
	// 
	VkImage vkImage_;

	// 
	VkDeviceMemory vkMemory_;

	// render surfaces
	Vector<SharedPtr<GfxRenderSurfaceVulkan>> gfxRenderSurfaces_;
};

}
