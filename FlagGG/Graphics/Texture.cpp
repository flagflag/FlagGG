#include "Graphics/Texture.h"
#include "Graphics/RenderEngine.h"
#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/GfxTexture.h"
#include "GfxDevice/GfxSampler.h"
#include "GfxDevice/GfxRenderSurface.h"
#include "Math/Math.h"
#include "Log.h"

namespace FlagGG
{

Texture::Texture()
	: Resource()
{
	mipsToSkip_[0] = 2;
	mipsToSkip_[1] = 1;
	mipsToSkip_[2] = 0;

	gfxTexture_ = GfxDevice::GetDevice()->CreateTexture();
	gfxSampler_ = GfxDevice::GetDevice()->CreateSampler();
}

Texture::~Texture()
{

}

void Texture::SetNumLevels(UInt32 levels)
{
	gfxTexture_->SetNumLevels(levels);
}

void Texture::SetFilterMode(TextureFilterMode filterMode)
{
	gfxSampler_->SetFilterMode(filterMode);
}

void Texture::SetAddressMode(TextureCoordinate coord, TextureAddressMode addressMode)
{
	gfxSampler_->SetAddressMode(coord, addressMode);
}

void Texture::SetComparisonFunc(ComparisonFunc comparisonFunc)
{
	gfxSampler_->SetComparisonFunc(comparisonFunc);
}

Int32 Texture::GetWidth() const
{
	return gfxTexture_->GetDesc().width_;
}

Int32 Texture::GetHeight() const
{
	return gfxTexture_->GetDesc().height_;
}

Int32 Texture::GetDepth() const
{
	return gfxTexture_->GetDesc().depth_;
}

bool Texture::IsCompressed() const
{
	return gfxTexture_->IsCompressed();
}

UInt32 Texture::GetComponents() const
{
	if (!GetWidth() || IsCompressed())
	{
		return 0;
	}

	return gfxTexture_->GetDetail().bitsPerPixel_ / 4;
}

GfxRenderSurface* Texture::GetRenderSurface() const
{
	return gfxTexture_->GetRenderSurface();
}

GfxRenderSurface* Texture::GetRenderSurface(UInt32 index) const
{
	return gfxTexture_->GetRenderSurface(index);
}

//void Texture::Initialize()
//{
//	D3D11_SAMPLER_DESC samplerDesc;
//	memset(&samplerDesc, 0, sizeof(samplerDesc));
//	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//	samplerDesc.MaxAnisotropy = 4;
//	samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
//	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
//	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
//
//	HRESULT hr = RenderEngine::Instance()->GetDevice()->CreateSamplerState(&samplerDesc, &sampler_);
//	if (hr != 0)
//	{
//		FLAGGG_LOG_ERROR("CreateSamplerState failed.");
//
//		SAFE_RELEASE(sampler_);
//
//		return;
//	}
//}

}
