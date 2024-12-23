//
// 抽象图形层SwapChain
//

#pragma once

#include "GfxDevice/GfxObject.h"

namespace FlagGG
{

class Window;
class GfxRenderSurface;
class GfxTexture;

class FlagGG_API GfxSwapChain : public GfxObject
{
	OBJECT_OVERRIDE(GfxSwapChain, GfxObject);
public:
	explicit GfxSwapChain(Window* window);

	// Resize
	virtual void Resize(UInt32 width, UInt32 height);

	// 拷贝数据到backbuffer
	virtual void CopyData(GfxTexture* gfxTexture);

	// 获取RT
	virtual GfxRenderSurface* GetRenderTarget();

	// 获取depth stencil
	virtual GfxRenderSurface* GetDepthStencil();

	// Swap buffer
	virtual void Present();
};

}
