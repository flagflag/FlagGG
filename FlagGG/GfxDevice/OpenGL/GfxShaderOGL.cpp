#include "GfxShaderOGL.h"
#include "GfxDevice/Shader/ShaderTranslation.h"
#include "Log.h"

namespace FlagGG
{

GfxShaderOpenGL::GfxShaderOpenGL()
	: GfxShader()
{

}

GfxShaderOpenGL::~GfxShaderOpenGL()
{
	if (oglShader_)
	{
		GL::DeleteShader(oglShader_);
		oglShader_ = 0;
	}
}

bool GfxShaderOpenGL::Compile()
{
	if (shaderType_ == None)
	{
		FLAGGG_LOG_WARN("Shader type is None, so initialize failed.");
		return false;
	}

	GLenum oglShaderType = shaderType_ == CS ? GL_COMPUTE_SHADER : (shaderType_ == VS ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);

	oglShader_ = GL::CreateShader(oglShaderType);
	ASSERT_MESSAGE(oglShader_ != 0, "CreateShader failed.");

	if (!CompileShader(COMPILE_SHADER_GLSL, shaderSource_.Get(), shaderSourceSize_, shaderType_, defines_, oglShaderCode_))
		return false;

	const GLchar* oglShaderCode = oglShaderCode_.CString();
	const GLint oglShaderCodeSize = oglShaderCode_.Length();
	GL::ShaderSource(oglShader_, 1, &oglShaderCode, &oglShaderCodeSize);
	GL::CompileShader(oglShader_);

	GLint compiled = 0;
	GL::GetShaderiv(oglShader_, GL_COMPILE_STATUS, &compiled);

	if (compiled == GL_FALSE)
	{
		GLchar log[1024];
		GLsizei logSize;
		GL::GetShaderInfoLog(oglShader_, sizeof(log), &logSize, log);

		FLAGGG_LOG_STD_ERROR("Compile shader failed, error log: %s\nSource code: %s", log, oglShaderCode_.CString());
		ASSERT_MESSAGE(false, "Compile shader failed.");

		GL::DeleteShader(oglShader_);
		oglShader_ = 0;
		return false;
	}

	return true;
}

}
