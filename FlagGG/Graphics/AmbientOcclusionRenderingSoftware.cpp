#include "AmbientOcclusionRenderingSoftware.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Shader.h"
#include "Graphics/ShaderHelper.h"
#include "Graphics/RenderEngine.h"
#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/GfxRenderSurface.h"
#include "GfxDevice/GfxTexture.h"
#include "GfxDevice/GfxShaderResourceView.h"
#include "Core/EngineSettings.h"

namespace FlagGG
{

AmbientOcclusionRenderingSoftware::AmbientOcclusionRenderingSoftware()
{
	const String SHADER_QUALITY = ToString("SHADER_QUALITY=%d", Int32(GetSubsystem<EngineSettings>()->AOQuality_));

	INIT_SHADER_VARIATION(SSAOVS_,      "Shader/SSAO/ScreenSpaceAmbientOcclusionVS.hlsl",      VS, {});
	INIT_SHADER_VARIATION(SSAOSetupPS_, "Shader/SSAO/ScreenSpaceAmbientOcclusionSetupPS.hlsl", PS, Vector<String>({ SHADER_QUALITY }));
	INIT_SHADER_VARIATION(SSAOStepPS1_,  "Shader/SSAO/ScreenSpaceAmbientOcclusionMainPS.hlsl",  PS, Vector<String>({ SHADER_QUALITY, "USE_AO_SETUP_AS_INPUT=1", "USE_UPSAMPLE=0" }));
	INIT_SHADER_VARIATION(SSAOStepPS2_, "Shader/SSAO/ScreenSpaceAmbientOcclusionMainPS.hlsl", PS, Vector<String>({ SHADER_QUALITY, "USE_AO_SETUP_AS_INPUT=1", "USE_UPSAMPLE=1" }));
	INIT_SHADER_VARIATION(SSAOFinalPS_, "Shader/SSAO/ScreenSpaceAmbientOcclusionMainPS.hlsl",  PS, Vector<String>({ SHADER_QUALITY, "USE_AO_SETUP_AS_INPUT=0", "USE_UPSAMPLE=1" }));

	randomNormals_ = GetSubsystem<ResourceCache>()->GetResource<Texture2D>("Textures/RandomNormals.tga");
	randomNormals_->SetFilterMode(TEXTURE_FILTER_NEAREST);

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
	shaderParameters_->AddParametersDefine<Vector4>("HiZRemapping");
	shaderParameters_->AddParametersDefine<Vector2>("invSize");
	shaderParameters_->AddParametersDefine<Real>("thresholdInverse");
}

AmbientOcclusionRenderingSoftware::~AmbientOcclusionRenderingSoftware()
{

}

void AmbientOcclusionRenderingSoftware::SetSSAOShaderParameters(float fov, const IntVector2& screenSize, const IntVector2& targetSize)
{
	float scaleFactor = float(screenSize.x_) / float(targetSize.x_);
	float scaleRadiusInWorldSpace = 0.0f;
	float AORadiusInShader = settings_.ambientOcclusionRadius_ / 400.0f;
	float f = log2(scaleFactor);
	AORadiusInShader *= powf(settings_.ambientOcclusionMipScale_, f) / 4.0f;

	float ratio = float(screenSize.x_) / float(screenSize.y_);
	Vector2 viewportUVToRandomUV(Real(targetSize.x_) / randomNormals_->GetWidth(), Real(targetSize.y_) / randomNormals_->GetHeight());

	float invTanHalfFov = (1.0f / tanf(fov * (PI / 180.0f) * 0.5f));
	float HiZStepMipLevelFactor = 0.4f;
	Vector2 temporalOffset = (GetSubsystem<Context>()->GetCurrentTemporalAASampleIndex() % 8) * Vector2(2.48f, 7.52f) / (float)randomNormals_->GetWidth();

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
	shaderParameters_->SetValue<Vector2>("viewportUVToRandomUV", viewportUVToRandomUV);
	shaderParameters_->SetValue<Real>("scaleFactor", scaleFactor);
	shaderParameters_->SetValue<Real>("scaleRadiusInWorldSpace", scaleRadiusInWorldSpace);
	shaderParameters_->SetValue<Real>("invTanHalfFov", invTanHalfFov);
	shaderParameters_->SetValue<Real>("HiZStepMipLevelFactor", HiZStepMipLevelFactor);
	shaderParameters_->SetValue<Vector2>("temporalOffset", temporalOffset);
	shaderParameters_->SetValue<Vector2>("AOViewport_ViewportSize", Vector2(targetSize));

}

void AmbientOcclusionRenderingSoftware::SetHIZShaderParameters(Texture2D* HiZMap, const IntVector2& screenSize, UInt32 AOSamplingMipLevel)
{
	const Vector2 HiZScaleFactor((Real)screenSize.x_ / (2.0f * HiZMap->GetWidth()), (Real)screenSize.y_ / (2.0f * HiZMap->GetHeight()));
	const Vector2 SSAO_DownsampledAOInverseSize(1.0f / (downsampledAO_->GetWidth() >> AOSamplingMipLevel), 1.0f / (downsampledAO_->GetHeight() >> AOSamplingMipLevel));

	shaderParameters_->SetValue<Vector4>("HiZRemapping", Vector4(0.5 * HiZScaleFactor.x_, -0.5 * HiZScaleFactor.y_, 0.5 * HiZScaleFactor.x_, 0.5 * HiZScaleFactor.y_));
	shaderParameters_->SetValue<Vector2>("SSAO_DownsampledAOInverseSize", SSAO_DownsampledAOInverseSize);
}

void AmbientOcclusionRenderingSoftware::DownsampleNormalAndDepth(const AmbientOcclusionInputData& inputData, UInt32 mipLevel)
{
	auto* gfxDevice = GfxDevice::GetDevice();
	auto* renderEngine = GetSubsystem<RenderEngine>();
	auto* renderSurface = downsampledNormal_->GetRenderSurface(0, mipLevel);

	// 重置rt、texture绑定
	gfxDevice->ResetRenderTargets();
	gfxDevice->SetDepthStencil(nullptr);
	gfxDevice->ResetTextures();
	gfxDevice->ResetSamplers();

	SetSSAOShaderParameters(inputData.camera_->GetFov(), inputData.renderSolution_, IntVector2(renderSurface->GetWidth(), renderSurface->GetHeight()));

	const Real thresholdInverse = settings_.ambientOcclusionMipThreshold_ * ((Real)renderSurface->GetWidth() / aoTexture_->GetWidth());
	shaderParameters_->SetValue<Vector2>("invSize", Vector2(1.0f / renderSurface->GetWidth(), 1.0f / renderSurface->GetHeight()));
	shaderParameters_->SetValue<Real>("thresholdInverse", thresholdInverse);

	gfxDevice->SetRenderTarget(0, renderSurface);

	gfxDevice->SetViewport(IntRect(0, 0, renderSurface->GetWidth(), renderSurface->GetHeight()));

	gfxDevice->SetTexture(0, inputData.screenNormalTexture_->GetGfxTextureRef());
	gfxDevice->SetTexture(1, inputData.screenDepthTexture_->GetGfxTextureRef());

	gfxDevice->SetSampler(0, inputData.screenNormalTexture_->GetGfxSamplerRef());
	gfxDevice->SetSampler(1, inputData.screenDepthTexture_->GetGfxSamplerRef());

	gfxDevice->SetShaders(SSAOVS_->GetGfxRef(), SSAOSetupPS_->GetGfxRef());
	renderEngine->DrawQuad();
}

void AmbientOcclusionRenderingSoftware::GenerateDownsampledAO(const AmbientOcclusionInputData& inputData, UInt32 mipLevel, bool combineDownsampledAO)
{
	auto* gfxDevice = GfxDevice::GetDevice();
	auto* renderEngine = GetSubsystem<RenderEngine>();
	auto* renderSurface = downsampledAO_->GetRenderSurface(0, mipLevel);

	// 重置rt、texture绑定
	gfxDevice->ResetRenderTargets();
	gfxDevice->SetDepthStencil(nullptr);
	gfxDevice->ResetTextures();
	gfxDevice->ResetSamplers();

	SetSSAOShaderParameters(inputData.camera_->GetFov(), inputData.renderSolution_, IntVector2(renderSurface->GetWidth(), renderSurface->GetHeight()));
	SetHIZShaderParameters(inputData.HiZMap_, inputData.renderSolution_, mipLevel + 1);

	gfxDevice->SetRenderTarget(0, renderSurface);

	gfxDevice->SetViewport(IntRect(0, 0, renderSurface->GetWidth(), renderSurface->GetHeight()));

	gfxDevice->SetTexture(2, randomNormals_->GetGfxTextureRef());
	gfxDevice->SetTexture(3, inputData.HiZMap_->GetGfxTextureRef());
	gfxDevice->SetTextureView(4, downsampledNormal_->GetGfxTextureRef()->GetSubResourceView(0, mipLevel));
	if (combineDownsampledAO)
	{
		gfxDevice->SetTextureView(5, downsampledNormal_->GetGfxTextureRef()->GetSubResourceView(0, mipLevel + 1));
		gfxDevice->SetTextureView(6, downsampledAO_->GetGfxTextureRef()->GetSubResourceView(0, mipLevel + 1));
	}

	gfxDevice->SetSampler(2, randomNormals_->GetGfxSamplerRef());
	gfxDevice->SetSampler(3, inputData.HiZMap_->GetGfxSamplerRef());
	gfxDevice->SetSampler(4, downsampledNormal_->GetGfxSamplerRef());
	if (combineDownsampledAO)
	{
		gfxDevice->SetSampler(5, downsampledNormal_->GetGfxSamplerRef());
		gfxDevice->SetSampler(6, downsampledAO_->GetGfxSamplerRef());
	}

	gfxDevice->SetShaders(SSAOVS_->GetGfxRef(), combineDownsampledAO ? SSAOStepPS2_->GetGfxRef() : SSAOStepPS1_->GetGfxRef());
	renderEngine->DrawQuad();
}

void AmbientOcclusionRenderingSoftware::GenerateScreenAO(const AmbientOcclusionInputData& inputData)
{
	auto* gfxDevice = GfxDevice::GetDevice();
	auto* renderEngine = GetSubsystem<RenderEngine>();
	auto* renderSurface = aoTexture_->GetRenderSurface();

	// 重置rt、texture绑定
	gfxDevice->ResetRenderTargets();
	gfxDevice->SetDepthStencil(nullptr);
	gfxDevice->ResetTextures();
	gfxDevice->ResetSamplers();

	SetSSAOShaderParameters(inputData.camera_->GetFov(), inputData.renderSolution_, IntVector2(aoTexture_->GetWidth(), aoTexture_->GetHeight()));
	SetHIZShaderParameters(inputData.HiZMap_, inputData.renderSolution_, 0);

	gfxDevice->SetRenderTarget(0, renderSurface);

	gfxDevice->SetViewport(IntRect(0, 0, renderSurface->GetWidth(), renderSurface->GetHeight()));

	gfxDevice->SetTexture(0, inputData.screenNormalTexture_->GetGfxTextureRef());
	gfxDevice->SetTexture(1, inputData.screenDepthTexture_->GetGfxTextureRef());
	gfxDevice->SetTexture(2, randomNormals_->GetGfxTextureRef());
	gfxDevice->SetTexture(3, inputData.HiZMap_->GetGfxTextureRef());
	gfxDevice->SetTextureView(5, downsampledNormal_->GetGfxTextureRef()->GetSubResourceView(0, 0));
	gfxDevice->SetTextureView(6, downsampledAO_->GetGfxTextureRef()->GetSubResourceView(0, 0));

	gfxDevice->SetSampler(0, inputData.screenNormalTexture_->GetGfxSamplerRef());
	gfxDevice->SetSampler(1, inputData.screenDepthTexture_->GetGfxSamplerRef());
	gfxDevice->SetSampler(2, randomNormals_->GetGfxSamplerRef());
	gfxDevice->SetSampler(3, inputData.HiZMap_->GetGfxSamplerRef());
	gfxDevice->SetSampler(5, downsampledNormal_->GetGfxSamplerRef());
	gfxDevice->SetSampler(6, downsampledAO_->GetGfxSamplerRef());

	gfxDevice->SetShaders(SSAOVS_->GetGfxRef(), SSAOFinalPS_->GetGfxRef());
	renderEngine->DrawQuad();
}

void AmbientOcclusionRenderingSoftware::RenderAO(const AmbientOcclusionInputData& inputData)
{
	AllocAOTexture(inputData.renderSolution_);

	auto* gfxDevice = GfxDevice::GetDevice();
	auto* renderEngine = GetSubsystem<RenderEngine>();

	gfxDevice->SetMaterialShaderParameters(shaderParameters_);

	bool isMultiStepPass = GetSubsystem<EngineSettings>()->AOQuality_ > AmbientOcclusionQuality::MEDIUM;

	// Downsample normal (1/2, 1/2)
	DownsampleNormalAndDepth(inputData, 0);

	if (isMultiStepPass)
	{
		// Downsample normal (1/4, 1/4)
		DownsampleNormalAndDepth(inputData, 1);

		// Build downsampled AO
		GenerateDownsampledAO(inputData, 1, false);
	}

	// Build downsampled AO
	GenerateDownsampledAO(inputData, 0, isMultiStepPass);

	// Build final AO
	GenerateScreenAO(inputData);
}

Texture2D* AmbientOcclusionRenderingSoftware::GetAmbientOcclusionTexture() const
{
	return aoTexture_;
}

void AmbientOcclusionRenderingSoftware::AllocAOTexture(const IntVector2& renderSolution)
{
	const IntVector2 halfSolution(renderSolution.x_ / 2, renderSolution.y_ / 2);
	const IntVector2 quarterSolution(renderSolution.x_ / 4, renderSolution.y_ / 4);

	if (!downsampledNormal_)
	{
		downsampledNormal_ = new Texture2D();
		downsampledNormal_->SetNumLevels(0);
		downsampledNormal_->SetFilterMode(TEXTURE_FILTER_NEAREST);
		downsampledNormal_->SetSubResourceViewEnabled(true);
	}

	if (downsampledNormal_->GetWidth() != halfSolution.x_ ||
		downsampledNormal_->GetHeight() != halfSolution.y_)
	{
		downsampledNormal_->SetSize(halfSolution.x_, halfSolution.y_, TEXTURE_FORMAT_RGBA16F, TEXTURE_RENDERTARGET);
		downsampledNormal_->SetGpuTag("DownsampledNormal");
	}

	if (!downsampledAO_)
	{
		downsampledAO_ = new Texture2D();
		downsampledAO_->SetNumLevels(0);
		downsampledAO_->SetFilterMode(TEXTURE_FILTER_NEAREST);
		downsampledAO_->SetSubResourceViewEnabled(true);
	}

	if (downsampledAO_->GetWidth() != halfSolution.x_ ||
		downsampledAO_->GetHeight() != halfSolution.y_)
	{
		downsampledAO_->SetSize(halfSolution.x_, halfSolution.y_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
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
		aoTexture_->SetGpuTag("AmbientOcclusion");
	}
}

}
