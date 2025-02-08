//
// 抽象图形层TextureView
//

#pragma once

#include "GfxDevice/GfxObject.h"

namespace FlagGG
{

class GfxTexture;

class GfxShaderResourceView : public GfxObject
{
	OBJECT_OVERRIDE(GfxShaderResourceView, GfxObject);
public:
	GfxShaderResourceView(GfxTexture* ownerTexture);

	GfxTexture* GetOwnerTexture() const { return ownerTexture_; }

private:
	GfxTexture* ownerTexture_;
};

}
