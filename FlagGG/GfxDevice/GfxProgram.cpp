#include "GfxProgram.h"

namespace FlagGG
{

GfxProgram::GfxProgram()
	: GfxObject()
{

}

GfxProgram::~GfxProgram()
{

}

void GfxProgram::Link(GfxShader* vertexShader, GfxShader* pixelShader)
{
	vertexShader_ = vertexShader;
	pixelShader_ = pixelShader;
}

void GfxProgram::LinkComputeShader(GfxShader* computeShader)
{
	computeShader_ = computeShader;
}

}
