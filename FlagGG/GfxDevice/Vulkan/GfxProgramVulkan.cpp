#include "GfxProgramVulkan.h"
#include "GfxShaderVulkan.h"
#include "Log.h"
#include "Math/HalfFloat.h"

namespace FlagGG
{

GfxProgramVulkan::GfxProgramVulkan()
{

}

GfxProgramVulkan::~GfxProgramVulkan()
{

}

void GfxProgramVulkan::Link(GfxShader* vertexShader, GfxShader* pixelShader)
{
	if (!vertexShader || !pixelShader)
	{
		FLAGGG_LOG_ERROR("Vertex shader or pixel shader is null.");
		return;
	}

	GfxShaderVulkan* metalVS = vertexShader->Cast<GfxShaderVulkan>();
	GfxShaderVulkan* metalPS = pixelShader->Cast<GfxShaderVulkan>();

	if (!metalVS || !metalPS)
	{
		FLAGGG_LOG_ERROR("Not opengl shader.");
		return;
	}

	GfxProgram::Link(vertexShader, pixelShader);
}

void GfxProgramVulkan::LinkComputeShader(GfxShader* computeShader)
{
	if (!computeShader)
	{
		FLAGGG_LOG_ERROR("Compute shader is null.");
		return;
	}

	GfxShaderVulkan* metalCS = computeShader->Cast<GfxShaderVulkan>();

	if (!metalCS)
	{
		FLAGGG_LOG_ERROR("Not opengl shader.");
		return;
	}

	// TODO:

	GfxProgram::LinkComputeShader(computeShader);
}

void GfxProgramVulkan::ApplyUniformSetter(VulkanConstantBufferVariableDesc& varDesc)
{

}

}
