//
// OpenGL图形层RenderSurface
//

#pragma once

#include "GfxDevice/GfxRenderSurface.h"
#include "GfxDevice/OpenGL/OpenGLInterface.h"
#include "Graphics/GraphicsDef.h"

namespace FlagGG
{

class GfxRenderSurfaceOpenGL : public GfxRenderSurface
{
	OBJECT_OVERRIDE(GfxRenderSurfaceOpenGL, GfxRenderSurface);
public:
	explicit GfxRenderSurfaceOpenGL(GLenum textureFormat, GLsizei width, GLsizei height);

	~GfxRenderSurfaceOpenGL() override;

	GLuint GetOGLRenderBuffer() const { return oglRenderBuffer_; }

private:
	GLuint oglRenderBuffer_;
};

}
