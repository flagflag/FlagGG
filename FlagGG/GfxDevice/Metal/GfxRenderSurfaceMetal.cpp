#include "GfxRenderSurfaceMetal.h"
#include "GfxTextureMetal.h"
#include "GfxSwapChainMetal.h"

namespace FlagGG
{

GfxRenderSurfaceMetal::GfxRenderSurfaceMetal(GfxTextureMetal* ownerTexture, UInt32 surfaceWidth, UInt32 surfaceHeight)
	: GfxRenderSurface(ownerTexture, surfaceWidth, surfaceHeight)
	, mtlRenderTexture_(ownerTexture->GetMetalTexture())
{
}

GfxRenderSurfaceMetal::GfxRenderSurfaceMetal(GfxSwapChainMetal* ownerSwapChain, mtlpp::Texture& mtlRenderTexture, UInt32 surfaceWidth, UInt32 surfaceHeight)
	: GfxRenderSurface(ownerSwapChain, surfaceWidth, surfaceHeight)
	, mtlRenderTexture_(mtlRenderTexture)
{

}

GfxRenderSurfaceMetal::~GfxRenderSurfaceMetal()
{

}

}
