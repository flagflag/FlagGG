//
// OpenGL图形层Shader
//

#pragma once

#include "GfxDevice/GfxShader.h"
#include "GfxDevice/OpenGL/OpenGLInterface.h"

namespace FlagGG
{

class GfxShaderOpenGL : public GfxShader
{
	OBJECT_OVERRIDE(GfxShaderOpenGL, GfxShader);
public:
	explicit GfxShaderOpenGL();

	~GfxShaderOpenGL() override;

	// 编译
	void Compile() override;

	//
	GLuint GetOpenGLShader() const { return oglShader_; }

private:
	GLuint oglShader_{};
};

}
