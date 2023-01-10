#include "GfxShader.h"
#include "Container/Sort.h"

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

void GfxShader::SetShaderSource(SharedArrayPtr<char> shaderSource, UInt32 size)
{
	shaderSource_ = shaderSource;
	shaderSourceSize_ = size;
}

void GfxShader::SetDefines(const Vector<String>& defines)
{
	defines_ = defines;
	definesString_ = HashVectorString(defines_);
}

void GfxShader::Compile()
{

}

}
