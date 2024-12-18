//
// Metal图形层RenderSurface
//

#pragma once

#include "GfxDevice/GfxRenderSurface.h"
#include "mtlpp/mtlpp.hpp"

namespace FlagGG
{

class GfxTextureMetal;
class GfxSwapChainMetal;

class GfxRenderSurfaceMetal : public GfxRenderSurface
{
	OBJECT_OVERRIDE(GfxRenderSurfaceMetal, GfxRenderSurface);
public:
	explicit GfxRenderSurfaceMetal(GfxTextureMetal* ownerTexture);

	explicit GfxRenderSurfaceMetal(GfxSwapChainMetal* ownerSwapChain, mtlpp::Texture& mtlRenderTexture);

	~GfxRenderSurfaceMetal() override;

private:
	mtlpp::Texture& mtlRenderTexture_;
};

}
