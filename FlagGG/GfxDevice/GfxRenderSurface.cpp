#include "GfxRenderSurface.h"

namespace FlagGG
{

GfxRenderSurface::GfxRenderSurface(GfxTexture* ownerTexture, UInt32 surfaceWidth, UInt32 surfaceHeight)
	: GfxObject()
	, ownerTexture_(ownerTexture)
	, ownerSwapChain_(nullptr)
	, width_(surfaceWidth)
	, height_(surfaceHeight)
{

}

GfxRenderSurface::GfxRenderSurface(GfxSwapChain* ownerSwapChain, UInt32 surfaceWidth, UInt32 surfaceHeight)
	: GfxObject()
	, ownerTexture_(nullptr)
	, ownerSwapChain_(ownerSwapChain)
	, width_(surfaceWidth)
	, height_(surfaceHeight)
{

}

GfxRenderSurface::~GfxRenderSurface()
{

}

}
