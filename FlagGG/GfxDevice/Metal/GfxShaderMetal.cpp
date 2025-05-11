#include "GfxShaderMetal.h"
#include "GfxDeviceMetal.h"
#include "GfxDevice/Shader/ShaderTranslation.h"
#include "Log.h"

namespace FlagGG
{

GfxShaderMetal::GfxShaderMetal()
{

}

GfxShaderMetal::~GfxShaderMetal()
{

}

bool GfxShaderMetal::Compile()
{
	if (!CompileShader(COMPILE_SHADER_METAL, *shaderInfo_, shaderType_, defines_, mtlShaderCode_))
		return false;

	ns::Error error;
	mtlShader_ = GetSubsystem<GfxDeviceMetal>()->GetMetalDevice().NewLibrary(mtlShaderCode_.CString(), mtlpp::CompileOptions(), &error);
	if (error.GetCode() != 0)
	{
		FLAGGG_LOG_STD_ERROR("Compile shader failed, error log: %s\nSource code: %s", error.GetLocalizedDescription().GetCStr(), mtlShaderCode_.CString());
		ASSERT_MESSAGE(false, "Compile shader failed.");
		return false;
	}

	ns::String funcName;

	if (shaderType_ == VS)
		funcName = ns::String("VS");
	else if (shaderType_ == PS)
		funcName = ns::String("PS");
	else if (shaderType_ == CS)
		funcName = ns::String("CS");
	
	mtlFunction_ = mtlShader_.NewFunction(funcName);
	
	return true;
}

}
