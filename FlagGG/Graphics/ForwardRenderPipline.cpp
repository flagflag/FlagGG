#include "Graphics/RenderPipline.h"
#include "Graphics/RenderPass.h"
#include "Graphics/RenderEngine.h"
#include "GfxDevice/GfxDevice.h"

namespace FlagGG
{

ForwardRenderPipline::ForwardRenderPipline()
{

}

ForwardRenderPipline::~ForwardRenderPipline()
{

}

void ForwardRenderPipline::CollectBatch()
{
	CollectLitBatch();

	CollectUnlitBatch();
}

void ForwardRenderPipline::PrepareRender()
{
	shadowRenderPass_->SortBatch();
	litRenderPass_[0]->SortBatch();
	litRenderPass_[1]->SortBatch();
	alphaRenderPass_->SortBatch();
}

void ForwardRenderPipline::Render()
{
	RenderEngine* renderEngine = RenderEngine::Instance();
	GfxDevice* gfxDevice = GfxDevice::GetDevice();

	if (renderEngine->GetDefaultTexture(TEXTURE_CLASS_SHADOWMAP))
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
