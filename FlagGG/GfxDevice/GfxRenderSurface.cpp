#include "GfxRenderSurface.h"

namespace FlagGG
{

GfxRenderSurface::GfxRenderSurface(GfxTexture* ownerTexture)
	: GfxObject()
	, ownerTexture_(ownerTexture)
	, ownerSwapChain_(nullptr)
{

}

GfxRenderSurface::GfxRenderSurface(GfxSwapChain* ownerSwapChain)
	: GfxObject()
	, ownerTexture_(nullptr)
	, ownerSwapChain_(ownerSwapChain)
{

}

GfxRenderSurface::~GfxRenderSurface()
{

}

}
