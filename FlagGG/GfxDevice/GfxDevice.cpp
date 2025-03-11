#include "GfxDevice.h"
#include "GfxDevice/GfxRenderSurface.h"
#include "GfxDevice/GfxBuffer.h"
#include "GfxDevice/GfxTexture.h"
#include "GfxDevice/GfxShaderResourceView.h"
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

void GfxDevice::BeginFrame()
{
	// Reset to default state
	ResetRenderTargets();
	depthStencil_.Reset();
	viewport_ = Rect::ZERO;
	scissorRect_ = IntRect::ZERO;
	ClearVertexBuffer();
	indexBuffer_.Reset();
	vertexDesc_.Reset();
	instanceBuffer_.Reset();
	instanceDesc_.Reset();
	ResetComputeResources();
	vertexShader_.Reset();
	pixelShader_.Reset();
	computeShader_.Reset();
	ResetBuffers();
	ResetTextures();
	ResetSamplers();
	rasterizerState_ = RasterizerState();
	depthStencilState_ = DepthStencilState();
	rasterizerStateDirty_ = depthStencilStateDirty_ = true;
	primitiveType_ = PRIMITIVE_TRIANGLE;
	engineShaderParameters_ = nullptr;
	materialShaderParameters_ = nullptr;
}

void GfxDevice::EndFrame()
{

}

void GfxDevice::BeginPass(const char* renderPassName)
{

}

void GfxDevice::EndPass()
{

}

void GfxDevice::Clear(ClearTargetFlags flags, const Color& color/* = Color::TRANSPARENT_BLACK*/, float depth/* = 1.0f*/, unsigned stencil/* = 0*/)
{

}

void GfxDevice::SetRenderTarget(GfxRenderSurface* gfxRenderTarget)
{
	if (renderTargets_[0] != gfxRenderTarget)
	{
		renderTargets_[0] = gfxRenderTarget;
		renderTargetDirty_ = true;
	}
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
	if (slotID < MAX_RENDERTARGET_COUNT && renderTargets_[slotID] != gfxRenderTarget)
	{
		renderTargets_[slotID] = gfxRenderTarget;
		renderTargetDirty_ = true;
	}
}

void GfxDevice::SetDepthStencil(GfxRenderSurface* gfxDepthStencil)
{
	if (depthStencil_ != gfxDepthStencil)
	{
		depthStencil_ = gfxDepthStencil;
		depthStencilDirty_ = true;
	}
}

GfxRenderSurface* GfxDevice::GetRenderTarget(UInt8 slotId)
{
	return slotId < MAX_RENDERTARGET_COUNT ? renderTargets_[slotId] : nullptr;
}

GfxRenderSurface* GfxDevice::GetDepthStencil()
{
	return depthStencil_;
}

void GfxDevice::SetViewport(const Rect& viewport)
{
	if (viewport_ != viewport)
	{
		viewport_ = viewport;
		viewportDirty_ = true;
	}
}

void GfxDevice::SetViewport(const IntRect& viewport)
{
	SetViewport(Rect(Vector2(viewport.Min()), Vector2(viewport.Max())));
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
	if (vertexDesc_ != vertexDesc)
	{
		vertexDesc_ = vertexDesc;
		vertexDescDirty_ = true;
	}
}

void GfxDevice::SetInstanceBuffer(GfxBuffer* gfxInstanceBuffer)
{
	if (instanceBuffer_ != gfxInstanceBuffer)
	{
		instanceBuffer_ = gfxInstanceBuffer;
		instanceBufferDirty_ = true;
	}
}

void GfxDevice::SetInstanceDescription(VertexDescription* instanceDesc)
{
	if (instanceDesc_ != instanceDesc)
	{
		instanceDesc_ = instanceDesc;
		instanceDescDirty_ = true;
	}
}

void GfxDevice::ResetComputeResources()
{
	for (auto& it : computeBuffers_)
	{
		it.Reset();
	}

	for (auto& it : computeTextures_)
	{
		it.Reset();
	}

	computeResourcesDirty_ = true;
}

void GfxDevice::SetComputeBuffer(UInt8 slotID, GfxBuffer* gfxComputeBuffer, ComputeBindAccess bindFlags)
{
	ASSERT(slotID < MAX_GPU_UNITS_COUNT);
	if (slotID < MAX_GPU_UNITS_COUNT)
	{
		computeBuffers_[slotID] = gfxComputeBuffer;
		computeTextures_[slotID].Reset();
		computeBindFlags_[slotID] = bindFlags;
		computeResourcesDirty_ = true;
	}
}

void GfxDevice::SetComputeTexture(UInt8 slotID, GfxTexture* gfxaTexture, ComputeBindAccess bindFlags)
{
	ASSERT(slotID < MAX_GPU_UNITS_COUNT);
	if (slotID < MAX_GPU_UNITS_COUNT)
	{
		computeBuffers_[slotID].Reset();
		computeTextures_[slotID] = gfxaTexture;
		computeBindFlags_[slotID] = bindFlags;
		computeResourcesDirty_ = true;
	}
}

void GfxDevice::ResetBuffers()
{
	for (auto& it : buffers_)
	{
		it.Reset();
	}

	buffersDirty_ = true;
}

void GfxDevice::SetBuffer(UInt8 slotID, GfxBuffer* gfxBuffer)
{
	ASSERT(slotID < MAX_GPU_UNITS_COUNT);
	if (slotID < MAX_GPU_UNITS_COUNT)
	{
		buffers_[slotID] = gfxBuffer;
		buffersDirty_ = true;
	}
}

void GfxDevice::ResetTextures()
{
	for (auto& it : textures_)
	{
		it.Reset();
	}

	for (auto& textureView : textureViews_)
	{
		textureView.Reset();
	}

	texturesDirty_ = true;
}

void GfxDevice::SetTexture(UInt32 slotID, GfxTexture* gfxTexture)
{
	if (slotID < MAX_TEXTURE_CLASS)
	{
		textures_[slotID] = gfxTexture;
		texturesDirty_ = true;
	}
}

void GfxDevice::SetTextureView(UInt32 slotID, GfxShaderResourceView* gfxTextureView)
{
	if (slotID < MAX_TEXTURE_CLASS)
	{
		textureViews_[slotID] = gfxTextureView;
		texturesDirty_ = true;
	}
}

void GfxDevice::ResetSamplers()
{
	for (auto& it : samplers_)
	{
		it.Reset();
	}

	samplerDirty_ = true;
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
	if (vertexShader_ != vertexShader || pixelShader_ != pixelShader)
	{
		vertexShader_ = vertexShader;
		pixelShader_ = pixelShader;
		computeShader_ = nullptr;
		shaderDirty_ = true;
		vertexDescDirty_ = true;
	}
}

void GfxDevice::SetComputeShader(GfxShader* computeShader)
{
	if (computeShader_ != computeShader)
	{
		vertexShader_ = nullptr;
		pixelShader_ = nullptr;
		computeShader_ = computeShader;
		shaderDirty_ = true;
	}
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

void GfxDevice::SetAntialiasedLineEnable(bool enable)
{

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
	if (rasterizerState_.scissorTest_ != scissorTest)
	{
		rasterizerState_.scissorTest_ = scissorTest;
		rasterizerStateDirty_ = true;
	}

	if (scissorRect_ != rect)
	{
		scissorRect_ = rect;
		scissorRectDirty_ = true;
	}
}

void GfxDevice::Draw(UInt32 vertexStart, UInt32 vertexCount)
{

}

void GfxDevice::DrawIndexed(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart/* = 0u*/)
{

}

void GfxDevice::DrawIndexedInstanced(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart, UInt32 instanceStart, UInt32 instanceCount)
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

AmbientOcclusionRendering* GfxDevice::CreateAmbientOcclusionRendering()
{
	return nullptr;
}

}
