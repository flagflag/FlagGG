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
	const String SHADER_QUALITY = ToString("SHADER_QUALITY=%d", Int32(/*GetSubsystem<EngineSettings>()->AOQuality_*/AmbientOcclusionQuality::MEDIUM));

	INIT_SHADER_VARIATION(SSAOVS_,      "Shader/SSAO/ScreenSpaceAmbientOcclusionVS.hlsl",      VS, {});
	INIT_SHADER_VARIATION(SSAOSetupPS_, "Shader/SSAO/ScreenSpaceAmbientOcclusionSetupPS.hlsl", PS, Vector<String>({ SHADER_QUALITY }));
	INIT_SHADER_VARIATION(SSAOStepPS_,  "Shader/SSAO/ScreenSpaceAmbientOcclusionMainPS.hlsl",  PS, Vector<String>({ SHADER_QUALITY, "USE_AO_SETUP_AS_INPUT=1", "USE_UPSAMPLE=0" }));
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

void AmbientOcclusionRenderingSoftware::RenderAO(const AmbientOcclusionInputData& inputData)
{
	AllocAOTexture(inputData.renderSolution_);

	auto* gfxDevice = GfxDevice::GetDevice();
	auto* renderEngine = GetSubsystem<RenderEngine>();

	gfxDevice->ResetRenderTargets();
	gfxDevice->SetDepthStencil(nullptr);
	gfxDevice->ResetTextures();
	gfxDevice->ResetSamplers();

	gfxDevice->SetMaterialShaderParameters(shaderParameters_);

	const Vector2 HiZScaleFactor((Real)downsampledNormal_->GetWidth() / inputData.HiZMap_->GetWidth(), (Real)downsampledNormal_->GetHeight() / inputData.HiZMap_->GetHeight());
	shaderParameters_->SetValue<Vector4>("HiZRemapping", Vector4(0.5 * HiZScaleFactor.x_, -0.5 * HiZScaleFactor.y_, 0.5 * HiZScaleFactor.x_, 0.5 * HiZScaleFactor.y_));

	// Downsample normal
	{
		SetSSAOShaderParameters(inputData.camera_->GetFov(), inputData.renderSolution_, IntVector2(downsampledNormal_->GetWidth(), downsampledNormal_->GetHeight()));

		const Real thresholdInverse = settings_.ambientOcclusionMipThreshold_ * ((Real)downsampledNormal_->GetWidth() / aoTexture_->GetWidth());
		shaderParameters_->SetValue<Vector2>("invSize", Vector2(1.0f / downsampledNormal_->GetWidth(), 1.0f / downsampledNormal_->GetHeight()));
		shaderParameters_->SetValue<Real>("thresholdInverse", thresholdInverse);

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
		SetSSAOShaderParameters(inputData.camera_->GetFov(), inputData.renderSolution_, IntVector2(downsampledAO_->GetWidth(), downsampledAO_->GetHeight()));

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
		SetSSAOShaderParameters(inputData.camera_->GetFov(), inputData.renderSolution_, IntVector2(aoTexture_->GetWidth(), aoTexture_->GetHeight()));

		shaderParameters_->SetValue<Vector2>("SSAO_DownsampledAOInverseSize", Vector2(1.0f / downsampledAO_->GetWidth(), 1.0f / downsampledAO_->GetHeight()));

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
		downsampledNormal_->SetFilterMode(TEXTURE_FILTER_NEAREST);
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
		downsampledAO_->SetFilterMode(TEXTURE_FILTER_NEAREST);
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
