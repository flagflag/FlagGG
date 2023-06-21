#include "GfxProgramOGL.h"
#include "GfxShaderOGL.h"
#include "Log.h"

namespace FlagGG
{

GfxProgramOpenGL::GfxProgramOpenGL()
	: GfxProgram()
{
}

GfxProgramOpenGL::~GfxProgramOpenGL()
{
	if (oglProgram_ == 0)
	{
		GL::DeleteProgram(oglProgram_);
		oglProgram_ = 0;
	}
}

void GfxProgramOpenGL::Link(GfxShader* vertexShader, GfxShader* pixelShader)
{
	if (!vertexShader || !pixelShader)
	{
		FLAGGG_LOG_ERROR("Vertex shader or pixel shader is null.");
		return;
	}

	GfxShaderOpenGL* oglVS = vertexShader->DynamicCast<GfxShaderOpenGL>();
	GfxShaderOpenGL* oglPS = pixelShader->DynamicCast<GfxShaderOpenGL>();

	if (!oglVS || !oglPS)
	{
		FLAGGG_LOG_ERROR("Not opengl shader.");
		return;
	}

	oglProgram_ = GL::CreateProgram();
	GL::AttachShader(oglProgram_, oglVS->GetOpenGLShader());
	GL::AttachShader(oglProgram_, oglPS->GetOpenGLShader());
	GL::LinkProgram(oglProgram_);
	GLint linked = 0;
	GL::GetProgramiv(oglProgram_, GL_LINK_STATUS, &linked);
	if (linked == 0)
	{
		char infoLog[1024];
		GL::GetProgramInfoLog(oglProgram_, sizeof(infoLog), NULL, infoLog);
		GL::DeleteProgram(oglProgram_);
		oglProgram_ = 0;
		FLAGGG_LOG_ERROR("Opengl link program failed, error: %d, msg: %s.", linked, infoLog);
		return;
	}

	GfxProgram::Link(vertexShader, pixelShader);
}

void GfxProgramOpenGL::LinkComputeShader(GfxShader* computeShader)
{
	if (!computeShader)
	{
		FLAGGG_LOG_ERROR("Compute shader is null.");
		return;
	}

	GfxShaderOpenGL* oglCS = computeShader->DynamicCast<GfxShaderOpenGL>();

	if (!oglCS)
	{
		FLAGGG_LOG_ERROR("Not opengl shader.");
		return;
	}

	// TODO:

	GfxProgram::LinkComputeShader(computeShader);
}

}
