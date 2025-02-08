#include "GfxTextureMetal.h"
#include "GfxDeviceMetal.h"
#include "GfxRenderSurfaceMetal.h"
#include "Log.h"

namespace FlagGG
{

struct MetalTextureFormatInfo
{
	mtlpp::PixelFormat format_;
	mtlpp::PixelFormat srgbFormat_;
};

static const MetalTextureFormatInfo metalTextureFormat[] =
{
	{ mtlpp::PixelFormat::BC1_RGBA,              mtlpp::PixelFormat::BC1_RGBA_sRGB         }, // BC1
	{ mtlpp::PixelFormat::BC2_RGBA,              mtlpp::PixelFormat::BC2_RGBA_sRGB         }, // BC2
	{ mtlpp::PixelFormat::BC3_RGBA,              mtlpp::PixelFormat::BC3_RGBA_sRGB         }, // BC3
	{ mtlpp::PixelFormat::BC4_RUnorm,            mtlpp::PixelFormat::BC4_RSnorm            }, // BC4
	{ mtlpp::PixelFormat::BC5_RGUnorm,           mtlpp::PixelFormat::BC5_RGSnorm           }, // BC5
	{ mtlpp::PixelFormat::BC6H_RGBFloat,         mtlpp::PixelFormat::BC6H_RGBFloat         }, // BC6H
	{ mtlpp::PixelFormat::BC7_RGBAUnorm,         mtlpp::PixelFormat::BC7_RGBAUnorm_sRGB    }, // BC7
	{ mtlpp::PixelFormat::EAC_RGBA8,             mtlpp::PixelFormat::EAC_RGBA8_sRGB        }, // ETC1
	{ mtlpp::PixelFormat::ETC2_RGB8,             mtlpp::PixelFormat::ETC2_RGB8_sRGB        }, // ETC2
	{ mtlpp::PixelFormat::ETC2_RGB8A1,           mtlpp::PixelFormat::ETC2_RGB8A1_sRGB      }, // ETC2A
	{ mtlpp::PixelFormat::ETC2_RGB8A1,           mtlpp::PixelFormat::ETC2_RGB8A1_sRGB      }, // ETC2A1
	{ mtlpp::PixelFormat::Invalid,               mtlpp::PixelFormat::Invalid               }, // PTC12
	{ mtlpp::PixelFormat::Invalid,               mtlpp::PixelFormat::Invalid               }, // PTC14
	{ mtlpp::PixelFormat::Invalid,               mtlpp::PixelFormat::Invalid               }, // PTC12A
	{ mtlpp::PixelFormat::Invalid,               mtlpp::PixelFormat::Invalid               }, // PTC14A
	{ mtlpp::PixelFormat::Invalid,               mtlpp::PixelFormat::Invalid               }, // PTC22
	{ mtlpp::PixelFormat::Invalid,               mtlpp::PixelFormat::Invalid               }, // PTC24
	{ mtlpp::PixelFormat::Invalid,               mtlpp::PixelFormat::Invalid               }, // ATC
	{ mtlpp::PixelFormat::Invalid,               mtlpp::PixelFormat::Invalid               }, // ATCE
	{ mtlpp::PixelFormat::Invalid,               mtlpp::PixelFormat::Invalid               }, // ATCI
	{ mtlpp::PixelFormat::ASTC_4x4_LDR,          mtlpp::PixelFormat::ASTC_4x4_sRGB         }, // ASTC4x4
	{ mtlpp::PixelFormat::ASTC_5x5_LDR,          mtlpp::PixelFormat::ASTC_5x5_sRGB         }, // ASTC5x5
	{ mtlpp::PixelFormat::ASTC_6x6_LDR,          mtlpp::PixelFormat::ASTC_6x6_sRGB         }, // ASTC6x6
	{ mtlpp::PixelFormat::ASTC_8x5_LDR,          mtlpp::PixelFormat::ASTC_8x5_sRGB         }, // ASTC8x5
	{ mtlpp::PixelFormat::ASTC_8x6_LDR,          mtlpp::PixelFormat::ASTC_8x6_sRGB         }, // ASTC8x6
	{ mtlpp::PixelFormat::ASTC_10x5_LDR,         mtlpp::PixelFormat::ASTC_10x5_sRGB        }, // ASTC10x5
	{ mtlpp::PixelFormat::Invalid,               mtlpp::PixelFormat::Invalid               }, // Unknown
	{ mtlpp::PixelFormat::Invalid,               mtlpp::PixelFormat::Invalid               }, // R1
	{ mtlpp::PixelFormat::A8Unorm,               mtlpp::PixelFormat::Invalid               }, // A8
	{ mtlpp::PixelFormat::R8Unorm,               mtlpp::PixelFormat::R8Unorm_sRGB          }, // R8
	{ mtlpp::PixelFormat::R8Sint,                mtlpp::PixelFormat::Invalid               }, // R8I
	{ mtlpp::PixelFormat::R8Uint,                mtlpp::PixelFormat::Invalid               }, // R8U
	{ mtlpp::PixelFormat::R8Snorm,               mtlpp::PixelFormat::Invalid               }, // R8S
	{ mtlpp::PixelFormat::R16Unorm,              mtlpp::PixelFormat::Invalid               }, // R16
	{ mtlpp::PixelFormat::R16Sint,               mtlpp::PixelFormat::Invalid               }, // R16I
	{ mtlpp::PixelFormat::R16Uint,               mtlpp::PixelFormat::Invalid               }, // R16U
	{ mtlpp::PixelFormat::R16Float,              mtlpp::PixelFormat::Invalid               }, // R16F
	{ mtlpp::PixelFormat::R16Snorm,              mtlpp::PixelFormat::Invalid               }, // R16S
	{ mtlpp::PixelFormat::R32Sint,               mtlpp::PixelFormat::Invalid               }, // R32I
	{ mtlpp::PixelFormat::R32Uint,               mtlpp::PixelFormat::Invalid               }, // R32U
	{ mtlpp::PixelFormat::R32Float,              mtlpp::PixelFormat::Invalid               }, // R32F
	{ mtlpp::PixelFormat::RG8Unorm,              mtlpp::PixelFormat::RG8Unorm_sRGB         }, // RG8
	{ mtlpp::PixelFormat::RG8Sint,               mtlpp::PixelFormat::Invalid               }, // RG8I
	{ mtlpp::PixelFormat::RG8Uint,               mtlpp::PixelFormat::Invalid               }, // RG8U
	{ mtlpp::PixelFormat::RG8Snorm,              mtlpp::PixelFormat::Invalid               }, // RG8S
	{ mtlpp::PixelFormat::RG16Unorm,             mtlpp::PixelFormat::Invalid               }, // RG16
	{ mtlpp::PixelFormat::RG16Sint,              mtlpp::PixelFormat::Invalid               }, // RG16I
	{ mtlpp::PixelFormat::RG16Uint,              mtlpp::PixelFormat::Invalid               }, // RG16U
	{ mtlpp::PixelFormat::RG16Float,             mtlpp::PixelFormat::Invalid               }, // RG16F
	{ mtlpp::PixelFormat::RG16Snorm,             mtlpp::PixelFormat::Invalid               }, // RG16S
	{ mtlpp::PixelFormat::R32Sint,               mtlpp::PixelFormat::Invalid               }, // RG32I
	{ mtlpp::PixelFormat::R32Uint,               mtlpp::PixelFormat::Invalid               }, // RG32U
	{ mtlpp::PixelFormat::R32Float,              mtlpp::PixelFormat::Invalid               }, // RG32F
	{ mtlpp::PixelFormat::RGBA8Unorm,            mtlpp::PixelFormat::RGBA8Unorm_sRGB       }, // RGB8
	{ mtlpp::PixelFormat::RGBA8Sint,             mtlpp::PixelFormat::Invalid               }, // RGB8I
	{ mtlpp::PixelFormat::RGBA8Uint,             mtlpp::PixelFormat::Invalid               }, // RGB8U
	{ mtlpp::PixelFormat::RGBA8Snorm,            mtlpp::PixelFormat::Invalid               }, // RGB8S
	{ mtlpp::PixelFormat::RGB9E5Float,           mtlpp::PixelFormat::Invalid               }, // RGB9E5F
	{ mtlpp::PixelFormat::BGRA8Unorm,            mtlpp::PixelFormat::BGRA8Unorm_sRGB       }, // BGRA8
	{ mtlpp::PixelFormat::RGBA8Unorm,            mtlpp::PixelFormat::RGBA8Unorm_sRGB       }, // RGBA8
	{ mtlpp::PixelFormat::RGBA8Sint,             mtlpp::PixelFormat::Invalid               }, // RGBA8I
	{ mtlpp::PixelFormat::RGBA8Uint,             mtlpp::PixelFormat::Invalid               }, // RGBA8U
	{ mtlpp::PixelFormat::RGBA8Snorm,            mtlpp::PixelFormat::Invalid               }, // RGBA8S
	{ mtlpp::PixelFormat::RGBA16Unorm,           mtlpp::PixelFormat::Invalid               }, // RGBA16
	{ mtlpp::PixelFormat::RGBA16Sint,            mtlpp::PixelFormat::Invalid               }, // RGBA16I
	{ mtlpp::PixelFormat::RGBA16Uint,            mtlpp::PixelFormat::Invalid               }, // RGBA16U
	{ mtlpp::PixelFormat::RGBA16Float,           mtlpp::PixelFormat::Invalid               }, // RGBA16F
	{ mtlpp::PixelFormat::RGBA16Snorm,           mtlpp::PixelFormat::Invalid               }, // RGBA16S
	{ mtlpp::PixelFormat::RGBA32Sint,            mtlpp::PixelFormat::Invalid               }, // RGBA32I
	{ mtlpp::PixelFormat::RGBA32Uint,            mtlpp::PixelFormat::Invalid               }, // RGBA32U
	{ mtlpp::PixelFormat::RGBA32Float,           mtlpp::PixelFormat::Invalid               }, // RGBA32F
	{ mtlpp::PixelFormat::B5G6R5Unorm,           mtlpp::PixelFormat::Invalid               }, // R5G6B5
	{ mtlpp::PixelFormat::ABGR4Unorm,            mtlpp::PixelFormat::Invalid               }, // RGBA4
	{ mtlpp::PixelFormat::BGR5A1Unorm,           mtlpp::PixelFormat::Invalid               }, // RGB5A1
	{ mtlpp::PixelFormat::RGB10A2Unorm,          mtlpp::PixelFormat::Invalid               }, // RGB10A2
	{ mtlpp::PixelFormat::RG11B10Float,          mtlpp::PixelFormat::Invalid               }, // RG11B10F
	{ mtlpp::PixelFormat::Invalid,               mtlpp::PixelFormat::Invalid               }, // UnknownDepth
	{ mtlpp::PixelFormat::Depth16Unorm,          mtlpp::PixelFormat::Invalid               }, // D16
	{ mtlpp::PixelFormat::Depth24Unorm_Stencil8, mtlpp::PixelFormat::Invalid               }, // D24
	{ mtlpp::PixelFormat::Depth24Unorm_Stencil8, mtlpp::PixelFormat::Invalid               }, // D24S8
	{ mtlpp::PixelFormat::Depth24Unorm_Stencil8, mtlpp::PixelFormat::Invalid               }, // D32
	{ mtlpp::PixelFormat::Depth32Float,          mtlpp::PixelFormat::Invalid               }, // D16F
	{ mtlpp::PixelFormat::Depth32Float,          mtlpp::PixelFormat::Invalid               }, // D24F
	{ mtlpp::PixelFormat::Depth32Float,          mtlpp::PixelFormat::Invalid               }, // D32F
	{ mtlpp::PixelFormat::Stencil8,              mtlpp::PixelFormat::Invalid               }, // D0S8
};

GfxTextureMetal::GfxTextureMetal()
{

}

GfxTextureMetal::~GfxTextureMetal()
{

}

void GfxTextureMetal::Apply(const void* initialDataPtr)
{
	if (!textureDesc_.width_ || !textureDesc_.height_)
	{
		FLAGGG_LOG_ERROR("Texture's width or height is invalid.");
		return;
	}

	textureDesc_.levels_ = GfxTextureUtils::CheckMaxLevels(textureDesc_.width_, textureDesc_.height_, textureDesc_.requestedLevels_);

	mtlpp::TextureDescriptor mtlTexDesc;
	if (textureDesc_.depth_ > 1)
		mtlTexDesc.SetTextureType(mtlpp::TextureType::Texture3D);
	else if (textureDesc_.isCube_)
		mtlTexDesc.SetTextureType(textureDesc_.layers_ > 1 ? mtlpp::TextureType::TextureCubeArray : mtlpp::TextureType::TextureCube);
	else
		mtlTexDesc.SetTextureType(textureDesc_.layers_ > 1 ? mtlpp::TextureType::Texture2DArray : mtlpp::TextureType::Texture2D);
	mtlTexDesc.SetPixelFormat(textureDesc_.sRGB_ ? metalTextureFormat[textureDesc_.format_].srgbFormat_ : metalTextureFormat[textureDesc_.format_].format_);
	mtlTexDesc.SetWidth(textureDesc_.width_);
	mtlTexDesc.SetHeight(textureDesc_.height_);
	mtlTexDesc.SetDepth(textureDesc_.depth_);
	mtlTexDesc.SetMipmapLevelCount(textureDesc_.levels_);
	mtlTexDesc.SetSampleCount(1); // 各向异性？
	mtlTexDesc.SetArrayLength(textureDesc_.layers_);
	UInt32 options = (UInt32)mtlpp::ResourceOptions::CpuCacheModeDefaultCache;
	mtlTexDesc.SetCpuCacheMode(mtlpp::CpuCacheMode::DefaultCache);
	switch (textureDesc_.storageMode_)
	{
	case STORAGE_MODE_SHARED:
		options |= (UInt32)mtlpp::ResourceOptions::StorageModeShared;
		mtlTexDesc.SetStorageMode(mtlpp::StorageMode::Shared);
		break;

	case STORAGE_MODE_PRIVATE:
		options |= (UInt32)mtlpp::ResourceOptions::StorageModePrivate;
		mtlTexDesc.SetStorageMode(mtlpp::StorageMode::Private);
		break;

	case STORAGE_MODE_MEMORYLESS:
		options |= (UInt32)mtlpp::ResourceOptions::StorageModeMemoryless;
		mtlTexDesc.SetStorageMode(mtlpp::StorageMode::Memoryless);
		break;
	}
	mtlTexDesc.SetResourceOptions((mtlpp::ResourceOptions)options);
	if (textureDesc_.usage_ == TEXTURE_RENDERTARGET || textureDesc_.usage_ == TEXTURE_DEPTHSTENCIL)
		mtlTexDesc.SetUsage(mtlpp::TextureUsage::RenderTarget);
	else
		mtlTexDesc.SetUsage(mtlpp::TextureUsage::ShaderRead);

	mtlTexture_ = GetSubsystem<GfxDeviceMetal>()->GetMetalDevice().NewTexture(mtlTexDesc);

	if (initialDataPtr)
		UpdateTexture(initialDataPtr);

	gfxRenderSurfaces_.Clear();
	gfxRenderSurfaces_.Push(MakeShared<GfxRenderSurfaceMetal>(this));
}

void GfxTextureMetal::UpdateTexture(const void* dataPtr)
{
	
}

void GfxTextureMetal::UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 width, UInt32 height)
{

}

void GfxTextureMetal::UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 z, UInt32 width, UInt32 height, UInt32 depth)
{

}

void GfxTextureMetal::UpdateTexture(GfxTexture* gfxTexture)
{

}

GfxRenderSurface* GfxTextureMetal::GetRenderSurface() const
{
	return gfxRenderSurfaces_.Size() ? gfxRenderSurfaces_[0] : nullptr;
}

GfxRenderSurface* GfxTextureMetal::GetRenderSurface(UInt32 index, UInt32 level) const
{
	return index < gfxRenderSurfaces_.Size() ? gfxRenderSurfaces_[index] : nullptr;
}

mtlpp::PixelFormat GfxTextureMetal::ToMetalPixelFormat(TextureFormat format)
{
	return metalTextureFormat[format].format_;
}

}
