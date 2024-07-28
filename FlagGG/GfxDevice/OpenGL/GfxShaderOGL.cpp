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

void GfxShaderOpenGL::Compile()
{
	if (shaderType_ == None)
	{
		FLAGGG_LOG_WARN("Shader type is None, so initialize failed.");
		return;
	}

	GLenum oglShaderType = shaderType_ == CS ? GL_COMPUTE_SHADER : (shaderType_ == VS ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER);

	oglShader_ = GL::CreateShader(oglShaderType);
	ASSERT_MESSAGE(oglShader_ != 0, "CreateShader failed.");

	String glslShaderCode;
	if (!CompileShader(COMPILE_SHADER_GLSL, shaderSource_.Get(), shaderSourceSize_, shaderType_, defines_, glslShaderCode))
		return;

	const GLchar* oglShaderCode = glslShaderCode.CString();
	const GLint oglShaderCodeSize = glslShaderCode.Length();
	GL::ShaderSource(oglShader_, 1, &oglShaderCode, &oglShaderCodeSize);
	GL::CompileShader(oglShader_);

	GLint compiled = 0;
	GL::GetShaderiv(oglShader_, GL_COMPILE_STATUS, &compiled);
	ASSERT_MESSAGE(compiled, "Compile shader failed.");
}

}
