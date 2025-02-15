#include "GfxRenderSurfaceOGL.h"

namespace FlagGG
{

GfxRenderSurfaceOpenGL::GfxRenderSurfaceOpenGL(GfxTexture* ownerTexture, GLenum textureFormat, GLsizei width, GLsizei height)
	: GfxRenderSurface(ownerTexture, width, height)
{
	GL::GenRenderbuffers(1, &oglRenderBuffer_);
	GL::BindRenderbuffer(GL_RENDERBUFFER, oglRenderBuffer_);
	GL::RenderbufferStorage(GL_RENDERBUFFER, textureFormat, width, height);
	GL::BindRenderbuffer(GL_RENDERBUFFER, 0);
}

GfxRenderSurfaceOpenGL::GfxRenderSurfaceOpenGL(GfxSwapChain* ownerSwapChain, GLsizei width, GLsizei height)
	: GfxRenderSurface(ownerSwapChain, width, height)
{

}

GfxRenderSurfaceOpenGL::~GfxRenderSurfaceOpenGL()
{
	if (oglRenderBuffer_ != 0)
	{
		GL::DeleteRenderbuffers(1, &oglRenderBuffer_);
		oglRenderBuffer_ = 0;
	}
}

}
