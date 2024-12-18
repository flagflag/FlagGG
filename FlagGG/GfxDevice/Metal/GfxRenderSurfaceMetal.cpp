#include "GfxRenderSurfaceMetal.h"
#include "GfxTextureMetal.h"
#include "GfxSwapChainMetal.h"

namespace FlagGG
{

GfxRenderSurfaceMetal::GfxRenderSurfaceMetal(GfxTextureMetal* ownerTexture)
	: GfxRenderSurface(ownerTexture)
	, mtlRenderTexture_(ownerTexture->GetMetalTexture())
{
}

GfxRenderSurfaceMetal::GfxRenderSurfaceMetal(GfxSwapChainMetal* ownerSwapChain, mtlpp::Texture& mtlRenderTexture)
	: GfxRenderSurface(ownerSwapChain)
	, mtlRenderTexture_(mtlRenderTexture)
{

}

GfxRenderSurfaceMetal::~GfxRenderSurfaceMetal()
{

}

}
