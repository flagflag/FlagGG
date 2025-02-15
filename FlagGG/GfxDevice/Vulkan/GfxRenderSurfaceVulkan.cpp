#include "GfxRenderSurfaceVulkan.h"
#include "GfxTextureVulkan.h"
#include "GfxSwapChainVulkan.h"

namespace FlagGG
{

GfxRenderSurfaceVulkan::GfxRenderSurfaceVulkan(GfxTextureVulkan* ownerTexture, VkImageView vkImageView, UInt32 surfaceWidth, UInt32 surfaceHeight)
	: GfxRenderSurface(ownerTexture, surfaceWidth, surfaceHeight)
	, vkFormat_(ownerTexture->GetVulkanFormat())
	, vkImageView_(vkImageView)
{
}

GfxRenderSurfaceVulkan::GfxRenderSurfaceVulkan(GfxSwapChainVulkan* ownerSwapChain, VkFormat vkFormat, VkImageView vkImageView, UInt32 surfaceWidth, UInt32 surfaceHeight)
	: GfxRenderSurface(ownerSwapChain, surfaceWidth, surfaceHeight)
	, vkFormat_(vkFormat)
	, vkImageView_(vkImageView)
{

}

GfxRenderSurfaceVulkan::~GfxRenderSurfaceVulkan()
{

}

void GfxRenderSurfaceVulkan::UpdateImageView(VkImageView vkImageView)
{
	vkImageView_ = vkImageView;
}

}
