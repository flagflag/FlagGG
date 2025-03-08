#include "Graphics/RenderPipline.h"
#include "Graphics/RenderPass.h"
#include "Graphics/ComputePass.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture2D.h"
#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/GfxRenderSurface.h"
#include "GfxDevice/GfxTexture.h"
#include "GfxDevice/GfxSwapChain.h"

namespace FlagGG
{

ForwardRenderPipline::ForwardRenderPipline()
	: CommonRenderPipline()
	, litRenderPass_{ SharedPtr<RenderPass>(new LitRenderPass()), SharedPtr<RenderPass>(new LitRenderPass()) }
{

}

ForwardRenderPipline::~ForwardRenderPipline()
{

}

void ForwardRenderPipline::Clear()
{
	if (clusterLightPass_)
		clusterLightPass_->Clear();
	shadowRenderPass_->Clear();
	alphaRenderPass_->Clear();
	litRenderPass_[0]->Clear();
	litRenderPass_[1]->Clear();
	waterRenderPass_->Clear();
}

void ForwardRenderPipline::OnSolveLitBatch()
{
	for (auto* drawable : renderPiplineContext_.drawables_)
	{
		drawable->SetHasLitPass(false);
	}

	RenderPassContext context{};

	Probe* globalProbe = renderPiplineContext_.probes_.Size() ? renderPiplineContext_.probes_[0] : nullptr;

	for (auto& litRenderObjects : litRenderObjectsResult_)
	{
		context.light_ = litRenderObjects.light_;
		context.probe_ = globalProbe;
		for (auto* drawable : litRenderObjects.drawables_)
		{
			context.drawable_ = drawable;

			// litbase
			if (!drawable->GetHasLitPass())
			{
				drawable->SetHasLitPass(true);
				litRenderPass_[0]->CollectBatch(&context);
			}
			// light
			else
			{
				litRenderPass_[1]->CollectBatch(&context);
			}
		}
	}

	context.light_ = renderPiplineContext_.shadowLight_;
	for (auto* shadowCaster : renderPiplineContext_.shadowCasters_)
	{
		context.drawable_ = shadowCaster;

		// shadow pass
		shadowRenderPass_->CollectBatch(&context);
	}

	for (auto* drawable : renderPiplineContext_.drawables_)
	{
		context.drawable_ = drawable;

		alphaRenderPass_->CollectBatch(&context);
		waterRenderPass_->CollectBatch(&context);
	}
}

void ForwardRenderPipline::PrepareRender()
{
	if (clusterLightPass_)
		clusterLightPass_->Dispatch(GetRenderPiplineContext());
	shadowRenderPass_->SortBatch();
	alphaRenderPass_->SortBatch();
	waterRenderPass_->SortBatch();
	litRenderPass_[0]->SortBatch();
	litRenderPass_[1]->SortBatch();
}

void ForwardRenderPipline::Render()
{
	RenderEngine* renderEngine = GetSubsystem<RenderEngine>();
	GfxDevice* gfxDevice = GfxDevice::GetDevice();

	if (!renderPiplineContext_.camera_->GetUseReflection() && renderEngine->GetDefaultTexture(TEXTURE_CLASS_SHADOWMAP))
	{
		gfxDevice->SetRenderTarget(nullptr);
		gfxDevice->SetDepthStencil(renderEngine->GetDefaultTexture(TEXTURE_CLASS_SHADOWMAP)->GetRenderSurface());
		gfxDevice->Clear(CLEAR_DEPTH | CLEAR_STENCIL, Color::WHITE, 1.f, 0);

		shadowRenderPass_->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);
	}

	bool needRT = waterRenderPass_->HasAnyBatch();
	if (needRT)
	{
		if (!colorTexture_)
			colorTexture_ = new Texture2D();
		if (!depthTexture_)
			depthTexture_ = new Texture2D();
		
		if (colorTexture_->GetWidth() != renderPiplineContext_.renderSolution_.x_ ||
			colorTexture_->GetHeight() != renderPiplineContext_.renderSolution_.y_)
		{
			colorTexture_->SetSize(renderPiplineContext_.renderSolution_.x_, renderPiplineContext_.renderSolution_.y_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
			depthTexture_->SetSize(renderPiplineContext_.renderSolution_.x_, renderPiplineContext_.renderSolution_.y_, TEXTURE_FORMAT_D24S8, TEXTURE_DEPTHSTENCIL);
		}
	
		gfxDevice->SetRenderTarget(colorTexture_->GetRenderSurface());
		gfxDevice->SetDepthStencil(depthTexture_->GetRenderSurface());
	}
	else
	{
		gfxDevice->SetRenderTarget(renderPiplineContext_.renderTarget_);
		gfxDevice->SetDepthStencil(renderPiplineContext_.depthStencil_);
	}
	gfxDevice->Clear(CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL, Color::BLACK, renderPiplineContext_.camera_->GetReverseZ() ? 0.0f : 1.0f);
	
	litRenderPass_[0]->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);
	litRenderPass_[1]->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);
	waterRenderPass_->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);
	alphaRenderPass_->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);

	if (needRT)
	{
		if (auto* swapChain = renderPiplineContext_.renderTarget_->GetOwnerSwapChain())
		{
			swapChain->CopyData(colorTexture_->GetGfxTextureRef());
		}
		else if (auto* ownerTexture = renderPiplineContext_.renderTarget_->GetOwnerTexture())
		{
			ownerTexture->UpdateTexture(colorTexture_);
		}
	}
}

}
