#include "GpuTextureCompression.h"
#include "Graphics/Texture2D.h"
#include "Graphics/ShaderHelper.h"
#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/GfxTexture.h"

namespace FlagGG
{

#define COMPUTE_THREAD_NUM 8

GpuTextureCompression::GpuTextureCompression()
{
	INIT_SHADER_VARIATION(compressComputeShader_, "Shader/TextureCompression/ASTC_Encode.hlsl", CS, {});

	shaderParameters_ = new ShaderParameters();
	shaderParameters_->AddParametersDefine<Int32>("InTexelHeight");
	shaderParameters_->AddParametersDefine<Int32>("InTexelWidth");
	shaderParameters_->AddParametersDefine<Int32>("InGroupNumX");
}

GpuTextureCompression::~GpuTextureCompression()
{

}

SharedPtr<Texture2D> GpuTextureCompression::CompressTexture(Texture2D* uncompressTexture)
{
	auto* gfxDevice = GfxDevice::GetDevice();

	SharedPtr<Texture2D> compressTexture(new Texture2D());
	compressTexture->GetGfxTextureRef()->SetBind(TEXTURE_BIND_COMPUTE_WRITE);
	compressTexture->SetNumLevels(1);
	compressTexture->SetSize(uncompressTexture->GetWidth() / 4, uncompressTexture->GetHeight() / 4, TEXTURE_FORMAT_RGBA32U);

	const Int32 numGroupX = (uncompressTexture->GetWidth() + COMPUTE_THREAD_NUM - 1) / COMPUTE_THREAD_NUM;
	const Int32 numGroupY = (uncompressTexture->GetHeight() + COMPUTE_THREAD_NUM - 1) / COMPUTE_THREAD_NUM;

	shaderParameters_->SetValue<Int32>("InTexelWidth", uncompressTexture->GetWidth());
	shaderParameters_->SetValue<Int32>("InTexelHeight", uncompressTexture->GetHeight());
	shaderParameters_->SetValue<Int32>("InGroupNumX", numGroupX);

	gfxDevice->ResetComputeResources();
	gfxDevice->SetComputeTexture(0, uncompressTexture->GetGfxTextureRef(), COMPUTE_BIND_ACCESS_READ);
	gfxDevice->SetComputeTexture(1, compressTexture->GetGfxTextureRef(), COMPUTE_BIND_ACCESS_WRITE);
	gfxDevice->SetComputeShader(compressComputeShader_->GetGfxRef());
	gfxDevice->SetEngineShaderParameters(shaderParameters_);
	gfxDevice->Dispatch(numGroupX, numGroupY, 1);

	SharedPtr<Texture2D> astc4x4Texture(new Texture2D());
	astc4x4Texture->SetNumLevels(1);
	astc4x4Texture->SetSize(uncompressTexture->GetWidth(), uncompressTexture->GetHeight(), TEXTURE_FORMAT_ASTC4x4);
	astc4x4Texture->GetGfxTextureRef()->UpdateTexture(compressTexture->GetGfxTextureRef());

	return astc4x4Texture;
}

}
