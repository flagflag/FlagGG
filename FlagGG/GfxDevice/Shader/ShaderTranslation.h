#pragma once

#include "Core/BaseTypes.h"
#include "Graphics/GraphicsDef.h"
#include "Container/Vector.h"
#include "GfxDevice/Shader/PreProcessShaderInfo.h"

namespace FlagGG
{

enum CompileShaderLanguage
{
	COMPILE_SHADER_GLSL,
	COMPILE_SHADER_GLES,
	COMPILE_SHADER_METAL,
	COMPILE_SHADER_VULKAN,
};

extern bool CompileShader(CompileShaderLanguage compileShaderLanguage, const PreProcessShaderInfo& shaderInfo, ShaderType type, const Vector<String>& defines, String& outShaderCode);

}
