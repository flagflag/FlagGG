#include "ScreenSpaceReflection.h"
#include "Graphics/Texture2D.h"
#include "Graphics/ShaderHelper.h"
#include "Graphics/RenderEngine.h"
#include "GfxDevice/GfxDevice.h"

namespace FlagGG
{

ScreenSpaceReflections::ScreenSpaceReflections()
{
	INIT_SHADER_VARIATION(SSRShaderVS_, "Shader/SSR/ScreenSpaceReflections.hlsl", VS, {});
	INIT_SHADER_VARIATION(SSRShaderPS_, "Shader/SSR/ScreenSpaceReflections.hlsl", PS, {});

	SSRParams_ = new ShaderParameters();
	SSRParams_->AddParametersDefine<float>("roughnessMaskScale");
	SSRParams_->AddParametersDefine<Vector4>("HiZUvFactorAndInvFactor");
}

ScreenSpaceReflections::~ScreenSpaceReflections()
{

}

void ScreenSpaceReflections::RenderSSR(const ScreenSpaceReflectionsInputData& inputData)
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
	}

	SSRParams_->SetValue<float>("roughnessMaskScale", 0.8f);
	SSRParams_->SetValue<Vector4>("HiZUvFactorAndInvFactor", Vector4(1, 1, 1, 1));

	gfxDevice->ResetRenderTargets();
	gfxDevice->SetDepthStencil(nullptr);

	gfxDevice->SetRenderTarget(0, GBufferSSR_->GetRenderSurface());

	gfxDevice->SetTexture(5u, inputData.HiZMap_->GetGfxTextureRef());
	gfxDevice->SetSampler(5u, inputData.HiZMap_->GetGfxSamplerRef());

	gfxDevice->SetShaders(SSRShaderVS_->GetGfxRef(), SSRShaderPS_->GetGfxRef());
	gfxDevice->SetMaterialShaderParameters(SSRParams_);
	renderEngine->DrawQuad(inputData.camera_);
}

}
