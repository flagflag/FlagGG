#include "ScreenSpacePlaneReflection.h"
#include "Graphics/Texture2D.h"
#include "Graphics/ShaderHelper.h"
#include "Graphics/RenderEngine.h"
#include "GfxDevice/GfxDevice.h"

namespace FlagGG
{

ScreenSpacePlaneReflections::ScreenSpacePlaneReflections()
{
	INIT_SHADER_VARIATION(SSRShaderVS_, "Shader/SSR/ScreenSpacePlaneReflections.hlsl", VS, {});
	INIT_SHADER_VARIATION(SSRShaderPS_, "Shader/SSR/ScreenSpacePlaneReflections.hlsl", PS, {});

	SSRParams_ = new ShaderParameters();
	SSRParams_->AddParametersDefine<Vector4>("reflectionPlane");
	SSRParams_->AddParametersDefine<Vector4>("planarReflectionOrigin");
	SSRParams_->AddParametersDefine<Vector4>("planarReflectionXAxis");
	SSRParams_->AddParametersDefine<Vector4>("planarReflectionYAxis");
	SSRParams_->AddParametersDefine<Vector2>("planarReflectionScreenBound");
	SSRParams_->AddParametersDefine<Matrix3x4>("inverseTransposeMirrorMatrix");
	SSRParams_->AddParametersDefine<Vector3>("planarReflectionParameters");
	SSRParams_->AddParametersDefine<Vector2>("planarReflectionParameters2");
}

ScreenSpacePlaneReflections::~ScreenSpacePlaneReflections()
{

}

void ScreenSpacePlaneReflections::SetShaderParameters(const ScreenSpaceReflectionsInputData& inputData)
{
	{
		SSRParams_->SetValue<Vector4>("planarReflectionOrigin", Vector4::ZERO);
		SSRParams_->SetValue<Vector4>("planarReflectionXAxis", Vector4(1, 0, 0, 20000));
		SSRParams_->SetValue<Vector4>("planarReflectionYAxis", Vector4(0, 1, 0, 20000));
		SSRParams_->SetValue<Vector2>("planarReflectionScreenBound", Vector2(1e5, 1e5));
		SSRParams_->SetValue<Matrix3x4>("inverseTransposeMirrorMatrix", Matrix3x4::IDENTITY);
	}
	{
		float clampedFadeStart = Max(settings_.distanceFromPlaneFadeoutStart_, 0.0f);
		float clampedFadeEnd = Max(settings_.distanceFromPlaneFadeoutEnd_, 0.0f);
		float distanceFadeScale = 1.0f / Max(clampedFadeEnd - clampedFadeStart, 1e-5f);
		SSRParams_->SetValue<Vector3>("planarReflectionParameters", Vector3(distanceFadeScale, clampedFadeStart * distanceFadeScale, settings_.normalDistortionStrength_));
	}
	{
		const float cosFadeStart = Cos(Clamp(settings_.angleFromPlaneFadeStart_, 0.1f, 89.9f) * (float)PI / 180.0f);
		const float cosFadeEnd = Cos(Clamp(settings_.angleFromPlaneFadeEnd_, 0.1f, 89.9f) * (float)PI / 180.0f);
		const float range = 1.0f / Max(cosFadeStart - cosFadeEnd, 1e-5f);
		SSRParams_->SetValue<Vector2>("planarReflectionParameters2", Vector2(range, -cosFadeEnd * range));
	}
}

void ScreenSpacePlaneReflections::RenderSSR(const ScreenSpaceReflectionsInputData& inputData)
{
	auto* gfxDevice = GfxDevice::GetDevice();
	auto* renderEngine = GetSubsystem<RenderEngine>();

	if (!GBufferSSR_)
	{
		GBufferSSR_ = new Texture2D();
		GBufferSSR_->SetNumLevels(1);
		GBufferSSR_->SetFilterMode(TEXTURE_FILTER_NEAREST);
	}

	if (GBufferSSR_->GetWidth() != inputData.renderSolution_.x_ ||
		GBufferSSR_->GetHeight() != inputData.renderSolution_.y_)
	{
		GBufferSSR_->SetSize(inputData.renderSolution_.x_, inputData.renderSolution_.y_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
		GBufferSSR_->SetGpuTag("GBufferSSR");
	}

	gfxDevice->ResetRenderTargets();
	gfxDevice->SetDepthStencil(nullptr);

	gfxDevice->SetRenderTarget(0, GBufferSSR_->GetRenderSurface());

	gfxDevice->SetViewport(IntRect(0, 0, GBufferSSR_->GetWidth(), GBufferSSR_->GetHeight()));

	gfxDevice->SetTexture(0u, inputData.GBufferA_->GetGfxTextureRef());
	gfxDevice->SetTexture(1u, inputData.GBufferB_->GetGfxTextureRef());
	gfxDevice->SetTexture(2u, inputData.GBufferC_->GetGfxTextureRef());
	gfxDevice->SetTexture(4u, inputData.screenDepthTexture_->GetGfxTextureRef());
	gfxDevice->SetTexture(5u, inputData.HiZMap_->GetGfxTextureRef());

	gfxDevice->SetSampler(0u, inputData.GBufferA_->GetGfxSamplerRef());
	gfxDevice->SetSampler(1u, inputData.GBufferB_->GetGfxSamplerRef());
	gfxDevice->SetSampler(2u, inputData.GBufferC_->GetGfxSamplerRef());
	gfxDevice->SetSampler(4u, inputData.screenDepthTexture_->GetGfxSamplerRef());
	gfxDevice->SetSampler(5u, inputData.HiZMap_->GetGfxSamplerRef());

	gfxDevice->SetShaders(SSRShaderVS_->GetGfxRef(), SSRShaderPS_->GetGfxRef());
	gfxDevice->SetMaterialShaderParameters(SSRParams_);
	renderEngine->DrawQuad(inputData.camera_);
}

}
