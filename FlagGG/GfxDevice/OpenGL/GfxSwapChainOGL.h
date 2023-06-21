//
// OpenGL图形层SwapChain
//

#pragma once

#include "GfxDevice/GfxSwapChain.h"
#include "GfxDevice/OpenGL/GfxRenderSurfaceOGL.h"
#include "Container/Ptr.h"

namespace FlagGG
{

class GfxSwapChainOpenGL : public GfxSwapChain
{
	OBJECT_OVERRIDE(GfxSwapChainOpenGL, GfxSwapChain);
public:
	explicit GfxSwapChainOpenGL(Window* window);

	~GfxSwapChainOpenGL() override;

	// Resize
	void Resize(UInt32 width, UInt32 height) override;

	// 获取RT
	GfxRenderSurface* GetRenderTarget() override { return renderTarget_.Get(); }

	// 获取depth stencil
	GfxRenderSurface* GetDepthStencil() override { return depthStencil_.Get(); }

	// Swap buffer
	void Present() override;

private:
	SharedPtr<GfxRenderSurfaceOpenGL> renderTarget_;
	SharedPtr<GfxRenderSurfaceOpenGL> depthStencil_;
};

}
