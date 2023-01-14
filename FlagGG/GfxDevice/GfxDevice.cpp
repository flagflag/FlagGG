#include "GfxDevice.h"
#include "GfxDevice/GfxRenderSurface.h"
#include "GfxDevice/GfxBuffer.h"
#include "GfxDevice/GfxTexture.h"
#include "GfxDevice/GfxShader.h"
#include "GfxDevice/GfxSampler.h"
#include "GfxDevice/VertexDescFactory.h"
#include "Graphics/ShaderParameter.h"

namespace FlagGG
{

GfxDevice::GfxDevice()
{
	VertexDescFactory::CreateInstance();
}

GfxDevice::~GfxDevice()
{
	VertexDescFactory::DestroyInstance();
}

void GfxDevice::Clear(ClearTargetFlags flags, const Color& color/* = Color::TRANSPARENT_BLACK*/, float depth/* = 1.0f*/, unsigned stencil/* = 0*/)
{

}

void GfxDevice::SetRenderTarget(GfxRenderSurface* gfxRenderTarget)
{
	renderTarget_ = gfxRenderTarget;
	renderTargetDirty_ = true;
}

void GfxDevice::SetDepthStencil(GfxRenderSurface* gfxDepthStencil)
{
	depthStencil_ = gfxDepthStencil;
	depthStencilDirty_ = true;
}

void GfxDevice::SetViewport(const Rect& viewport)
{
	viewport_ = viewport;
	viewportDirty_ = true;
}

void GfxDevice::SetViewport(const IntRect& viewport)
{
	viewport_.min_ = Vector2(viewport.Min());
	viewport_.max_ = Vector2(viewport.Max());
	viewportDirty_ = true;
}

void GfxDevice::SetVertexBuffer(GfxBuffer* gfxVertexBuffer)
{
	vertexBuffers_.Resize(1);
	vertexBuffers_[0] = gfxVertexBuffer;
	vertexBufferDirty_ = true;
}

void GfxDevice::ClearVertexBuffer()
{
	vertexBuffers_.Clear();
	vertexBufferDirty_ = true;
}

void GfxDevice::AddVertexBuffer(GfxBuffer* gfxVertexBuffer)
{
	vertexBuffers_.Push(SharedPtr<GfxBuffer>(gfxVertexBuffer));
	vertexBufferDirty_ = true;
}

void GfxDevice::SetIndexBuffer(GfxBuffer* gfxIndexBuffer)
{
	indexBuffer_ = gfxIndexBuffer;
	indexBufferDirty_ = true;
}

void GfxDevice::SetVertexDescription(VertexDescription* vertexDesc)
{
	vertexDesc_ = vertexDesc;
	vertexDescDirty_ = true;
}

void GfxDevice::SetTexture(UInt32 slotID, GfxTexture* gfxTexture)
{
	if (slotID < MAX_TEXTURE_CLASS)
	{
		textures_[slotID] = gfxTexture;
		texturesDirty_ = true;
	}
}

void GfxDevice::SetSampler(UInt32 slotID, GfxSampler* gfxSampler)
{
	if (slotID < MAX_TEXTURE_CLASS)
	{
		samplers_[slotID] = gfxSampler;
		samplerDirty_ = true;
	}
}

void GfxDevice::SetShaders(GfxShader* vertexShader, GfxShader* pixelShader)
{
	vertexShader_ = vertexShader;
	pixelShader_ = pixelShader;
	shaderDirty_ = true;
	vertexDescDirty_ = true;
}

void GfxDevice::SetEngineShaderParameters(ShaderParameters* engineShaderParameters)
{
	engineShaderParameters_ = engineShaderParameters;
}

void GfxDevice::SetMaterialShaderParameters(ShaderParameters* materialShaderParameters)
{
	materialShaderParameters_ = materialShaderParameters;
}

void GfxDevice::SetPrimitiveType(PrimitiveType primitiveType)
{
	primitiveType_ = primitiveType;
}

void GfxDevice::SetBlendMode(BlendMode blendMode)
{
	if (blendMode != rasterizerState_.blendMode_)
	{
		rasterizerState_.blendMode_ = blendMode;
		rasterizerStateDirty_ = true;
	}
}

void GfxDevice::SetCullMode(CullMode cullMode)
{
	if (cullMode != rasterizerState_.cullMode_)
	{
		rasterizerState_.cullMode_ = cullMode;
		rasterizerStateDirty_ = true;
	}
}

void GfxDevice::SetFillMode(FillMode fillMode)
{
	if (fillMode != rasterizerState_.fillMode_)
	{
		rasterizerState_.fillMode_ = fillMode;
		rasterizerStateDirty_ = true;
	}
}

void GfxDevice::SetDepthWrite(bool depthWrite)
{
	if (depthWrite != rasterizerState_.depthWrite_)
	{
		rasterizerState_.depthWrite_ = depthWrite;
		rasterizerStateDirty_ = true;
	}
}

void GfxDevice::SetScissorTest(bool scissorTest, const IntRect& rect)
{

}

void GfxDevice::Draw(UInt32 vertexStart, UInt32 vertexCount)
{

}

void GfxDevice::DrawIndexed(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart/* = 0u*/)
{

}

void GfxDevice::Flush()
{

}

GfxSwapChain* GfxDevice::CreateSwapChain(Window* window)
{
	return nullptr;
}

GfxTexture* GfxDevice::CreateTexture()
{
	return nullptr;
}

GfxSampler* GfxDevice::CreateSampler()
{
	return new GfxSampler();
}

GfxBuffer* GfxDevice::CreateBuffer()
{
	return nullptr;
}

GfxShader* GfxDevice::CreateShader()
{
	return nullptr;
}

GfxProgram* GfxDevice::CreateProgram()
{
	return nullptr;
}

}
