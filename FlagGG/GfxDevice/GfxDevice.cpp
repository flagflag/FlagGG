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
	
}

GfxDevice::~GfxDevice()
{
	
}

void GfxDevice::Clear(ClearTargetFlags flags, const Color& color/* = Color::TRANSPARENT_BLACK*/, float depth/* = 1.0f*/, unsigned stencil/* = 0*/)
{

}

void GfxDevice::SetRenderTarget(GfxRenderSurface* gfxRenderTarget)
{
	renderTargets_[0] = gfxRenderTarget;
	renderTargetDirty_ = true;
}

void GfxDevice::ResetRenderTargets()
{
	for (UInt32 i = 0; i < MAX_RENDERTARGET_COUNT; ++i)
	{
		renderTargets_[i] = nullptr;
	}
	renderTargetDirty_ = true;
}

void GfxDevice::SetRenderTarget(UInt8 slotID, GfxRenderSurface* gfxRenderTarget)
{
	if (slotID < MAX_RENDERTARGET_COUNT)
	{
		renderTargets_[slotID] = gfxRenderTarget;
		renderTargetDirty_ = true;
	}
}

void GfxDevice::SetDepthStencil(GfxRenderSurface* gfxDepthStencil)
{
	depthStencil_ = gfxDepthStencil;
	depthStencilDirty_ = true;
}

GfxRenderSurface* GfxDevice::GetRenderTarget(UInt8 slotId)
{
	return slotId < MAX_RENDERTARGET_COUNT ? renderTargets_[slotId] : nullptr;
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

void GfxDevice::SetComputeBuffer(UInt8 slotID, GfxBuffer* gfxComputeBuffer)
{
	CRY_ASSERT(slotID < MAX_GPU_UNITS_COUNT);
	if (slotID < MAX_GPU_UNITS_COUNT)
	{
		computeBuffers_[slotID] = gfxComputeBuffer;
		computeBufferDirty_ = true;
	}
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
	computeShader_ = nullptr;
	shaderDirty_ = true;
	vertexDescDirty_ = true;
}

void GfxDevice::SetComputeShader(GfxShader* computeShader)
{
	vertexShader_ = nullptr;
	pixelShader_ = nullptr;
	computeShader_ = computeShader;
	shaderDirty_ = true;
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

void GfxDevice::SetDepthBias(float depthBias, float slopeScaledDepthBias)
{
	if (!Equals(depthBias, rasterizerState_.depthBias_))
	{
		rasterizerState_.depthBias_ = depthBias;
		rasterizerStateDirty_ = true;
	}

	if (!Equals(slopeScaledDepthBias, rasterizerState_.slopeScaledDepthBias_))
	{
		rasterizerState_.slopeScaledDepthBias_ = slopeScaledDepthBias;
		rasterizerStateDirty_ = true;
	}
}

void GfxDevice::SetDepthTestMode(ComparisonFunc depthTestMode)
{
	if (depthTestMode != depthStencilState_.depthTestMode_)
	{
		depthStencilState_.depthTestMode_ = depthTestMode;
		depthStencilStateDirty_ = true;
	}
}

void GfxDevice::SetDepthWrite(bool depthWrite)
{
	if (depthWrite != depthStencilState_.depthWrite_)
	{
		depthStencilState_.depthWrite_ = depthWrite;
		depthStencilStateDirty_ = true;
	}
}

void GfxDevice::SetStencilTest(bool stencilTest, ComparisonFunc stencilTestMode, UInt32 stencilRef, UInt32 stencilReadMask/* = F_MAX_UNSIGNED*/, UInt32 stencilWriteMask/* = F_MAX_UNSIGNED*/)
{
	if (stencilTest != depthStencilState_.stencilTest_ ||
		stencilTestMode != depthStencilState_.stencilTestMode_ ||
		stencilRef != depthStencilState_.stencilRef_ ||
		stencilReadMask != depthStencilState_.stencilReadMask_ ||
		stencilWriteMask != depthStencilState_.stencilWriteMask_)
	{
		depthStencilState_.stencilTest_ = stencilTest;
		depthStencilState_.stencilTestMode_ = stencilTestMode;
		depthStencilState_.stencilRef_ = stencilRef;
		depthStencilState_.stencilReadMask_ = stencilReadMask;
		depthStencilState_.stencilWriteMask_ = stencilWriteMask;
		depthStencilStateDirty_ = true;
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

void GfxDevice::DrawIndexedInstanced(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart, UInt32 instanceCount)
{

}

void GfxDevice::Flush()
{

}

void GfxDevice::Dispatch(UInt32 threadGroupCountX, UInt32 threadGroupCountY, UInt32 threadGroupCountZ)
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
