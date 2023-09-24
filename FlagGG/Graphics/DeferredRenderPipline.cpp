#include "Graphics/RenderPipline.h"
#include "Graphics/RenderPass.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Shader.h"
#include "GfxDevice/GfxDevice.h"
#include "Resource/ResourceCache.h"

namespace FlagGG
{

DeferredRenderPipline::DeferredRenderPipline()
	: CommonRenderPipline()
	, cache_(GetSubsystem<ResourceCache>())
	, baseRenderPass_(new DeferredBaseRenderPass())
{

}

DeferredRenderPipline::~DeferredRenderPipline()
{

}

void DeferredRenderPipline::Clear()
{
	shadowRenderPass_->Clear();
	alphaRenderPass_->Clear();
	baseRenderPass_->Clear();
}

void DeferredRenderPipline::OnSolveLitBatch()
{
	for (auto* drawable : renderPiplineContext_.drawables_)
	{
		drawable->SetHasLitPass(false);
	}

	RenderPassContext context{};

	for (auto& litRenderObjects : litRenderObjectsResult_)
	{
		for (auto* drawable : litRenderObjects.drawables_)
		{
			context.drawable_ = drawable;

			// shadow pass
			shadowRenderPass_->CollectBatch(&context);

			// deferred base pass
			baseRenderPass_->CollectBatch(&context);
		}
	}
}

void DeferredRenderPipline::PrepareRender()
{
	shadowRenderPass_->SortBatch();
	alphaRenderPass_->SortBatch();
	baseRenderPass_->SortBatch();
}

void DeferredRenderPipline::AllocGBuffers()
{
	if (!GBufferA_)
		GBufferA_ = new Texture2D();

	if (!GBufferB_)
		GBufferB_ = new Texture2D();

	if (!GBufferC_)
		GBufferC_ = new Texture2D();

	const IntVector2& renderSolution = renderPiplineContext_.renderSolution_;

	if (GBufferA_->GetWidth() != renderSolution.x_ || GBufferA_->GetHeight() != renderSolution.y_)
	{
		GBufferA_->SetNumLevels(1);
		GBufferA_->SetSize(renderSolution.x_, renderSolution.y_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
	}

	if (GBufferB_->GetWidth() != renderSolution.x_ || GBufferB_->GetHeight() != renderSolution.y_)
	{
		GBufferB_->SetNumLevels(1);
		GBufferB_->SetSize(renderSolution.x_, renderSolution.y_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
	}

	if (GBufferC_->GetWidth() != renderSolution.x_ || GBufferC_->GetHeight() != renderSolution.y_)
	{
		GBufferC_->SetNumLevels(1);
		GBufferC_->SetSize(renderSolution.x_, renderSolution.y_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
	}
}

void DeferredRenderPipline::Render()
{
	RenderEngine* renderEngine = RenderEngine::InstancePtr();
	GfxDevice* gfxDevice = GfxDevice::GetDevice();

	if (!renderPiplineContext_.camera_->GetUseReflection() && renderEngine->GetDefaultTexture(TEXTURE_CLASS_SHADOWMAP))
	{
		gfxDevice->SetRenderTarget(nullptr);
		gfxDevice->SetDepthStencil(renderEngine->GetDefaultTexture(TEXTURE_CLASS_SHADOWMAP)->GetRenderSurface());
		gfxDevice->Clear(CLEAR_DEPTH | CLEAR_STENCIL, Color::WHITE, 1.f, 0);

		shadowRenderPass_->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);
	}

	AllocGBuffers();

	// Render deferred base to multiple render targets
	{
		gfxDevice->SetRenderTarget(0u, GBufferA_->GetRenderSurface());
		gfxDevice->SetRenderTarget(1u, GBufferB_->GetRenderSurface());
		gfxDevice->SetRenderTarget(2u, GBufferC_->GetRenderSurface());
		gfxDevice->SetDepthStencil(renderPiplineContext_.depthStencil_);
		gfxDevice->Clear(CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL);

		baseRenderPass_->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);
	}

	// Render deferred lit to color render target
	{
		gfxDevice->SetTexture(0u, GBufferA_->GetGfxTextureRef());
		gfxDevice->SetTexture(1u, GBufferB_->GetGfxTextureRef());
		gfxDevice->SetTexture(2u, GBufferC_->GetGfxTextureRef());

		gfxDevice->SetSampler(0u, GBufferA_->GetGfxSamplerRef());
		gfxDevice->SetSampler(1u, GBufferB_->GetGfxSamplerRef());
		gfxDevice->SetSampler(2u, GBufferC_->GetGfxSamplerRef());

		gfxDevice->ResetRenderTargets();
		gfxDevice->SetRenderTarget(0u, renderPiplineContext_.renderTarget_);
		gfxDevice->SetDepthStencil(renderPiplineContext_.depthStencil_);

		if (!litVertexShader_ || !litPixelShader_)
		{
			auto shaderCode = cache_->GetResource<ShaderCode>("Shader/Deferred/LitQuad.hlsl");

			auto vs = shaderCode->GetShader(VS, {});
			litVertexShader_ = vs->GetGfxRef();

			auto ps = shaderCode->GetShader(PS, {});
 			litPixelShader_ = ps->GetGfxRef();
		}

		gfxDevice->SetShaders(litVertexShader_, litPixelShader_);

		renderEngine->DrawQuad(renderPiplineContext_.camera_);
	}
}

}
