#include "AmbientOcclusionRenderingSoftware.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Shader.h"
#include "Graphics/ShaderHelper.h"
#include "Graphics/RenderEngine.h"
#include "GfxDevice/GfxDevice.h"
#include "Core/EngineSettings.h"

namespace FlagGG
{

AmbientOcclusionRenderingSoftware::AmbientOcclusionRenderingSoftware()
{
	const String SHADER_QUALITY = ToString("SHADER_QUALITY=%d", Int32(GetSubsystem<EngineSettings>()->AOQuality_));

	INIT_SHADER_VARIATION(SSAOVS_,      "Shader/SSAO/ScreenSpaceAmbientOcclusionVS.hlsl",      VS, {});
	INIT_SHADER_VARIATION(SSAOSetupPS_, "Shader/SSAO/ScreenSpaceAmbientOcclusionSetupPS.hlsl", PS, Vector<String>({ SHADER_QUALITY }));
	INIT_SHADER_VARIATION(SSAOStepPS_,  "Shader/SSAO/ScreenSpaceAmbientOcclusionMainPS.hlsl",  PS, Vector<String>({ SHADER_QUALITY, "USE_AO_SETUP_AS_INPUT=1", "USE_UPSAMPLE=0" }));
	INIT_SHADER_VARIATION(SSAOFinalPS_, "Shader/SSAO/ScreenSpaceAmbientOcclusionMainPS.hlsl",  PS, Vector<String>({ SHADER_QUALITY, "USE_AO_SETUP_AS_INPUT=0", "USE_UPSAMPLE=1" }));

	randomNormals_ = GetSubsystem<ResourceCache>()->GetResource<Texture2D>("Textures/RandomNormals.tga");

	shaderParameters_ = new ShaderParameters();
	shaderParameters_->AddParametersDefine<Real>("ambientOcclusionPower");
	shaderParameters_->AddParametersDefine<Real>("ambientOcclusionBias");
	shaderParameters_->AddParametersDefine<Real>("invAmbientOcclusionDistance");
	shaderParameters_->AddParametersDefine<Real>("ambientOcclusionIntensity");
	shaderParameters_->AddParametersDefine<Real>("ambientOcclusionFadeRadius");
	shaderParameters_->AddParametersDefine<Real>("ambientOcclusionFadeDistance");
	shaderParameters_->AddParametersDefine<Real>("ambientOcclusionMipThreshold");
	shaderParameters_->AddParametersDefine<Real>("ambientOcclusionMipBlend");
	shaderParameters_->AddParametersDefine<Real>("AORadiusInShader");
	shaderParameters_->AddParametersDefine<Real>("ratio");
	shaderParameters_->AddParametersDefine<Vector2>("viewportUVToRandomUV");
	shaderParameters_->AddParametersDefine<Real>("scaleFactor");
	shaderParameters_->AddParametersDefine<Real>("scaleRadiusInWorldSpace");
	shaderParameters_->AddParametersDefine<Real>("invTanHalfFov");
	shaderParameters_->AddParametersDefine<Real>("HiZStepMipLevelFactor");
	shaderParameters_->AddParametersDefine<Vector2>("temporalOffset");
	shaderParameters_->AddParametersDefine<Vector2>("SSAO_DownsampledAOInverseSize");
	shaderParameters_->AddParametersDefine<Vector2>("AOViewport_ViewportSize");
	shaderParameters_->AddParametersDefine<Vector2>("invSize");
	shaderParameters_->AddParametersDefine<Real>("thresholdInverse");
}

AmbientOcclusionRenderingSoftware::~AmbientOcclusionRenderingSoftware()
{

}

void AmbientOcclusionRenderingSoftware::SetSSAOShaderParameters(float fov, float ratio)
{
	float scaleFactor = 1.0f;
	float scaleRadiusInWorldSpace = 0.0f;
	float AORadiusInShader = settings_.ambientOcclusionRadius_ / 400.0f;
	float f = log2(scaleFactor);
	AORadiusInShader *= powf(settings_.ambientOcclusionMipScale_, f) / 4.0f;

	float invTanHalfFov = 1.0f / tanf(fov * 0.5f);
	float HiZStepMipLevelFactor = 0.4f;

	shaderParameters_->SetValue<Real>("ambientOcclusionPower", settings_.ambientOcclusionPower_);
	shaderParameters_->SetValue<Real>("ambientOcclusionBias", settings_.ambientOcclusionBias_ / 1000.0f);
	shaderParameters_->SetValue<Real>("invAmbientOcclusionDistance", 1.0 / settings_.ambientOcclusionDistance_);
	shaderParameters_->SetValue<Real>("ambientOcclusionIntensity", settings_.ambientOcclusionIntensity_);
	shaderParameters_->SetValue<Real>("ambientOcclusionFadeRadius", settings_.ambientOcclusionFadeRadius_);
	shaderParameters_->SetValue<Real>("ambientOcclusionFadeDistance", settings_.ambientOcclusionFadeDistance_);
	shaderParameters_->SetValue<Real>("ambientOcclusionMipThreshold", settings_.ambientOcclusionMipThreshold_);
	shaderParameters_->SetValue<Real>("ambientOcclusionMipBlend", settings_.ambientOcclusionMipBlend_);
	shaderParameters_->SetValue<Real>("AORadiusInShader", AORadiusInShader);
	shaderParameters_->SetValue<Real>("ratio", ratio);
	shaderParameters_->SetValue<Real>("scaleFactor", scaleFactor);
	shaderParameters_->SetValue<Real>("scaleRadiusInWorldSpace", scaleRadiusInWorldSpace);
	shaderParameters_->SetValue<Real>("invTanHalfFov", invTanHalfFov);
	shaderParameters_->SetValue<Real>("HiZStepMipLevelFactor", HiZStepMipLevelFactor);
	shaderParameters_->SetValue<Vector2>("temporalOffset", Vector2());
}

void AmbientOcclusionRenderingSoftware::RenderAO(const AmbientOcclusionInputData& inputData)
{
	AllocAOTexture(inputData.renderSolution_);

	SetSSAOShaderParameters(inputData.camera_->GetFov(), float(inputData.renderSolution_.x_) / float(inputData.renderSolution_.y_));

	auto* gfxDevice = GfxDevice::GetDevice();
	auto* renderEngine = GetSubsystem<RenderEngine>();

	gfxDevice->ResetRenderTargets();
	gfxDevice->SetDepthStencil(nullptr);
	gfxDevice->ResetTextures();
	gfxDevice->ResetSamplers();

	gfxDevice->SetMaterialShaderParameters(shaderParameters_);

	// Downsample normal
	{
		const float thresholdInversee = settings_.ambientOcclusionMipThreshold_ * ((float)downsampledNormal_->GetWidth() / aoTexture_->GetWidth());
		shaderParameters_->SetValue<Vector2>("invSize", Vector2(1.0f / downsampledNormal_->GetWidth(), 1.0f / downsampledNormal_->GetHeight()));
		shaderParameters_->SetValue<Real>("thresholdInverse", thresholdInversee);

		gfxDevice->SetRenderTarget(0, downsampledNormal_->GetRenderSurface());

		gfxDevice->SetViewport(IntRect(0, 0, downsampledNormal_->GetWidth(), downsampledNormal_->GetHeight()));

		gfxDevice->SetTexture(0, inputData.screenNormalTexture_->GetGfxTextureRef());
		gfxDevice->SetTexture(1, inputData.screenDepthTexture_->GetGfxTextureRef());

		gfxDevice->SetSampler(0, inputData.screenNormalTexture_->GetGfxSamplerRef());
		gfxDevice->SetSampler(1, inputData.screenDepthTexture_->GetGfxSamplerRef());

		gfxDevice->SetShaders(SSAOVS_->GetGfxRef(), SSAOSetupPS_->GetGfxRef());
		renderEngine->DrawQuad();
	}

	// Build downsampled AO
	{
		shaderParameters_->SetValue<Vector2>("viewportUVToRandomUV", Vector2((float)downsampledAO_->GetWidth() / randomNormals_->GetWidth(), (float)downsampledAO_->GetHeight() / randomNormals_->GetWidth()));
		shaderParameters_->SetValue<Vector2>("AOViewport_ViewportSize", Vector2(downsampledAO_->GetWidth(), downsampledAO_->GetHeight()));

		gfxDevice->SetRenderTarget(0, downsampledAO_->GetRenderSurface());

		gfxDevice->SetViewport(IntRect(0, 0, downsampledAO_->GetWidth(), downsampledAO_->GetHeight()));

		gfxDevice->SetTexture(2, randomNormals_->GetGfxTextureRef());
		gfxDevice->SetTexture(3, inputData.HiZMap_->GetGfxTextureRef());
		gfxDevice->SetTexture(4, downsampledNormal_->GetGfxTextureRef());

		gfxDevice->SetSampler(2, randomNormals_->GetGfxSamplerRef());
		gfxDevice->SetSampler(3, inputData.HiZMap_->GetGfxSamplerRef());
		gfxDevice->SetSampler(4, downsampledNormal_->GetGfxSamplerRef());

		gfxDevice->SetShaders(SSAOVS_->GetGfxRef(), SSAOStepPS_->GetGfxRef());
		renderEngine->DrawQuad();
	}

	// Build final AO
	{
		shaderParameters_->SetValue<Vector2>("viewportUVToRandomUV", Vector2((float)aoTexture_->GetWidth() / randomNormals_->GetWidth(), (float)aoTexture_->GetHeight() / randomNormals_->GetWidth()));
		shaderParameters_->SetValue<Vector2>("SSAO_DownsampledAOInverseSize", Vector2(1.0f / downsampledAO_->GetWidth(), 1.0f / downsampledAO_->GetHeight()));
		shaderParameters_->SetValue<Vector2>("AOViewport_ViewportSize", Vector2(aoTexture_->GetWidth(), aoTexture_->GetHeight()));

		gfxDevice->SetRenderTarget(0, aoTexture_->GetRenderSurface());

		gfxDevice->SetViewport(IntRect(0, 0, aoTexture_->GetWidth(), aoTexture_->GetHeight()));

		gfxDevice->SetTexture(5, downsampledAO_->GetGfxTextureRef());
		gfxDevice->SetSampler(5, downsampledAO_->GetGfxSamplerRef());

		gfxDevice->SetShaders(SSAOVS_->GetGfxRef(), SSAOFinalPS_->GetGfxRef());
		renderEngine->DrawQuad();
	}
}

Texture2D* AmbientOcclusionRenderingSoftware::GetAmbientOcclusionTexture() const
{
	return aoTexture_;
}

void AmbientOcclusionRenderingSoftware::AllocAOTexture(const IntVector2& renderSolution)
{
	const IntVector2 downsampledSolution(renderSolution.x_ / 2, renderSolution.y_ / 2);

	if (!downsampledNormal_)
	{
		downsampledNormal_ = new Texture2D();
		downsampledNormal_->SetNumLevels(1);
		downsampledNormal_->SetFilterMode(TEXTURE_FILTER_BILINEAR);
	}

	if (downsampledNormal_->GetWidth() != downsampledSolution.x_ ||
		downsampledNormal_->GetHeight() != downsampledSolution.y_)
	{
		downsampledNormal_->SetSize(downsampledSolution.x_, downsampledSolution.y_, TEXTURE_FORMAT_RGBA16F, TEXTURE_RENDERTARGET);
		downsampledNormal_->SetGpuTag("DownsampledNormal");
	}

	if (!downsampledAO_)
	{
		downsampledAO_ = new Texture2D();
		downsampledAO_->SetNumLevels(1);
		downsampledAO_->SetFilterMode(TEXTURE_FILTER_BILINEAR);
	}

	if (downsampledAO_->GetWidth() != downsampledSolution.x_ ||
		downsampledAO_->GetHeight() != downsampledSolution.y_)
	{
		downsampledAO_->SetSize(downsampledSolution.x_, downsampledSolution.y_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
		downsampledAO_->SetGpuTag("DownsampledAO");
	}

	if (!aoTexture_)
	{
		aoTexture_ = new Texture2D();
		aoTexture_->SetNumLevels(1);
		aoTexture_->SetFilterMode(TEXTURE_FILTER_NEAREST);
	}

	if (aoTexture_->GetWidth() != renderSolution.x_ ||
		aoTexture_->GetHeight() != renderSolution.y_)
	{
		aoTexture_->SetSize(renderSolution.x_, renderSolution.y_, TEXTURE_FORMAT_R8, TEXTURE_RENDERTARGET);
		downsampledAO_->SetGpuTag("AmbientOcclusion");
	}
}

}
