#include "Graphics/RenderPipline.h"
#include "Graphics/RenderPass.h"
#include "Graphics/ComputePass.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Shader.h"
#include "Graphics/AmbientOcclusionRendering.h"
#include "Graphics/AmbientOcclusionRenderingSoftware.h"
#include "Graphics/HiZCulling.h"
#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/GfxRenderSurface.h"
#include "GfxDevice/GfxTexture.h"
#include "GfxDevice/GfxSwapChain.h"
#include "Core/EngineSettings.h"

namespace FlagGG
{

DeferredRenderPipline::DeferredRenderPipline()
	: CommonRenderPipline()
	, depthRenderPass_(new DepthRenderPass())
	, baseRenderPass_(new DeferredBaseRenderPass())
	, deferredLitRenderPass_(new DeferredLitRenderPass())
{

}

DeferredRenderPipline::~DeferredRenderPipline()
{

}

void DeferredRenderPipline::Clear()
{
	if (clusterLightPass_)
		clusterLightPass_->Clear();
	shadowRenderPass_->Clear();
	depthRenderPass_->Clear();
	alphaRenderPass_->Clear();
	waterRenderPass_->Clear();
	baseRenderPass_->Clear();
	deferredLitRenderPass_->Clear();
}

void DeferredRenderPipline::OnSolveLitBatch()
{
	for (auto* drawable : renderPiplineContext_.drawables_)
	{
		drawable->SetHasLitPass(false);
	}

	RenderPassContext context{};

	Probe* globalProbe = renderPiplineContext_.probes_.Size() ? renderPiplineContext_.probes_[0] : nullptr;

	for (auto& litRenderObjects : litRenderObjectsResult_)
	{
		for (auto* drawable : litRenderObjects.drawables_)
		{
			context.drawable_ = drawable;

			// deferred base pass
			baseRenderPass_->CollectBatch(&context);
		}

		context.light_ = litRenderObjects.light_;
		context.probe_ = globalProbe;
		deferredLitRenderPass_->CollectBatch(&context);
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

		depthRenderPass_->CollectBatch(&context);
		alphaRenderPass_->CollectBatch(&context);
		waterRenderPass_->CollectBatch(&context);
	}
}

void DeferredRenderPipline::PrepareRender()
{
	if (clusterLightPass_)
		clusterLightPass_->Dispatch(GetRenderPiplineContext());
	shadowRenderPass_->SortBatch();
	depthRenderPass_->SortBatch();
	alphaRenderPass_->SortBatch();
	waterRenderPass_->SortBatch();
	baseRenderPass_->SortBatch();
	deferredLitRenderPass_->SortBatch();
}

void DeferredRenderPipline::AllocGBuffers()
{
	if (!GBufferA_)
		GBufferA_ = new Texture2D();

	if (!GBufferB_)
		GBufferB_ = new Texture2D();

	if (!GBufferC_)
		GBufferC_ = new Texture2D();

	if (!GBufferD_)
		GBufferD_ = new Texture2D();

	if (!depthTexture_)
		depthTexture_ = new Texture2D();

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

	if (GBufferD_->GetWidth() != renderSolution.x_ || GBufferD_->GetHeight() != renderSolution.y_)
	{
		GBufferD_->SetNumLevels(1);
		GBufferD_->SetSize(renderSolution.x_, renderSolution.y_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
	}

	if (depthTexture_->GetWidth() != renderSolution.x_ || depthTexture_->GetHeight() != renderSolution.y_)
	{
		depthTexture_->SetNumLevels(1);
		depthTexture_->SetSize(renderSolution.x_, renderSolution.y_, TEXTURE_FORMAT_D24S8, TEXTURE_DEPTHSTENCIL);
	}
}

void DeferredRenderPipline::Render()
{
	RenderEngine* renderEngine = GetSubsystem<RenderEngine>();
	GfxDevice* gfxDevice = GfxDevice::GetDevice();

	bool HiZCullingEnable = GetSubsystem<EngineSettings>()->occlusionCullingType_ == OcclusionCullingType::HiZCulling;
	if (HiZCullingEnable && HiZCulling_)
	{
		HiZCulling_->ClearGeometries();

		for (auto* drawable : renderPiplineContext_.drawables_)
		{
			HiZCulling_->AddGeometry(drawable);
		}

		HiZCulling_->CalcGeometriesVisibility(renderPiplineContext_.camera_);

		HiZVisibleDrawables_.Clear();

		for (auto* drawable : renderPiplineContext_.drawables_)
		{
			if (HiZCulling_->IsGeometryVisible(drawable))
			{
				HiZVisibleDrawables_.Push(drawable);
			}
		}

		renderPiplineContext_.drawables_.Swap(HiZVisibleDrawables_);
	}

	if (!renderPiplineContext_.camera_->GetUseReflection() && renderEngine->GetDefaultTexture(TEXTURE_CLASS_SHADOWMAP))
	{
		auto* shadowMap = renderEngine->GetDefaultTexture(TEXTURE_CLASS_SHADOWMAP);
		gfxDevice->SetViewport(IntRect(0, 0, shadowMap->GetWidth(), shadowMap->GetHeight()));
		gfxDevice->SetRenderTarget(nullptr);
		gfxDevice->SetDepthStencil(shadowMap->GetRenderSurface());
		gfxDevice->Clear(CLEAR_DEPTH | CLEAR_STENCIL, Color::WHITE, 1.f, 0);

		shadowRenderPass_->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);
	}

	AllocGBuffers();

	gfxDevice->SetViewport(IntRect(0, 0, renderPiplineContext_.renderSolution_.x_, renderPiplineContext_.renderSolution_.y_));

	// Pre-z pass
	{
		gfxDevice->ResetRenderTargets();
		gfxDevice->SetDepthStencil(depthTexture_->GetRenderSurface());
		gfxDevice->Clear(CLEAR_DEPTH | CLEAR_STENCIL);

		depthRenderPass_->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);
	}

	// Render deferred base to multiple render targets
	{
		gfxDevice->ResetRenderTargets();
		gfxDevice->SetRenderTarget(0u, GBufferA_->GetRenderSurface());
		gfxDevice->SetRenderTarget(1u, GBufferB_->GetRenderSurface());
		gfxDevice->SetRenderTarget(2u, GBufferC_->GetRenderSurface());
		gfxDevice->SetRenderTarget(3u, GBufferD_->GetRenderSurface());
		gfxDevice->SetDepthStencil(depthTexture_->GetRenderSurface());
		gfxDevice->Clear(CLEAR_COLOR);

		baseRenderPass_->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);
	}

	// Render ssao
	if (GetSubsystem<EngineSettings>()->renderAO_)
	{
		if (!aoRendering_)
		{
			aoRendering_ = gfxDevice->CreateAmbientOcclusionRendering();
			
			// 使用软件ssao
			if (aoRendering_ == nullptr)
			{
				aoRendering_ = new AmbientOcclusionRenderingSoftware();
			}
		}

		AmbientOcclusionInputData inputData;
		inputData.depthTexture_ = depthTexture_->GetGfxTextureRef();
		inputData.normalTexture_ = GBufferA_->GetGfxTextureRef();
		inputData.viewMatrix_ = renderPiplineContext_.camera_->GetViewMatrix();
		inputData.projectMatrix_ = renderPiplineContext_.camera_->GetProjectionMatrix();
		inputData.renderSolution_ = renderPiplineContext_.renderSolution_;

		aoRendering_->RenderAO(inputData);

		renderEngine->SetDefaultTexture(TEXTURE_CLASS_SSAO, aoRendering_->GetAmbientOcclusionTexture());
	}
	else
	{
		if (!noAOTexture_)
		{
			noAOTexture_ = new Texture2D();
			noAOTexture_->SetSize(1, 1, TEXTURE_FORMAT_R8);
			const UInt32 color32 = Color::WHITE.ToUInt();
			noAOTexture_->SetData(0, 0, 0, 1, 1, &color32);
		}

		renderEngine->SetDefaultTexture(TEXTURE_CLASS_SSAO, noAOTexture_);
	}

	bool needRT = waterRenderPass_->HasAnyBatch();

	// Render deferred lit to color render target
	{
		gfxDevice->SetTexture(0u, GBufferA_->GetGfxTextureRef());
		gfxDevice->SetTexture(1u, GBufferB_->GetGfxTextureRef());
		gfxDevice->SetTexture(2u, GBufferC_->GetGfxTextureRef());
		gfxDevice->SetTexture(3u, GBufferD_->GetGfxTextureRef());
		gfxDevice->SetTexture(4u, depthTexture_->GetGfxTextureRef());

		gfxDevice->SetSampler(0u, GBufferA_->GetGfxSamplerRef());
		gfxDevice->SetSampler(1u, GBufferB_->GetGfxSamplerRef());
		gfxDevice->SetSampler(2u, GBufferC_->GetGfxSamplerRef());
		gfxDevice->SetSampler(3u, GBufferD_->GetGfxSamplerRef());
		gfxDevice->SetSampler(4u, depthTexture_->GetGfxSamplerRef());

		gfxDevice->ResetRenderTargets();

		if (needRT)
		{
			if (!colorTexture_)
				colorTexture_ = new Texture2D();

			if (colorTexture_->GetWidth() != renderPiplineContext_.renderSolution_.x_ ||
				colorTexture_->GetHeight() != renderPiplineContext_.renderSolution_.y_)
			{
				colorTexture_->SetSize(renderPiplineContext_.renderSolution_.x_, renderPiplineContext_.renderSolution_.y_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
			}

			gfxDevice->SetRenderTarget(colorTexture_->GetRenderSurface());
			gfxDevice->SetDepthStencil(nullptr);
		}
		else
		{
			gfxDevice->SetRenderTarget(renderPiplineContext_.renderTarget_);
			gfxDevice->SetDepthStencil(nullptr);
		}

		if (clusterLightPass_)
			clusterLightPass_->BindGpuObject();

		deferredLitRenderPass_->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);
	}

	gfxDevice->ResetTextures();
	gfxDevice->ResetSamplers();
	gfxDevice->SetDepthStencil(depthTexture_->GetRenderSurface());

	waterRenderPass_->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);
	alphaRenderPass_->RenderBatch(renderPiplineContext_.camera_, renderPiplineContext_.shadowCamera_, 0u);

	if (HiZCullingEnable)
	{
		if (!HiZCulling_)
			HiZCulling_ = new HiZCulling();
		
		HiZCulling_->BuildHiZMap(depthTexture_);
	}

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
