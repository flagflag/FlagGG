//
// 抽象图形层RenderSurface
//

#pragma once

#include "GfxDevice/GfxObject.h"

namespace FlagGG
{

class GfxTexture;
class GfxSwapChain;

class GfxRenderSurface : public GfxObject
{
	OBJECT_OVERRIDE(GfxRenderSurface, GfxObject);
public:
	explicit GfxRenderSurface(GfxTexture* ownerTexture);

	explicit GfxRenderSurface(GfxSwapChain* ownerSwapChain);

	~GfxRenderSurface() override;

	GfxTexture* GetOwnerTexture() const { return ownerTexture_; }

	GfxSwapChain* GetOwnerSwapChain() const { return ownerSwapChain_; }

private:
	GfxTexture* ownerTexture_;

	GfxSwapChain* ownerSwapChain_;
};

}
