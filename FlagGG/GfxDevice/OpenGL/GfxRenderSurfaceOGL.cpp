#include "GfxRenderSurfaceOGL.h"

namespace FlagGG
{

GfxRenderSurfaceOpenGL::GfxRenderSurfaceOpenGL(GLenum textureFormat, GLsizei width, GLsizei height)
	: GfxRenderSurface()
{
	GL::GenRenderbuffers(1, &oglRenderBuffer_);
	GL::BindRenderbuffer(GL_RENDERBUFFER, oglRenderBuffer_);
	GL::RenderbufferStorage(GL_RENDERBUFFER, textureFormat, width, height);
	GL::BindRenderbuffer(GL_RENDERBUFFER, 0);
}

GfxRenderSurfaceOpenGL::~GfxRenderSurfaceOpenGL()
{
	if (oglRenderBuffer_ != 0)
		GL::DeleteRenderbuffers(1, &oglRenderBuffer_);
}

}
