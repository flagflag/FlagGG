#include "GfxRenderSurfaceVulkan.h"
#include "GfxTextureVulkan.h"
#include "GfxSwapChainVulkan.h"

namespace FlagGG
{

GfxRenderSurfaceVulkan::GfxRenderSurfaceVulkan(GfxTextureVulkan* ownerTexture)
	: GfxRenderSurface(ownerTexture)
{
}

GfxRenderSurfaceVulkan::GfxRenderSurfaceVulkan(GfxSwapChainVulkan* ownerSwapChain)
	: GfxRenderSurface(ownerSwapChain)
{

}

GfxRenderSurfaceVulkan::~GfxRenderSurfaceVulkan()
{

}

}
