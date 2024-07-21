#include "GfxSwapChainOGL.h"

namespace FlagGG
{

GfxSwapChainOpenGL::GfxSwapChainOpenGL(GL::IGLContext* glContext, Window* window)
	: GfxSwapChain(window)
{
	glSwapChain_ = glContext->CreateSwapChain(window);
}

GfxSwapChainOpenGL::~GfxSwapChainOpenGL()
{

}

void GfxSwapChainOpenGL::Resize(UInt32 width, UInt32 height)
{
	glSwapChain_->Resize(width, height);
}

void GfxSwapChainOpenGL::Present()
{
	glSwapChain_->Swap();
}

}
