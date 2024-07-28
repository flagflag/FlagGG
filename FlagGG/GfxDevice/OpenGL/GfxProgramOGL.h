//
// OpenGL图形层GPU程序
//

#pragma once

#include "GfxDevice/GfxProgram.h"
#include "GfxDevice/OpenGL/OpenGLInterface.h"

namespace FlagGG
{

class GfxProgramOpenGL : public GfxProgram
{
	OBJECT_OVERRIDE(GfxProgramOpenGL, GfxProgram);
public:
	explicit GfxProgramOpenGL();

	~GfxProgramOpenGL() override;

	// link vs ps
	void Link(GfxShader* vertexShader, GfxShader* pixelShader) override;

	// link cs
	void LinkComputeShader(GfxShader* computeShader) override;


	// 
	GLuint GetOGLProgram() const { return oglProgram_; }

private:
	GLuint oglProgram_{};
};

}
