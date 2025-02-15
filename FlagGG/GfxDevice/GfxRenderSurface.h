//
// 抽象图形层RenderSurface
//

#pragma once

#include "GfxDevice/GfxObject.h"

namespace FlagGG
{

class GfxTexture;
class GfxSwapChain;

class FlagGG_API GfxRenderSurface : public GfxObject
{
	OBJECT_OVERRIDE(GfxRenderSurface, GfxObject);
public:
	explicit GfxRenderSurface(GfxTexture* ownerTexture, UInt32 surfaceWidth, UInt32 surfaceHeight);

	explicit GfxRenderSurface(GfxSwapChain* ownerSwapChain, UInt32 surfaceWidth, UInt32 surfaceHeight);

	~GfxRenderSurface() override;

	GfxTexture* GetOwnerTexture() const { return ownerTexture_; }

	GfxSwapChain* GetOwnerSwapChain() const { return ownerSwapChain_; }

	// 获取Surface宽度
	UInt32 GetWidth() const { return width_; }

	// 获取Surface高度
	UInt32 GetHeight() const { return height_; }

private:
	GfxTexture* ownerTexture_;

	GfxSwapChain* ownerSwapChain_;

	// Surface宽（对于有mips的Texture，Surface的宽取决于mip level）
	UInt32 width_;

	// Surface高（对于有mips的Texture，Surface的高取决于mip level）
	UInt32 height_;
};

}
