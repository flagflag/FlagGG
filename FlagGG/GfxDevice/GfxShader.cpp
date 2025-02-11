#include "GfxShader.h"
#include "Container/Sort.h"
#include "GfxDevice/Shader/PreProcessShaderInfo.h"

namespace FlagGG
{

static String HashVectorString(Vector<String> vecStr)
{
	Sort(vecStr.Begin(), vecStr.End());
	String hash;
	for (const auto& str : vecStr)
	{
		hash += str;
	}
	return hash;
}

GfxShader::GfxShader()
	: GfxObject()
{

}

GfxShader::~GfxShader()
{

}

void GfxShader::SetShaderType(ShaderType shaderType)
{
	shaderType_ = shaderType;
}

void GfxShader::SetShaderInfo(PreProcessShaderInfo* shaderInfo)
{
	shaderInfo_ = shaderInfo;
}

void GfxShader::SetDefines(const Vector<String>& defines)
{
	defines_ = defines;
	definesString_ = HashVectorString(defines_);
}

bool GfxShader::Compile()
{
	return false;
}

}
