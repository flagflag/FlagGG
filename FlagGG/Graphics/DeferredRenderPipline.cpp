#include "Graphics/RenderPipline.h"
#include "Graphics/RenderPass.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture2D.h"
#include "GfxDevice/GfxDevice.h"

namespace FlagGG
{

DeferredRenderPipline::DeferredRenderPipline()
	: CommonRenderPipline()
	, baseRenderPass_(new DeferredBaseRenderPass())
	, litRenderPass_(new DeferredLitRenderPass())
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
	litRenderPass_->Clear();
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
	litRenderPass_->SortBatch();
}

void DeferredRenderPipline::AllocGBuffers()
{
	if (!GBufferA_)
		GBufferA_ = new Texture2D(nullptr);

	if (!GBufferB_)
		GBufferB_ = new Texture2D(nullptr);

	if (!GBufferC_)
		GBufferC_ = new Texture2D(nullptr);

	const IntVector2& renderSolution = renderPiplineContext_.renderSolution_;

	if (GBufferA_->GetWidth() != renderSolution.x_ || GBufferA_->GetHeight() != renderSolution.y_)
	{
		GBufferA_->SetSize(renderSolution.x_, renderSolution.y_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
	}

	if (GBufferB_->GetWidth() != renderSolution.x_ || GBufferB_->GetHeight() != renderSolution.y_)
	{
		GBufferB_->SetSize(renderSolution.x_, renderSolution.y_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
	}

	if (GBufferC_->GetWidth() != renderSolution.x_ || GBufferC_->GetHeight() != renderSolution.y_)
	{
		GBufferC_->SetSize(renderSolution.x_, renderSolution.y_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
	}
}

void DeferredRenderPipline::Render()
{
	RenderEngine* renderEngine = RenderEngine::Instance();
	GfxDevice* gfxDevice = GfxDevice::GetDevice();

	if (!renderPiplineContext_.camera_->GetUseReflection() && renderEngine->GetDefaultTexture(TEXTURE_CLASS_SHADOWMAP))
	{
		gfxDevice->SetRenderTarget(renderEngine->GetDefaultTexture(TEXTURE_CLASS_SHADOWMAP)->GetRenderSurface());
		gfxDevice->SetDepthStencil(nullptr);
		gfxDevice->Clear(CLEAR_COLOR, Color::WHITE);

		shadowRenderPass_->RenderBatch(renderPiplineContext_.camera_, 0u);
	}

	AllocGBuffers();

	// Render deferred base to multiple render targets
	{
		gfxDevice->SetRenderTarget(0u, GBufferA_->GetRenderSurface());
		gfxDevice->SetRenderTarget(1u, GBufferB_->GetRenderSurface());
		gfxDevice->SetRenderTarget(2u, GBufferC_->GetRenderSurface());
		gfxDevice->SetDepthStencil(renderPiplineContext_.depthStencil_);

		baseRenderPass_->RenderBatch(renderPiplineContext_.camera_, 0u);
	}

	// Render deferred lit to color render target
	{
		gfxDevice->SetRenderTarget(4u, renderPiplineContext_.renderTarget_);

		litRenderPass_->RenderBatch(renderPiplineContext_.camera_, 0u);
	}
}

}
