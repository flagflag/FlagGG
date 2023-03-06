#include "Graphics/RenderPipline.h"
#include "Graphics/RenderPass.h"
#include "Graphics/RenderEngine.h"
#include "GfxDevice/GfxDevice.h"

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
	shadowRenderPass_->Clear();
	alphaRenderPass_->Clear();
	litRenderPass_[0]->Clear();
	litRenderPass_[1]->Clear();
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

			// shadow pass
			shadowRenderPass_->CollectBatch(&context);

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
}

void ForwardRenderPipline::PrepareRender()
{
	shadowRenderPass_->SortBatch();
	alphaRenderPass_->SortBatch();
	litRenderPass_[0]->SortBatch();
	litRenderPass_[1]->SortBatch();
}

void ForwardRenderPipline::Render()
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
	
	gfxDevice->SetRenderTarget(renderPiplineContext_.renderTarget_);
	gfxDevice->SetDepthStencil(renderPiplineContext_.depthStencil_);
	gfxDevice->Clear(CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL);
	
	litRenderPass_[0]->RenderBatch(renderPiplineContext_.camera_, 0u);
	litRenderPass_[1]->RenderBatch(renderPiplineContext_.camera_, 0u);
	alphaRenderPass_->RenderBatch(renderPiplineContext_.camera_, 0u);
}

}
