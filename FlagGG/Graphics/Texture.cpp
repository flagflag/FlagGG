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

void Texture::SetSRGB(bool srgb)
{
	gfxTexture_->SetSRGB(srgb);
}

void Texture::SetSubResourceViewEnabled(bool enable)
{
	gfxTexture_->SetSubResourceViewEnabled(enable);
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

UInt32 Texture::GetNumLevels() const
{
	return gfxTexture_->GetDesc().levels_;
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

	return gfxTexture_->GetDetail().bitsPerPixel_ / 8;
}

GfxRenderSurface* Texture::GetRenderSurface() const
{
	return gfxTexture_->GetRenderSurface();
}

GfxRenderSurface* Texture::GetRenderSurface(UInt32 index, UInt32 level) const
{
	return gfxTexture_->GetRenderSurface(index, level);
}

}
