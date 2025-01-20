#include "GfxRenderSurfaceVulkan.h"
#include "GfxTextureVulkan.h"
#include "GfxSwapChainVulkan.h"

namespace FlagGG
{

GfxRenderSurfaceVulkan::GfxRenderSurfaceVulkan(GfxTextureVulkan* ownerTexture)
	: GfxRenderSurface(ownerTexture)
	, width_(ownerTexture->GetDesc().width_)
	, height_(ownerTexture->GetDesc().height_)
	, vkFormat_(ownerTexture->GetVulkanFormat())
	, vkImageView_(ownerTexture->GetVulkanImageView())
{
}

GfxRenderSurfaceVulkan::GfxRenderSurfaceVulkan(GfxSwapChainVulkan* ownerSwapChain, VkFormat vkFormat, VkImageView vkImageView)
	: GfxRenderSurface(ownerSwapChain)
	, vkFormat_(vkFormat)
	, vkImageView_(vkImageView)
	, width_(ownerSwapChain->GetBackbufferWidth())
	, height_(ownerSwapChain->GetBackbufferHeight())
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
