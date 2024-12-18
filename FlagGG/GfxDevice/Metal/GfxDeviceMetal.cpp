#include "GfxDeviceMetal.h"
#include "GfxSwapChainMetal.h"
#include "GfxTextureMetal.h"
#include "GfxBufferMetal.h"
#include "GfxShaderMetal.h"
#include "mtlpp/mtlpp.hpp"

namespace FlagGG
{

GfxDeviceMetal::GfxDeviceMetal()
{

}

GfxDeviceMetal::~GfxDeviceMetal()
{

}

void GfxDeviceMetal::Clear(ClearTargetFlags flags, const Color& color, float depth, unsigned stencil)
{

}

void GfxDeviceMetal::Draw(UInt32 vertexStart, UInt32 vertexCount)
{

}

void GfxDeviceMetal::DrawIndexed(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart)
{

}

void GfxDeviceMetal::Flush()
{

}

GfxSwapChain* GfxDeviceMetal::CreateSwapChain(Window* window)
{
	return new GfxSwapChainMetal(window);
}

GfxTexture* GfxDeviceMetal::CreateTexture()
{
	return new GfxTextureMetal();
}

GfxBuffer* GfxDeviceMetal::CreateBuffer()
{
	return new GfxBufferMetal();
}

GfxShader* GfxDeviceMetal::CreateShader()
{
	return nullptr;
}

GfxProgram* GfxDeviceMetal::CreateProgram()
{
	return nullptr;
}

mtlpp::Device& GfxDeviceMetal::GetMetalDevice()
{
	static mtlpp::Device mtlDevice = mtlpp::Device::CreateSystemDefaultDevice();
	return mtlDevice;
}

void GfxDeviceMetal::PrepareDraw()
{

}

void GfxDeviceMetal::PrepareRasterizerState()
{

}

}
