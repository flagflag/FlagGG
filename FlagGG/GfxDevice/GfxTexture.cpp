#include "GfxTexture.h"

namespace FlagGG
{

GfxTexture::GfxTexture()
	: GfxObject()
	, textureDesc_{ TEXTURE_FORMAT_RGBA8, 1, 1, 1, 1, 1, 0, false, 1, false, false, TEXTURE_STATIC }
{

}

GfxTexture::~GfxTexture()
{

}

void GfxTexture::SetFormat(TextureFormat format)
{
	textureDesc_.format_ = format;
}

void GfxTexture::SetNumLevels(UInt32 levels)
{
	textureDesc_.requestedLevels_ = levels;
}

void GfxTexture::SetWidth(UInt32 width)
{
	textureDesc_.width_ = width;
}

void GfxTexture::SetHeight(UInt32 height)
{
	textureDesc_.height_ = height;
}

void GfxTexture::SetDepth(UInt32 depth)
{
	textureDesc_.depth_ = depth;
}

void GfxTexture::SetLayers(UInt32 layers)
{
	textureDesc_.layers_ = layers;
}

void GfxTexture::SetCube(bool cube)
{
	textureDesc_.isCube_ = cube;
}

void GfxTexture::SetMultiSample(UInt32 multiSample)
{
	textureDesc_.multiSample_ = multiSample;
}

void GfxTexture::SetAutoResolve(bool autoResolve)
{
	textureDesc_.autoResolve_ = autoResolve;
}

void GfxTexture::SetSRGB(bool sRGB)
{
	textureDesc_.sRGB_ = sRGB;
}

void GfxTexture::SetUsage(TextureUsage usage)
{
	textureDesc_.usage_ = usage;
}

void GfxTexture::Apply(const void* initialDataPtr)
{

}

void GfxTexture::UpdateTexture(const void* dataPtr)
{

}

void GfxTexture::UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 width, UInt32 height)
{

}

void GfxTexture::UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 z, UInt32 width, UInt32 height, UInt32 depth)
{

}

void GfxTexture::UpdateTexture(GfxTexture* gfxTexture)
{

}

GfxRenderSurface* GfxTexture::GetRenderSurface() const
{
	return nullptr;
}

GfxRenderSurface* GfxTexture::GetRenderSurface(UInt32 index) const
{
	return nullptr;
}

const TextureDetail& GfxTexture::GetDetail() const
{
	return GfxTextureUtils::GetTextureDetail(textureDesc_.format_);
}

const TextureMipInfo& GfxTexture::GetMipInfo(UInt32 level) const
{
	return GfxTextureUtils::GetTextureMipInfo(textureDesc_.format_, textureDesc_.width_, textureDesc_.height_, textureDesc_.depth_, textureDesc_.levels_, level);
}

bool GfxTexture::IsCompressed() const
{
	return GfxTextureUtils::IsCompressed(textureDesc_.format_);
}

}
