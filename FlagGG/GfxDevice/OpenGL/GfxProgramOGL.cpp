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

	bool useProgramInterfaceQuery = !!glGetProgramInterfaceiv;

	struct VariableInfo
	{
		GLenum type;
		GLint  loc;
		GLint  num;
	} vi;

	char name[1025];

	if (useProgramInterfaceQuery)
	{
		GLint numUniforms = 0;
		GL::GetProgramInterfaceiv(oglProgram_, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

		GLenum props[] = { GL_TYPE, GL_LOCATION, GL_ARRAY_SIZE };

		for (UInt32 i = 0; i < numUniforms; ++i)
		{
			GL::GetProgramResourceiv(oglProgram_, GL_UNIFORM, i, 3, props, 3, NULL, (GLint*)&vi);
			GL::GetProgramResourceName(oglProgram_, GL_UNIFORM, i, 1024, NULL, name);

			OGLShaderUniformVariableDesc& uniformVariableDesc = uniformVariableDescs_.EmplaceBack();
			uniformVariableDesc.name_ = name;
			uniformVariableDesc.type_ = vi.type;
			uniformVariableDesc.location_ = vi.loc;
			uniformVariableDesc.arraySize_ = Max(vi.num, 1);

			switch (vi.type)
			{
			case GL_INT:
			{
				uniformVariableDesc.dataSize_ = sizeof(int) * uniformVariableDesc.arraySize_;
				uniformVariableDesc.uniformFunc_ = [](const OGLShaderUniformVariableDesc& desc, const void* data)
				{
					GL::Uniform1iv(desc.location_, desc.arraySize_, (const GLint*)data);
				};
			}
			break;

			case GL_FLOAT:
			{
				uniformVariableDesc.dataSize_ = sizeof(float) * uniformVariableDesc.arraySize_;
				uniformVariableDesc.uniformFunc_ = [](const OGLShaderUniformVariableDesc& desc, const void* data)
				{
					GL::Uniform1fv(desc.location_, desc.arraySize_, (const GLfloat*)data);
				};
			}
			break;

			case GL_FLOAT_VEC2:
			{
				uniformVariableDesc.dataSize_ = sizeof(float) * 2 * uniformVariableDesc.arraySize_;
				uniformVariableDesc.uniformFunc_ = [](const OGLShaderUniformVariableDesc& desc, const void* data)
				{
					GL::Uniform2fv(desc.location_, desc.arraySize_, (const GLfloat*)data);
				};
			}
			break;

			case GL_FLOAT_VEC3:
			{
				uniformVariableDesc.dataSize_ = sizeof(float) * 3 * uniformVariableDesc.arraySize_;
				uniformVariableDesc.uniformFunc_ = [](const OGLShaderUniformVariableDesc& desc, const void* data)
				{
					GL::Uniform3fv(desc.location_, desc.arraySize_, (const GLfloat*)data);
				};
			}
			break;

			case GL_FLOAT_VEC4:
			{
				uniformVariableDesc.dataSize_ = sizeof(float) * 4 * uniformVariableDesc.arraySize_;
				uniformVariableDesc.uniformFunc_ = [](const OGLShaderUniformVariableDesc& desc, const void* data)
				{
					GL::Uniform4fv(desc.location_, desc.arraySize_, (const GLfloat*)data);
				};
			}
			break;

			case GL_FLOAT_MAT3:
			{
				uniformVariableDesc.dataSize_ = sizeof(float) * 9 * uniformVariableDesc.arraySize_;
				uniformVariableDesc.uniformFunc_ = [](const OGLShaderUniformVariableDesc& desc, const void* data)
				{
					GL::UniformMatrix3fv(desc.location_, desc.arraySize_, false, (const GLfloat*)data);
				};
			}
			break;

			case GL_FLOAT_MAT4:
			{
				uniformVariableDesc.dataSize_ = sizeof(float) * 16 * uniformVariableDesc.arraySize_;
				uniformVariableDesc.uniformFunc_ = [](const OGLShaderUniformVariableDesc& desc, const void* data)
				{
					GL::UniformMatrix4fv(desc.location_, desc.arraySize_, false, (const GLfloat*)data);
				};
			}
			break;

			default:
			{
				uniformVariableDesc.uniformFunc_ = nullptr;
			}
			break;
			}
		}
	}
	else
	{
		GLint numUniforms = 0;
		GL::GetProgramiv(oglProgram_, GL_UNIFORM, &numUniforms);

		for (UInt32 i = 0; i < numUniforms; ++i)
		{
			GL::GetActiveUniform(oglProgram_, i, 1024, NULL, &vi.num, &vi.type, name);
			vi.loc = GL::GetUniformLocation(oglProgram_, name);

			OGLShaderUniformVariableDesc& uniformVariableDesc = uniformVariableDescs_.EmplaceBack();
			uniformVariableDesc.name_ = name;
			uniformVariableDesc.type_ = vi.type;
			uniformVariableDesc.location_ = vi.loc;
			uniformVariableDesc.arraySize_ = Max(vi.num, 1);
		}
	}
}

}
