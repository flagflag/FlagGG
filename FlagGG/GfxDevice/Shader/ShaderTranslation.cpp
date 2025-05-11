// HLSLCC
#include "hlslcc.h"

#include "ShaderTranslation.h"
#if PLATFORM_WINDOWS
#include "HlslShaderCompile.h"
#endif
#include "SpirvHelper.h"
#include "Log.h"

namespace FlagGG
{

static const char* SHADER_LANGUAGE_DEFINE[] =
{
	"SHADER_LANGUAGE_GLSL=1",
	"SHADER_LANGUAGE_GLES=1",
	"SHADER_LANGUAGE_METAL=1",
	"SHADER_LANGUAGE_VULKAN=1",
};

bool CompileShader(CompileShaderLanguage compileShaderLanguage, const PreProcessShaderInfo& shaderInfo, ShaderType type, const Vector<String>& defines, String& outShaderCode)
{
#if PLATFORM_WINDOWS
	ID3DBlob* d3d11CompileCode = nullptr;
	ID3DBlob* d3d11ShaderCode = nullptr;

	Vector<String> actualDefines = defines;
	actualDefines.Push(SHADER_LANGUAGE_DEFINE[compileShaderLanguage]);

	if (!CompileShader(shaderInfo, type, actualDefines, d3d11CompileCode, d3d11ShaderCode))
	{
		FLAGGG_LOG_ERROR("Failed to compile shader.");
		return false;
	}

	if (!d3d11ShaderCode)
	{
		FLAGGG_LOG_ERROR("D3d11ShaderCode is nullptr.");
		return false;
	}
	
	GlExtensions ext;
	//ext.ARB_explicit_attrib_location = !isES;
	ext.ARB_explicit_attrib_location = 0;
	// Buggy at the moment
	ext.ARB_explicit_uniform_location = 0;//(targetGL == LANG_ES_300 || targetGL == LANG_ES_310) || targetGL >= LANG_150;
	//ext.ARB_shading_language_420pack = 1;
	ext.ARB_shading_language_420pack = 0;
	//ext.EXT_shader_framebuffer_fetch = HasFlag(input->compileFlags, kShaderCompFlags_FramebufferFetch);
	ext.EXT_shader_framebuffer_fetch = 0;

	unsigned int options = HLSLCC_FLAG_INOUT_SEMANTIC_NAMES |
		HLSLCC_FLAG_DISABLE_EXPLICIT_LOCATIONS |
		HLSLCC_FLAG_DISABLE_GLOBALS_STRUCT |
		HLSLCC_FLAG_GLOBAL_CONSTS_NEVER_IN_UBO |
		HLSLCC_FLAG_REMOVE_UNUSED_GLOBALS |
		HLSLCC_FLAG_TRANSLATE_MATRICES |
		HLSLCC_FLAG_UNIFORM_BUFFER_OBJECT |
		// HLSLCC_FLAG_MOBILE_TARGET|
		HLSLCC_FLAG_METAL_SHADOW_SAMPLER_LINEAR;

	if (type == CS)
		options |= HLSLCC_FLAG_GLES31_IMAGE_QUALIFIERS | HLSLCC_FLAG_SAMPLER_PRECISION_ENCODED_IN_NAME;
	//if (type == VS)
	//	options |= HLSLCC_FLAG_GL_AS_INSTANCE;

	GLSLCrossDependencyData dependencyData;
	dependencyData.ui32ProgramStages = PS_FLAG_VERTEX_SHADER | PS_FLAG_PIXEL_SHADER;

	HLSLccSamplerPrecisionInfo pi;

	HLSLccReflection relection;
	GLSLShader shader;

	switch (compileShaderLanguage)
	{
	case COMPILE_SHADER_VULKAN:
		options |= HLSLCC_FLAG_VULKAN_BINDINGS;
	case COMPILE_SHADER_GLSL:
		TranslateHLSLFromMem((char*)d3d11CompileCode->GetBufferPointer(), options, LANG_430, &ext, &dependencyData, pi, relection, &shader);
		break;

	case COMPILE_SHADER_GLES:
		TranslateHLSLFromMem((char*)d3d11CompileCode->GetBufferPointer(), options, LANG_ES_310, &ext, &dependencyData, pi, relection, &shader);
		break;

	case COMPILE_SHADER_METAL:
		TranslateHLSLFromMem((char*)d3d11CompileCode->GetBufferPointer(), options, LANG_METAL, &ext, &dependencyData, pi, relection, &shader);
		break;
	}

	D3D11_SAFE_RELEASE(d3d11CompileCode);
	D3D11_SAFE_RELEASE(d3d11ShaderCode);

	outShaderCode.Resize(shader.sourceCode.length());
	memcpy(&outShaderCode[0], shader.sourceCode.c_str(), shader.sourceCode.length());

#if FLAGGG_VULKAN
	// GLSL to SPIR-V
	if (compileShaderLanguage == COMPILE_SHADER_VULKAN)
	{
		String spirvShaderCode;
		if (!GetSubsystem<SpirvHelper>()->GlslToSpirv(type, outShaderCode, spirvShaderCode))
		{
			FLAGGG_LOG_ERROR("Glsl to Spir-V error.");
			return false;
		}
		outShaderCode = std::move(spirvShaderCode);
	}
#endif

	return true;
#else
	return false;
#endif
}

}
