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
	GfxShaderResourceView(GfxTexture* ownerTexture, UInt32 viewWidth, UInt32 viewHeight);

	GfxTexture* GetOwnerTexture() const { return ownerTexture_; }

private:
	GfxTexture* ownerTexture_;

	// View宽
	UInt32 width_;

	// View高
	UInt32 height_;
};

}
