#include "GfxRenderSurfaceVulkan.h"
#include "GfxTextureVulkan.h"
#include "GfxSwapChainVulkan.h"

namespace FlagGG
{

GfxRenderSurfaceVulkan::GfxRenderSurfaceVulkan(GfxTextureVulkan* ownerTexture)
	: GfxRenderSurface(ownerTexture)
	, vkImageView_(ownerTexture->GetVulkanImageView())
	, width_(ownerTexture->GetDesc().width_)
	, height_(ownerTexture->GetDesc().height_)
{
}

GfxRenderSurfaceVulkan::GfxRenderSurfaceVulkan(GfxSwapChainVulkan* ownerSwapChain, VkImageView vkImageView)
	: GfxRenderSurface(ownerSwapChain)
	, vkImageView_(vkImageView)
	, width_(ownerSwapChain->GetBackbufferWidth())
	, height_(ownerSwapChain->GetBackbufferHeight())
{

}

GfxRenderSurfaceVulkan::~GfxRenderSurfaceVulkan()
{

}

}
