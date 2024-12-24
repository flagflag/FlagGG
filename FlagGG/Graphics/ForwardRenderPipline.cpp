#include "Graphics/RenderPipline.h"
#include "Graphics/RenderPass.h"
#include "Graphics/ComputePass.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture2D.h"
#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/GfxRenderSurface.h"
#include "GfxDevice/GfxTexture.h"
#include "GfxDevice/GfxSwapChain.h"
#include "Core/EngineSettings.h"

namespace FlagGG
{

ForwardRenderPipline::ForwardRenderPipline()
	: CommonRenderPipline()
	, litRenderPass_{ SharedPtr<RenderPass>(new LitRenderPass()), SharedPtr<RenderPass>(new LitRenderPass()) }
	, waterRenderPass_(new WaterRenderPass())
{
	if (GetSubsystem<EngineSettings>()->clusterLightEnabled_)
		clusterLightPass_ = new ClusterLightPass();
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

	for (auto& litRenderObjects : litRenderObjectsResult_)
	{
		context.light_ = litRenderObjects.light_;
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
		if (!renderTexture_)
			renderTexture_ = new Texture2D();
		
		if (renderTexture_->GetWidth() != renderPiplineContext_.renderSolution_.x_ ||
			renderTexture_->GetHeight() != renderPiplineContext_.renderSolution_.y_)
		{
			renderTexture_->SetSize(renderPiplineContext_.renderSolution_.x_, renderPiplineContext_.renderSolution_.y_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
		}
	
		gfxDevice->SetRenderTarget(renderTexture_->GetGfxTextureRef()->GetRenderSurface());
	}
	else
	{
		gfxDevice->SetRenderTarget(renderPiplineContext_.renderTarget_);
	}
	gfxDevice->SetDepthStencil(renderPiplineContext_.depthStencil_);
	gfxDevice->Clear(CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL);
	
	litRenderPass_[0]->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);
	litRenderPass_[1]->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);
	waterRenderPass_->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);
	alphaRenderPass_->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);

	if (needRT)
	{
		if (auto* swapChain = renderPiplineContext_.renderTarget_->GetOwnerSwapChain())
		{
			swapChain->CopyData(renderTexture_->GetGfxTextureRef());
		}
		else if (auto* ownerTexture = renderPiplineContext_.renderTarget_->GetOwnerTexture())
		{
			ownerTexture->UpdateTexture(renderTexture_);
		}
	}
}

}
