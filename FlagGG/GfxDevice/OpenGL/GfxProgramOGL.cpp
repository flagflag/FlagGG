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

	GfxShaderOpenGL* oglVS = vertexShader->Cast<GfxShaderOpenGL>();
	GfxShaderOpenGL* oglPS = pixelShader->Cast<GfxShaderOpenGL>();

	if (!oglVS || !oglPS)
	{
		FLAGGG_LOG_ERROR("Not opengl shader.");
		return;
	}

	oglProgram_ = GL::CreateProgram();
	GL::AttachShader(oglProgram_, oglVS->GetOGLShader());
	GL::AttachShader(oglProgram_, oglPS->GetOGLShader());
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

	ProcessReflect();
}

void GfxProgramOpenGL::LinkComputeShader(GfxShader* computeShader)
{
	if (!computeShader)
	{
		FLAGGG_LOG_ERROR("Compute shader is null.");
		return;
	}

	GfxShaderOpenGL* oglCS = computeShader->Cast<GfxShaderOpenGL>();

	if (!oglCS)
	{
		FLAGGG_LOG_ERROR("Not opengl shader.");
		return;
	}

	// TODO:

	GfxProgram::LinkComputeShader(computeShader);
}

void GfxProgramOpenGL::ProcessReflect()
{
	for (UInt32 i = 0; i < MAX_VERTEX_ELEMENT_SEMANTIC; ++i)
	{
		for (UInt32 j = 0; j < 8; ++j)
		{
			const String variableName = String("in_") + VERTEX_ELEMENT_SEM_NAME[i] + String(j);
			GLint location = GL::GetAttribLocation(oglProgram_, variableName.CString());
			if (location != -1)
			{
				vertexInputDescs_.Insert(MakePair(StringHash(VERTEX_ELEMENT_SEM_NAME[i]), OGLVertexInputDesc{ location, variableName }));
			}
		}
	}
}

}
