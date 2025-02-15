#include "GfxTextureD3D11.h"
#include "GfxDeviceD3D11.h"
#include "GfxShaderResourceViewD3D11.h"
#include "GfxRenderSurfaceD3D11.h"
#include "GfxD3D11Defines.h"
#include "Memory/Memory.h"
#include "Log.h"

#define DXGI_FORMAT_ASTC_4X4_TYPELESS     DXGI_FORMAT(133)
#define DXGI_FORMAT_ASTC_4X4_UNORM        DXGI_FORMAT(134)
#define DXGI_FORMAT_ASTC_4X4_UNORM_SRGB   DXGI_FORMAT(135)
#define DXGI_FORMAT_ASTC_5X4_TYPELESS     DXGI_FORMAT(137)
#define DXGI_FORMAT_ASTC_5X4_UNORM        DXGI_FORMAT(138)
#define DXGI_FORMAT_ASTC_5X4_UNORM_SRGB   DXGI_FORMAT(139)
#define DXGI_FORMAT_ASTC_5X5_TYPELESS     DXGI_FORMAT(141)
#define DXGI_FORMAT_ASTC_5X5_UNORM        DXGI_FORMAT(142)
#define DXGI_FORMAT_ASTC_5X5_UNORM_SRGB   DXGI_FORMAT(143)
#define DXGI_FORMAT_ASTC_6X5_TYPELESS     DXGI_FORMAT(145)
#define DXGI_FORMAT_ASTC_6X5_UNORM        DXGI_FORMAT(146)
#define DXGI_FORMAT_ASTC_6X5_UNORM_SRGB   DXGI_FORMAT(147)
#define DXGI_FORMAT_ASTC_6X6_TYPELESS     DXGI_FORMAT(149)
#define DXGI_FORMAT_ASTC_6X6_UNORM        DXGI_FORMAT(150)
#define DXGI_FORMAT_ASTC_6X6_UNORM_SRGB   DXGI_FORMAT(151)
#define DXGI_FORMAT_ASTC_8X5_TYPELESS     DXGI_FORMAT(153)
#define DXGI_FORMAT_ASTC_8X5_UNORM        DXGI_FORMAT(154)
#define DXGI_FORMAT_ASTC_8X5_UNORM_SRGB   DXGI_FORMAT(155)
#define DXGI_FORMAT_ASTC_8X6_TYPELESS     DXGI_FORMAT(157)
#define DXGI_FORMAT_ASTC_8X6_UNORM        DXGI_FORMAT(158)
#define DXGI_FORMAT_ASTC_8X6_UNORM_SRGB   DXGI_FORMAT(159)
#define DXGI_FORMAT_ASTC_8X8_TYPELESS     DXGI_FORMAT(161)
#define DXGI_FORMAT_ASTC_8X8_UNORM        DXGI_FORMAT(162)
#define DXGI_FORMAT_ASTC_8X8_UNORM_SRGB   DXGI_FORMAT(163)
#define DXGI_FORMAT_ASTC_10X5_TYPELESS    DXGI_FORMAT(165)
#define DXGI_FORMAT_ASTC_10X5_UNORM       DXGI_FORMAT(166)
#define DXGI_FORMAT_ASTC_10X5_UNORM_SRGB  DXGI_FORMAT(167)
#define DXGI_FORMAT_ASTC_10X6_TYPELESS    DXGI_FORMAT(169)
#define DXGI_FORMAT_ASTC_10X6_UNORM       DXGI_FORMAT(170)
#define DXGI_FORMAT_ASTC_10X6_UNORM_SRGB  DXGI_FORMAT(171)
#define DXGI_FORMAT_ASTC_10X8_TYPELESS    DXGI_FORMAT(173)
#define DXGI_FORMAT_ASTC_10X8_UNORM       DXGI_FORMAT(174)
#define DXGI_FORMAT_ASTC_10X8_UNORM_SRGB  DXGI_FORMAT(175)
#define DXGI_FORMAT_ASTC_10X10_TYPELESS   DXGI_FORMAT(177)
#define DXGI_FORMAT_ASTC_10X10_UNORM      DXGI_FORMAT(178)
#define DXGI_FORMAT_ASTC_10X10_UNORM_SRGB DXGI_FORMAT(179)
#define DXGI_FORMAT_ASTC_12X10_TYPELESS   DXGI_FORMAT(181)
#define DXGI_FORMAT_ASTC_12X10_UNORM      DXGI_FORMAT(182)
#define DXGI_FORMAT_ASTC_12X10_UNORM_SRGB DXGI_FORMAT(183)
#define DXGI_FORMAT_ASTC_12X12_TYPELESS   DXGI_FORMAT(185)
#define DXGI_FORMAT_ASTC_12X12_UNORM      DXGI_FORMAT(186)
#define DXGI_FORMAT_ASTC_12X12_UNORM_SRGB DXGI_FORMAT(187)

namespace FlagGG
{

struct D3D11TextureFormatInfo
{
	DXGI_FORMAT format_;
	DXGI_FORMAT srvFormat_;
	DXGI_FORMAT dsvFormat_;
	DXGI_FORMAT srgbFormat_;
};

static const D3D11TextureFormatInfo d3d11TextureFormatInfo[] =
{
	{ DXGI_FORMAT_BC1_UNORM,          DXGI_FORMAT_BC1_UNORM,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_BC1_UNORM_SRGB       }, // BC1
	{ DXGI_FORMAT_BC2_UNORM,          DXGI_FORMAT_BC2_UNORM,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_BC2_UNORM_SRGB       }, // BC2
	{ DXGI_FORMAT_BC3_UNORM,          DXGI_FORMAT_BC3_UNORM,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_BC3_UNORM_SRGB       }, // BC3
	{ DXGI_FORMAT_BC4_UNORM,          DXGI_FORMAT_BC4_UNORM,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // BC4
	{ DXGI_FORMAT_BC5_UNORM,          DXGI_FORMAT_BC5_UNORM,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // BC5
	{ DXGI_FORMAT_BC6H_SF16,          DXGI_FORMAT_BC6H_SF16,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // BC6H
	{ DXGI_FORMAT_BC7_UNORM,          DXGI_FORMAT_BC7_UNORM,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_BC7_UNORM_SRGB       }, // BC7
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // ETC1
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // ETC2
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // ETC2A
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // ETC2A1
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // PTC12
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // PTC14
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // PTC12A
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // PTC14A
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // PTC22
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // PTC24
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // ATC
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // ATCE
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // ATCI
	{ DXGI_FORMAT_ASTC_4X4_UNORM,     DXGI_FORMAT_ASTC_4X4_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_4X4_UNORM_SRGB  }, // ASTC4x4
	{ DXGI_FORMAT_ASTC_5X5_UNORM,     DXGI_FORMAT_ASTC_5X5_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_5X5_UNORM_SRGB  }, // ASTC5x5
	{ DXGI_FORMAT_ASTC_6X6_UNORM,     DXGI_FORMAT_ASTC_6X6_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_6X6_UNORM_SRGB  }, // ASTC6x6
	{ DXGI_FORMAT_ASTC_8X5_UNORM,     DXGI_FORMAT_ASTC_8X5_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_8X5_UNORM_SRGB  }, // ASTC8x5
	{ DXGI_FORMAT_ASTC_8X6_UNORM,     DXGI_FORMAT_ASTC_8X6_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_8X6_UNORM_SRGB  }, // ASTC8x6
	{ DXGI_FORMAT_ASTC_10X5_UNORM,    DXGI_FORMAT_ASTC_10X5_UNORM,       DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_10X5_UNORM_SRGB }, // ASTC10x5
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // Unknown
	{ DXGI_FORMAT_R1_UNORM,           DXGI_FORMAT_R1_UNORM,              DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // R1
	{ DXGI_FORMAT_A8_UNORM,           DXGI_FORMAT_A8_UNORM,              DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // A8
	{ DXGI_FORMAT_R8_UNORM,           DXGI_FORMAT_R8_UNORM,              DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // R8
	{ DXGI_FORMAT_R8_SINT,            DXGI_FORMAT_R8_SINT,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // R8I
	{ DXGI_FORMAT_R8_UINT,            DXGI_FORMAT_R8_UINT,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // R8U
	{ DXGI_FORMAT_R8_SNORM,           DXGI_FORMAT_R8_SNORM,              DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // R8S
	{ DXGI_FORMAT_R16_UNORM,          DXGI_FORMAT_R16_UNORM,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // R16
	{ DXGI_FORMAT_R16_SINT,           DXGI_FORMAT_R16_SINT,              DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // R16I
	{ DXGI_FORMAT_R16_UINT,           DXGI_FORMAT_R16_UINT,              DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // R16U
	{ DXGI_FORMAT_R16_FLOAT,          DXGI_FORMAT_R16_FLOAT,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // R16F
	{ DXGI_FORMAT_R16_SNORM,          DXGI_FORMAT_R16_SNORM,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // R16S
	{ DXGI_FORMAT_R32_SINT,           DXGI_FORMAT_R32_SINT,              DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // R32I
	{ DXGI_FORMAT_R32_UINT,           DXGI_FORMAT_R32_UINT,              DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // R32U
	{ DXGI_FORMAT_R32_FLOAT,          DXGI_FORMAT_R32_FLOAT,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // R32F
	{ DXGI_FORMAT_R8G8_UNORM,         DXGI_FORMAT_R8G8_UNORM,            DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RG8
	{ DXGI_FORMAT_R8G8_SINT,          DXGI_FORMAT_R8G8_SINT,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RG8I
	{ DXGI_FORMAT_R8G8_UINT,          DXGI_FORMAT_R8G8_UINT,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RG8U
	{ DXGI_FORMAT_R8G8_SNORM,         DXGI_FORMAT_R8G8_SNORM,            DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RG8S
	{ DXGI_FORMAT_R16G16_UNORM,       DXGI_FORMAT_R16G16_UNORM,          DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RG16
	{ DXGI_FORMAT_R16G16_SINT,        DXGI_FORMAT_R16G16_SINT,           DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RG16I
	{ DXGI_FORMAT_R16G16_UINT,        DXGI_FORMAT_R16G16_UINT,           DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RG16U
	{ DXGI_FORMAT_R16G16_FLOAT,       DXGI_FORMAT_R16G16_FLOAT,          DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RG16F
	{ DXGI_FORMAT_R16G16_SNORM,       DXGI_FORMAT_R16G16_SNORM,          DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RG16S
	{ DXGI_FORMAT_R32G32_SINT,        DXGI_FORMAT_R32G32_SINT,           DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RG32I
	{ DXGI_FORMAT_R32G32_UINT,        DXGI_FORMAT_R32G32_UINT,           DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RG32U
	{ DXGI_FORMAT_R32G32_FLOAT,       DXGI_FORMAT_R32G32_FLOAT,          DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RG32F
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGB8
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGB8I
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGB8U
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGB8S
	{ DXGI_FORMAT_R9G9B9E5_SHAREDEXP, DXGI_FORMAT_R9G9B9E5_SHAREDEXP,    DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGB9E5F
	{ DXGI_FORMAT_B8G8R8A8_UNORM,     DXGI_FORMAT_B8G8R8A8_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_B8G8R8A8_UNORM_SRGB  }, // BGRA8
	{ DXGI_FORMAT_R8G8B8A8_UNORM,     DXGI_FORMAT_R8G8B8A8_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_R8G8B8A8_UNORM_SRGB  }, // RGBA8
	{ DXGI_FORMAT_R8G8B8A8_SINT,      DXGI_FORMAT_R8G8B8A8_SINT,         DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_R8G8B8A8_UNORM_SRGB  }, // RGBA8I
	{ DXGI_FORMAT_R8G8B8A8_UINT,      DXGI_FORMAT_R8G8B8A8_UINT,         DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_R8G8B8A8_UNORM_SRGB  }, // RGBA8U
	{ DXGI_FORMAT_R8G8B8A8_SNORM,     DXGI_FORMAT_R8G8B8A8_SNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGBA8S
	{ DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM,    DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGBA16
	{ DXGI_FORMAT_R16G16B16A16_SINT,  DXGI_FORMAT_R16G16B16A16_SINT,     DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGBA16I
	{ DXGI_FORMAT_R16G16B16A16_UINT,  DXGI_FORMAT_R16G16B16A16_UINT,     DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGBA16U
	{ DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT,    DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGBA16F
	{ DXGI_FORMAT_R16G16B16A16_SNORM, DXGI_FORMAT_R16G16B16A16_SNORM,    DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGBA16S
	{ DXGI_FORMAT_R32G32B32A32_SINT,  DXGI_FORMAT_R32G32B32A32_SINT,     DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGBA32I
	{ DXGI_FORMAT_R32G32B32A32_UINT,  DXGI_FORMAT_R32G32B32A32_UINT,     DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGBA32U
	{ DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT,    DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGBA32F
	{ DXGI_FORMAT_B5G6R5_UNORM,       DXGI_FORMAT_B5G6R5_UNORM,          DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // R5G6B5
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGBA4
	{ DXGI_FORMAT_B5G5R5A1_UNORM,     DXGI_FORMAT_B5G5R5A1_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGB5A1
	{ DXGI_FORMAT_R10G10B10A2_UNORM,  DXGI_FORMAT_R10G10B10A2_UNORM,     DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RGB10A2
	{ DXGI_FORMAT_R11G11B10_FLOAT,    DXGI_FORMAT_R11G11B10_FLOAT,       DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // RG11B10F
	{ DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN              }, // UnknownDepth
	{ DXGI_FORMAT_R16_TYPELESS,       DXGI_FORMAT_R16_UNORM,             DXGI_FORMAT_D16_UNORM,         DXGI_FORMAT_UNKNOWN              }, // D16
	{ DXGI_FORMAT_R24G8_TYPELESS,     DXGI_FORMAT_R24_UNORM_X8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_UNKNOWN              }, // D24
	{ DXGI_FORMAT_R24G8_TYPELESS,     DXGI_FORMAT_R24_UNORM_X8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_UNKNOWN              }, // D24S8
	{ DXGI_FORMAT_R24G8_TYPELESS,     DXGI_FORMAT_R24_UNORM_X8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_UNKNOWN              }, // D32
	{ DXGI_FORMAT_R32_TYPELESS,       DXGI_FORMAT_R32_FLOAT,             DXGI_FORMAT_D32_FLOAT,         DXGI_FORMAT_UNKNOWN              }, // D16F
	{ DXGI_FORMAT_R32_TYPELESS,       DXGI_FORMAT_R32_FLOAT,             DXGI_FORMAT_D32_FLOAT,         DXGI_FORMAT_UNKNOWN              }, // D24F
	{ DXGI_FORMAT_R32_TYPELESS,       DXGI_FORMAT_R32_FLOAT,             DXGI_FORMAT_D32_FLOAT,         DXGI_FORMAT_UNKNOWN              }, // D32F
	{ DXGI_FORMAT_R24G8_TYPELESS,     DXGI_FORMAT_R24_UNORM_X8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_UNKNOWN              }, // D0S8
};

GfxTextureD3D11::GfxTextureD3D11()
	: GfxTexture()
{

}

GfxTextureD3D11::~GfxTextureD3D11()
{
	ReleaseTexture();
}

void GfxTextureD3D11::ReleaseTexture()
{
	D3D11_SAFE_RELEASE(resolveTexture_);
	D3D11_SAFE_RELEASE(d3d11Texture2D_);
	D3D11_SAFE_RELEASE(d3d11Texture3D_);

	gfxRenderSurfaces_.Clear();
}

void GfxTextureD3D11::CreateTexture2D()
{
	GfxDeviceD3D11* gfxDevice = GetSubsystem<GfxDeviceD3D11>();
	ID3D11Device* d3d11Device = gfxDevice->GetD3D11Device();

	D3D11_TEXTURE2D_DESC textureDesc;
	Memory::Memzero(&textureDesc, sizeof(textureDesc));
	textureDesc.Format = textureDesc_.sRGB_ ? d3d11TextureFormatInfo[textureDesc_.format_].srgbFormat_ : d3d11TextureFormatInfo[textureDesc_.format_].format_;

	if (textureDesc_.multiSample_ > 1 && gfxDevice->CheckMultiSampleSupport(textureDesc.Format, textureDesc_.multiSample_))
	{
		textureDesc_.multiSample_ = 1;
		textureDesc_.autoResolve_ = false;
	}

	if (textureDesc_.usage_ == TEXTURE_DEPTHSTENCIL)
	{
		textureDesc_.levels_ = 1;
	}

	textureDesc.Width = (UINT)textureDesc_.width_;
	textureDesc.Height = (UINT)textureDesc_.height_;
	textureDesc.MipLevels = (textureDesc_.multiSample_ == 1 && textureDesc_.usage_ != TEXTURE_DYNAMIC) ? textureDesc_.levels_ : 1;
	textureDesc.ArraySize = textureDesc_.layers_ ? textureDesc_.layers_ : 1;
	textureDesc.SampleDesc.Count = (UINT)textureDesc_.multiSample_;
	textureDesc.SampleDesc.Quality = gfxDevice->GetMultiSampleQuality(textureDesc.Format, textureDesc_.multiSample_);
	textureDesc.Usage = textureDesc_.usage_ == TEXTURE_DYNAMIC ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (textureDesc_.usage_ == TEXTURE_RENDERTARGET)
	{
		textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	else if (textureDesc_.usage_ == TEXTURE_DEPTHSTENCIL)
	{
		textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	}
	textureDesc.CPUAccessFlags = textureDesc_.usage_ == TEXTURE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

	if (textureDesc_.usage_ == TEXTURE_DEPTHSTENCIL && textureDesc_.multiSample_ > 1 && d3d11Device->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_1)
	{
		textureDesc.BindFlags &= ~D3D11_BIND_SHADER_RESOURCE;
	}

	HRESULT hr = d3d11Device->CreateTexture2D(&textureDesc, nullptr, &d3d11Texture2D_);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("Failed to create texture2d.");
		D3D11_SAFE_RELEASE(d3d11Texture2D_);
		return;
	}

	if (textureDesc_.multiSample_ > 1 && textureDesc_.autoResolve_)
	{
		textureDesc.MipLevels = textureDesc_.levels_;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		if (textureDesc_.levels_ != 1)
		{
			textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}

		HRESULT hr = d3d11Device->CreateTexture2D(&textureDesc, nullptr, (ID3D11Texture2D**)&resolveTexture_);
		if (FAILED(hr))
		{
			FLAGGG_LOG_ERROR("Failed to create resolve texture.");
			D3D11_SAFE_RELEASE(resolveTexture_);
			return;
		}
	}

	if (textureDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		bool multiSample = (textureDesc_.multiSample_ > 1 && !textureDesc_.autoResolve_);

		D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
		Memory::Memzero(&resourceViewDesc, sizeof(resourceViewDesc));
		resourceViewDesc.Format = textureDesc_.sRGB_ ? d3d11TextureFormatInfo[textureDesc_.format_].srgbFormat_ : d3d11TextureFormatInfo[textureDesc_.format_].srvFormat_;
		if (textureDesc.ArraySize <= 1)
		{
			resourceViewDesc.ViewDimension = multiSample ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		}
		else
		{
			if (multiSample)
			{
				resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
				resourceViewDesc.Texture2DMSArray.ArraySize = textureDesc.ArraySize;
			}
			else
			{
				resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				resourceViewDesc.Texture2DArray.MipLevels = textureDesc.MipLevels;
				resourceViewDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;
			}
		}
		resourceViewDesc.Texture2D.MipLevels = textureDesc_.usage_ != TEXTURE_DYNAMIC ? (UINT)textureDesc_.levels_ : 1;

		ID3D11Resource* viewObject = resolveTexture_ ? resolveTexture_ : d3d11Texture2D_;
		hr = d3d11Device->CreateShaderResourceView(viewObject, &resourceViewDesc, &shaderResourceView_);
		if (FAILED(hr))
		{
			FLAGGG_LOG_ERROR("Failed to create shader resource view.");
			D3D11_SAFE_RELEASE(shaderResourceView_);
			return;
		}

		if (textureDesc_.subResourceViewEnable_)
		{
			for (UInt32 index = 0; index < textureDesc.ArraySize; ++index)
			{
				if (multiSample)
				{
					Memory::Memzero(&resourceViewDesc, sizeof(resourceViewDesc));

					if (textureDesc.ArraySize <= 1)
						resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
					else
					{
						resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
						resourceViewDesc.Texture2DMSArray.FirstArraySlice = index;
						resourceViewDesc.Texture2DMSArray.ArraySize = textureDesc.ArraySize;
					}

					ID3D11Resource* viewObject = resolveTexture_ ? resolveTexture_ : d3d11Texture2D_;
					ID3D11ShaderResourceView* shaderResourceView = nullptr;
					hr = d3d11Device->CreateShaderResourceView(viewObject, &resourceViewDesc, &shaderResourceView);
					if (FAILED(hr))
					{
						FLAGGG_LOG_ERROR("Failed to create shader sub resource view.");
						D3D11_SAFE_RELEASE(shaderResourceView);
						return;
					}

					gfxTextureViews_.Push(MakeShared<GfxShaderResourceViewD3D11>(this, shaderResourceView, textureDesc_.width_, textureDesc_.height_));
				}
				else
				{
					UInt32 levels = textureDesc_.usage_ != TEXTURE_DYNAMIC ? textureDesc_.levels_ : 1;
					for (UInt32 level = 0; level < levels; ++level)
					{
						Memory::Memzero(&resourceViewDesc, sizeof(resourceViewDesc));

						if (textureDesc.ArraySize <= 1)
						{
							resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
							resourceViewDesc.Texture2D.MostDetailedMip = level;
							resourceViewDesc.Texture2D.MipLevels = 1;
						}
						else
						{
							resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
							resourceViewDesc.Texture2DArray.MostDetailedMip = level;
							resourceViewDesc.Texture2DArray.MipLevels = 1;
							resourceViewDesc.Texture2DArray.FirstArraySlice = index;
							resourceViewDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;
						}

						ID3D11Resource* viewObject = resolveTexture_ ? resolveTexture_ : d3d11Texture2D_;
						ID3D11ShaderResourceView* shaderResourceView = nullptr;
						hr = d3d11Device->CreateShaderResourceView(viewObject, &resourceViewDesc, &shaderResourceView);
						if (FAILED(hr))
						{
							FLAGGG_LOG_ERROR("Failed to create shader sub resource view.");
							D3D11_SAFE_RELEASE(shaderResourceView);
							return;
						}

						gfxTextureViews_.Push(MakeShared<GfxShaderResourceViewD3D11>(this, shaderResourceView, textureDesc_.width_ >> level, textureDesc_.height_ >> level));
					}
				}
			}
		}
	}

	if (textureDesc_.usage_ == TEXTURE_RENDERTARGET)
	{
		for (UInt32 index = 0; index < textureDesc.ArraySize; ++index)
		{
			if (textureDesc_.multiSample_ > 1)
			{
				D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
				Memory::Memzero(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
				renderTargetViewDesc.Format = textureDesc.Format;
				renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
				if (textureDesc.ArraySize > 1)
				{
					renderTargetViewDesc.Texture2DMSArray.FirstArraySlice = index;
					renderTargetViewDesc.Texture2DMSArray.ArraySize = 1;
				}

				ID3D11RenderTargetView* renderTargetView;
				hr = d3d11Device->CreateRenderTargetView(d3d11Texture2D_, &renderTargetViewDesc, &renderTargetView);
				if (FAILED(hr))
				{
					FLAGGG_LOG_ERROR("Failed to create rendertarget view.");
					D3D11_SAFE_RELEASE(renderTargetView);
					return;
				}

				SharedPtr<GfxRenderSurfaceD3D11> renderSurface(new GfxRenderSurfaceD3D11(this, textureDesc_.width_, textureDesc_.height_));
				renderSurface->SetRenderTargetView(renderTargetView);
				gfxRenderSurfaces_.Push(renderSurface);
			}
			else
			{
				for (UInt32 level = 0; level < textureDesc_.levels_; ++level)
				{
					D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
					Memory::Memzero(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
					renderTargetViewDesc.Format = textureDesc.Format;
					renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
					if (textureDesc.ArraySize <= 1)
						renderTargetViewDesc.Texture2D.MipSlice = level;
					else
					{
						renderTargetViewDesc.Texture2DArray.MipSlice = level;
						renderTargetViewDesc.Texture2DArray.FirstArraySlice = index;
						renderTargetViewDesc.Texture2DArray.ArraySize = 1;
					}

					ID3D11RenderTargetView* renderTargetView;
					hr = d3d11Device->CreateRenderTargetView(d3d11Texture2D_, &renderTargetViewDesc, &renderTargetView);
					if (FAILED(hr))
					{
						FLAGGG_LOG_ERROR("Failed to create rendertarget view.");
						D3D11_SAFE_RELEASE(renderTargetView);
						return;
					}

					SharedPtr<GfxRenderSurfaceD3D11> renderSurface(new GfxRenderSurfaceD3D11(this, textureDesc_.width_ >> level, textureDesc_.height_ >> level));
					renderSurface->SetRenderTargetView(renderTargetView);
					gfxRenderSurfaces_.Push(renderSurface);
				}
			}
		}
	}
	else if (textureDesc_.usage_ == TEXTURE_DEPTHSTENCIL)
	{
		for (UInt32 index = 0; index < textureDesc.ArraySize; ++index)
		{
			if (textureDesc_.multiSample_ > 1)
			{
				D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
				Memory::Memzero(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
				depthStencilViewDesc.Format = d3d11TextureFormatInfo[textureDesc_.format_].dsvFormat_;
				depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
				if (textureDesc.ArraySize > 1)
				{
					depthStencilViewDesc.Texture2DMSArray.FirstArraySlice = index;
					depthStencilViewDesc.Texture2DMSArray.ArraySize = 1;
				}

				ID3D11DepthStencilView* depthStencilView;
				hr = d3d11Device->CreateDepthStencilView(d3d11Texture2D_, &depthStencilViewDesc, &depthStencilView);
				if (FAILED(hr))
				{
					FLAGGG_LOG_ERROR("Failed to create depth-stencil view.");
					D3D11_SAFE_RELEASE(depthStencilView);
					return;
				}

				SharedPtr<GfxRenderSurfaceD3D11> renderSurface(new GfxRenderSurfaceD3D11(this, textureDesc_.width_, textureDesc_.height_));
				renderSurface->SetDepthStencilView(depthStencilView);
				gfxRenderSurfaces_.Push(renderSurface);
			}
			else
			{
				for (UInt32 level = 0; level < textureDesc_.levels_; ++level)
				{
					D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
					Memory::Memzero(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
					depthStencilViewDesc.Format = d3d11TextureFormatInfo[textureDesc_.format_].dsvFormat_;
					depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
					if (textureDesc.ArraySize <= 1)
						depthStencilViewDesc.Texture2D.MipSlice = level;
					else
					{
						depthStencilViewDesc.Texture2DArray.MipSlice = level;
						depthStencilViewDesc.Texture2DArray.FirstArraySlice = index;
						depthStencilViewDesc.Texture2DArray.ArraySize = 1;
					}

					ID3D11DepthStencilView* depthStencilView;
					hr = d3d11Device->CreateDepthStencilView(d3d11Texture2D_, &depthStencilViewDesc, &depthStencilView);
					if (FAILED(hr))
					{
						FLAGGG_LOG_ERROR("Failed to create depth-stencil view.");
						D3D11_SAFE_RELEASE(depthStencilView);
						return;
					}

					SharedPtr<GfxRenderSurfaceD3D11> renderSurface(new GfxRenderSurfaceD3D11(this, textureDesc_.width_ >> level, textureDesc_.height_ >> level));
					renderSurface->SetDepthStencilView(depthStencilView);
					gfxRenderSurfaces_.Push(renderSurface);
				}
			}
		}

		//if (d3d11Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0)
		//{
		//	depthStencilViewDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;
		//	hr = d3d11Device->CreateDepthStencilView(d3d11Texture2D_, &depthStencilViewDesc,
		//		(ID3D11DepthStencilView**)&renderSurface_->readOnlyView_);
		//	if (FAILED(hr))
		//	{
		//		FLAGGG_LOG_ERROR("Failed to create read-only depth-stencil view.");
		//		D3D11_SAFE_RELEASE(renderSurface_->readOnlyView_);
		//	}
		//}
	}
}

void GfxTextureD3D11::CreateTexture3D()
{
	GfxDeviceD3D11* gfxDevice = GetSubsystem<GfxDeviceD3D11>();
	ID3D11Device* d3d11Device = gfxDevice->GetD3D11Device();

	D3D11_TEXTURE3D_DESC textureDesc;
	Memory::Memzero(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = (UINT)textureDesc_.width_;
	textureDesc.Height = (UINT)textureDesc_.height_;
	textureDesc.Depth = (UINT)textureDesc_.depth_;
	textureDesc.MipLevels = textureDesc_.usage_ != TEXTURE_DYNAMIC ? textureDesc_.levels_ : 1;
	textureDesc.Format = textureDesc_.sRGB_ ? d3d11TextureFormatInfo[textureDesc_.format_].srgbFormat_ : d3d11TextureFormatInfo[textureDesc_.format_].format_;
	textureDesc.Usage = textureDesc_.usage_ == TEXTURE_DYNAMIC ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = textureDesc_.usage_ == TEXTURE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

	HRESULT hr = d3d11Device->CreateTexture3D(&textureDesc, nullptr, &d3d11Texture3D_);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("Failed to create texture3d.");
		D3D11_SAFE_RELEASE(d3d11Texture3D_);
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
	Memory::Memzero(&resourceViewDesc, sizeof(resourceViewDesc));
	resourceViewDesc.Format = d3d11TextureFormatInfo[textureDesc_.format_].srvFormat_;
	resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	resourceViewDesc.Texture3D.MipLevels = textureDesc_.usage_ != TEXTURE_DYNAMIC ? (UINT)textureDesc_.levels_ : 1;

	hr = d3d11Device->CreateShaderResourceView(d3d11Texture3D_, &resourceViewDesc, &shaderResourceView_);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("Failed to create shader resource view.");
		return;
	}
}

void GfxTextureD3D11::CreateTextureCube()
{
	GfxDeviceD3D11* gfxDevice = GetSubsystem<GfxDeviceD3D11>();
	ID3D11Device* d3d11Device = gfxDevice->GetD3D11Device();

	D3D11_TEXTURE2D_DESC textureDesc;
	Memory::Memzero(&textureDesc, sizeof(textureDesc));
	textureDesc.Format = textureDesc_.sRGB_ ? d3d11TextureFormatInfo[textureDesc_.format_].srgbFormat_ : d3d11TextureFormatInfo[textureDesc_.format_].format_;

	if (textureDesc_.multiSample_ > 1 && gfxDevice->CheckMultiSampleSupport(textureDesc.Format, textureDesc_.multiSample_))
	{
		textureDesc_.multiSample_ = 1;
		textureDesc_.autoResolve_ = false;
	}

	textureDesc.Width = (UINT)textureDesc_.width_;
	textureDesc.Height = (UINT)textureDesc_.height_;
	textureDesc.MipLevels = (textureDesc_.multiSample_ == 1 && textureDesc_.usage_ != TEXTURE_DYNAMIC) ? textureDesc_.levels_ : 1;
	textureDesc.ArraySize = MAX_CUBEMAP_FACES;
	textureDesc.SampleDesc.Count = (UINT)textureDesc_.multiSample_;
	textureDesc.SampleDesc.Quality = gfxDevice->GetMultiSampleQuality(textureDesc.Format, textureDesc_.multiSample_);
	textureDesc.Usage = textureDesc_.usage_ == TEXTURE_DYNAMIC ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (textureDesc_.usage_ == TEXTURE_RENDERTARGET)
	{
		textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	else if (textureDesc_.usage_ == TEXTURE_DEPTHSTENCIL)
	{
		textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	}
	textureDesc.CPUAccessFlags = textureDesc_.usage_ == TEXTURE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

	if (textureDesc_.multiSample_ < 2)
	{
		textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	HRESULT hr = d3d11Device->CreateTexture2D(&textureDesc, nullptr, &d3d11Texture2D_);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("Failed to create texture2d.");
		D3D11_SAFE_RELEASE(d3d11Texture2D_);
		return;
	}

	if (textureDesc_.multiSample_ > 1)
	{
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
		if (textureDesc_.levels_ != 1)
		{
			textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}

		HRESULT hr = d3d11Device->CreateTexture2D(&textureDesc, nullptr, (ID3D11Texture2D**)&resolveTexture_);
		if (FAILED(hr))
		{
			FLAGGG_LOG_ERROR("Failed to create resolve texture.");
			D3D11_SAFE_RELEASE(resolveTexture_);
			return;
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
	Memory::Memzero(&resourceViewDesc, sizeof(resourceViewDesc));
	resourceViewDesc.Format = d3d11TextureFormatInfo[textureDesc_.format_].srvFormat_;
	resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	resourceViewDesc.Texture2D.MipLevels = textureDesc_.usage_ != TEXTURE_DYNAMIC ? (UINT)textureDesc_.levels_ : 1;

	ID3D11Resource* viewObject = resolveTexture_ ? resolveTexture_ : d3d11Texture2D_;
	hr = d3d11Device->CreateShaderResourceView(viewObject, &resourceViewDesc, &shaderResourceView_);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("Failed to create shader resource view.");
		D3D11_SAFE_RELEASE(shaderResourceView_);
		return;
	}

	if (textureDesc_.usage_ == TEXTURE_RENDERTARGET)
	{
		for (UInt32 i = 0; i < MAX_CUBEMAP_FACES; ++i)
		{
			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
			Memory::Memzero(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
			renderTargetViewDesc.Format = textureDesc.Format;
			if (textureDesc_.multiSample_ > 1)
			{
				renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
				renderTargetViewDesc.Texture2DMSArray.ArraySize = 1;
				renderTargetViewDesc.Texture2DMSArray.FirstArraySlice = i;
			}
			else
			{
				renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
				renderTargetViewDesc.Texture2DArray.ArraySize = 1;
				renderTargetViewDesc.Texture2DArray.FirstArraySlice = i;
				renderTargetViewDesc.Texture2DArray.MipSlice = 0;
			}

			ID3D11RenderTargetView* renderTargetView;
			hr = d3d11Device->CreateRenderTargetView(d3d11Texture2D_, &renderTargetViewDesc, &renderTargetView);
			if (FAILED(hr))
			{
				FLAGGG_LOG_ERROR("Failed to create rendertarget view.");
				D3D11_SAFE_RELEASE(renderTargetView);
				return;
			}

			SharedPtr<GfxRenderSurfaceD3D11> renderSurface(new GfxRenderSurfaceD3D11(this, textureDesc_.width_, textureDesc_.height_));
			renderSurface->SetRenderTargetView(renderTargetView);
			gfxRenderSurfaces_.Push(renderSurface);
		}
	}
}

void GfxTextureD3D11::SetGpuTag(const String& gpuTag)
{
	if (d3d11Texture2D_)
	{
		d3d11Texture2D_->SetPrivateData(WKPDID_D3DDebugObjectName, gpuTag.Length(), gpuTag.CString());
	}

	if (d3d11Texture3D_)
	{
		d3d11Texture3D_->SetPrivateData(WKPDID_D3DDebugObjectName, gpuTag.Length(), gpuTag.CString());
	}

	for (UInt32 i = 0; i < textureDesc_.levels_; ++i)
	{
		if (auto* renderSurface = GetRenderSurface(0, i))
		{
			renderSurface->SetGpuTag(ToString("%s-RenderView(%d x %d)", gpuTag.CString(), textureDesc_.width_ >> i, textureDesc_.height_ >> i));
		}
		if (auto* textureView = GetSubResourceView(0, i))
		{
			textureView->SetGpuTag(ToString("%s-TextureView(%d x %d)", gpuTag.CString(), textureDesc_.width_ >> i, textureDesc_.height_ >> i));
		}
	}

	if (shaderResourceView_)
	{
		const String viewGpuTag = gpuTag + "-TextureView";
		shaderResourceView_->SetPrivateData(WKPDID_D3DDebugObjectName, viewGpuTag.Length(), viewGpuTag.CString());
	}
}

void GfxTextureD3D11::Apply(const void* initialDataPtr)
{
	ReleaseTexture();

	if (!textureDesc_.width_ || !textureDesc_.height_)
	{
		FLAGGG_LOG_ERROR("Texture's width or height is invalid.");
		return;
	}

	textureDesc_.levels_ = GfxTextureUtils::CheckMaxLevels(textureDesc_.width_, textureDesc_.height_, textureDesc_.requestedLevels_);

	if (textureDesc_.depth_ > 1)
	{
		CreateTexture3D();
	}
	else if (textureDesc_.isCube_)
	{
		CreateTextureCube();
	}
	else
	{
		CreateTexture2D();
	}
}

void GfxTextureD3D11::UpdateTexture(const void* dataPtr)
{

}

void GfxTextureD3D11::UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 width, UInt32 height)
{
	if (!d3d11Texture2D_)
	{
		FLAGGG_LOG_ERROR("Gfx texture invalid or format conflict.");
		return;
	}

	if (!dataPtr)
	{
		FLAGGG_LOG_ERROR("Texture2D ==> set nullptr data.");
		return;
	}

	if (level >= textureDesc_.levels_)
	{
		FLAGGG_LOG_ERROR("Texture2D ==> illegal mip level.");
		return;
	}

	const TextureMipInfo mipInfo = GetMipInfo(level);
	if (x < 0 || x + width > mipInfo.width_ || y < 0 || y + height > mipInfo.height_ || width <= 0 || height <= 0)
	{
		FLAGGG_LOG_ERROR("Texture2D ==> illegal dimensions.");
		return;
	}

	const UInt8* src = static_cast<const UInt8*>(dataPtr);
	UInt32 rowSize = GfxTextureUtils::GetRowDataSize(textureDesc_.format_, width);
	UInt32 rowStart = GfxTextureUtils::GetRowDataSize(textureDesc_.format_, x);
	UInt32 subResource = D3D11CalcSubresource(level, index, textureDesc_.levels_);

	if (textureDesc_.usage_ == TEXTURE_DYNAMIC)
	{
		D3D11_MAPPED_SUBRESOURCE mappedData;
		mappedData.pData = nullptr;

		HRESULT hr = GetSubsystem<GfxDeviceD3D11>()->GetD3D11DeviceContext()->Map(d3d11Texture2D_,
			subResource, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		if (FAILED(hr) || !mappedData.pData)
		{
			FLAGGG_LOG_ERROR("Failed to update texture resource.");
			return;
		}
		else
		{
			for (Int32 row = 0; row < height; ++row)
				Memory::Memcpy((UInt8*)mappedData.pData + (row + y) * mappedData.RowPitch + rowStart, src + row * rowSize, rowSize);

			GetSubsystem<GfxDeviceD3D11>()->GetD3D11DeviceContext()->Unmap(d3d11Texture2D_, subResource);
		}
	}
	else
	{
		D3D11_BOX destBox;
		destBox.left = (UINT)x;
		destBox.right = (UINT)(x + width);
		destBox.top = (UINT)y;
		destBox.bottom = (UINT)(y + height);
		destBox.front = 0;
		destBox.back = 1;

		GetSubsystem<GfxDeviceD3D11>()->GetD3D11DeviceContext()->UpdateSubresource(d3d11Texture2D_, subResource, &destBox, dataPtr, rowSize, 0);
	}
}

void GfxTextureD3D11::UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 z, UInt32 width, UInt32 height, UInt32 depth)
{
	if (!d3d11Texture3D_)
	{
		FLAGGG_LOG_ERROR("Gfx texture invalid or format conflict.");
		return;
	}

	if (!dataPtr)
	{
		FLAGGG_LOG_ERROR("Texture3D ==> set nullptr data.");
		return;
	}

	if (level >= textureDesc_.levels_)
	{
		FLAGGG_LOG_ERROR("Texture3D ==> illegal mip level.");
		return;
	}

	const TextureMipInfo mipInfo = GetMipInfo(level);
	if (x < 0 || x + width > mipInfo.width_ || y < 0 || y + height > mipInfo.height_ || z < 0 || z + depth > mipInfo.depth_ || width <= 0 || height <= 0 || depth <= 0)
	{
		FLAGGG_LOG_ERROR("Texture3D ==> illegal dimensions.");
		return;
	}

	const UInt8* src = static_cast<const UInt8*>(dataPtr);
	UInt32 rowSize = GfxTextureUtils::GetRowDataSize(textureDesc_.format_, width);
	UInt32 rowStart = GfxTextureUtils::GetRowDataSize(textureDesc_.format_, x);
	UInt32 subResource = D3D11CalcSubresource(level, index, textureDesc_.levels_);

	if (textureDesc_.usage_ == TEXTURE_DYNAMIC)
	{
		D3D11_MAPPED_SUBRESOURCE mappedData;
		mappedData.pData = nullptr;

		HRESULT hr = GetSubsystem<GfxDeviceD3D11>()->GetD3D11DeviceContext()->Map(d3d11Texture3D_,
			subResource, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		if (FAILED(hr) || !mappedData.pData)
		{
			FLAGGG_LOG_ERROR("Failed to update texture resource.");
			return;
		}
		else
		{
			for (Int32 page = 0; page < depth; ++page)
			{
				for (Int32 row = 0; row < height; ++row)
					Memory::Memcpy((UInt8*)mappedData.pData + (page + z) * mappedData.DepthPitch + (row + y) * mappedData.RowPitch + rowStart,
						src + row * rowSize, rowSize);
			}

			GetSubsystem<GfxDeviceD3D11>()->GetD3D11DeviceContext()->Unmap(d3d11Texture3D_, subResource);
		}
	}
	else
	{
		D3D11_BOX destBox;
		destBox.left = (UINT)x;
		destBox.right = (UINT)(x + width);
		destBox.top = (UINT)y;
		destBox.bottom = (UINT)(y + height);
		destBox.front = (UINT)z;
		destBox.back = (UINT)(z + depth);

		GetSubsystem<GfxDeviceD3D11>()->GetD3D11DeviceContext()->UpdateSubresource(d3d11Texture3D_, subResource, &destBox, dataPtr, rowSize, mipInfo.height_ * rowSize);
	}
}

void GfxTextureD3D11::UpdateTexture(GfxTexture* gfxTexture)
{
	auto* gfxTextureD3D11 = RTTICast<GfxTextureD3D11>(gfxTexture);
	if (gfxTextureD3D11)
	{
		GetSubsystem<GfxDeviceD3D11>()->GetD3D11DeviceContext()->CopyResource(GetD3D11Resource(), gfxTextureD3D11->GetD3D11Resource());
	}
}

bool GfxTextureD3D11::ReadBack(void* dataPtr, UInt32 index, UInt32 level)
{
	return ReadBackSubRegion(dataPtr, index, level, 0, 0, textureDesc_.width_, textureDesc_.height_);
}

bool GfxTextureD3D11::ReadBackSubRegion(void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 width, UInt32 height)
{
	auto* gfxDevice = GetSubsystem<GfxDeviceD3D11>();
	auto* d3d11Device = GetSubsystem<GfxDeviceD3D11>()->GetD3D11Device();
	auto* d3d11DeviceContext = GetSubsystem<GfxDeviceD3D11>()->GetD3D11DeviceContext();

	D3D11_TEXTURE2D_DESC textureDesc;
	Memory::Memzero(&textureDesc, sizeof(textureDesc));
	textureDesc.Format = textureDesc_.sRGB_ ? d3d11TextureFormatInfo[textureDesc_.format_].srgbFormat_ : d3d11TextureFormatInfo[textureDesc_.format_].format_;
	textureDesc.Width = (UINT)width;
	textureDesc.Height = (UINT)height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = gfxDevice->GetMultiSampleQuality(textureDesc.Format, 1);
	textureDesc.Usage = D3D11_USAGE_STAGING;
	textureDesc.BindFlags = 0;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	ID3D11Texture2D* copyTexture;
	HRESULT hr = d3d11Device->CreateTexture2D(&textureDesc, nullptr, &copyTexture);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("Can not create d3d11 texture2d");
		return false;
	}

	UInt32 copySubResource = D3D11CalcSubresource(level, index, textureDesc_.levels_);
	UInt32 subResource = D3D11CalcSubresource(0, 0, 1);

	D3D11_BOX copyBox;
	copyBox.left = x;
	copyBox.top = y;
	copyBox.right = x + width;
	copyBox.bottom = y + height;
	copyBox.front = 0;
	copyBox.back = 1;

	d3d11DeviceContext->CopySubresourceRegion(copyTexture, subResource, 0, 0, 0, GetD3D11Resource(), copySubResource, &copyBox);

	D3D11_MAPPED_SUBRESOURCE mapped;
	d3d11DeviceContext->Map(copyTexture, subResource, D3D11_MAP_READ, 0, &mapped);

	UInt32 destRowDataSize = GfxTextureUtils::GetRowDataSize(textureDesc_.format_, width);
	const char* srcPtr = (const char*)mapped.pData;
	char* destPtr = (char*)dataPtr;
	for (UInt32 row = 0; row < height; ++row)
	{
		Memory::Memcpy(destPtr, srcPtr, destRowDataSize);
		srcPtr += mapped.RowPitch;
		destPtr += destRowDataSize;
	}

	d3d11DeviceContext->Unmap(copyTexture, subResource);

	D3D11_SAFE_RELEASE(copyTexture);

	return true;
}

GfxShaderResourceView* GfxTextureD3D11::GetSubResourceView(UInt32 index, UInt32 level)
{
	UInt32 layers = textureDesc_.isCube_ ? 6 : textureDesc_.layers_;
	UInt32 arrayIndex = index * layers + level;
	return arrayIndex < gfxTextureViews_.Size() ? gfxTextureViews_[arrayIndex] : nullptr;
}

GfxRenderSurface* GfxTextureD3D11::GetRenderSurface() const
{
	return gfxRenderSurfaces_.Size() ? gfxRenderSurfaces_[0] : nullptr;
}

GfxRenderSurface* GfxTextureD3D11::GetRenderSurface(UInt32 index, UInt32 level) const
{
	UInt32 layers = textureDesc_.isCube_ ? 6 : textureDesc_.layers_;
	UInt32 arrayIndex = index * layers + level;
	return arrayIndex < gfxRenderSurfaces_.Size() ? gfxRenderSurfaces_[arrayIndex] : nullptr;
}

ID3D11Resource* GfxTextureD3D11::GetD3D11Resource() const
{
	if (d3d11Texture2D_)
		return d3d11Texture2D_;

	if (d3d11Texture3D_)
		return d3d11Texture3D_;

	return nullptr;
}

}
