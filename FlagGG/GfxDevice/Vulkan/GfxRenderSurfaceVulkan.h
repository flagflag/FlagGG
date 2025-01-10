//
// Vulkan图形层RenderSurface
//

#pragma once

#include "GfxDevice/GfxRenderSurface.h"

namespace FlagGG
{

class GfxTextureVulkan;
class GfxSwapChainVulkan;

class GfxRenderSurfaceVulkan : public GfxRenderSurface
{
	OBJECT_OVERRIDE(GfxRenderSurfaceVulkan, GfxRenderSurface);
public:
	explicit GfxRenderSurfaceVulkan(GfxTextureVulkan* ownerTexture);

	explicit GfxRenderSurfaceVulkan(GfxSwapChainVulkan* ownerSwapChain);

	~GfxRenderSurfaceVulkan() override;

private:
};

}
