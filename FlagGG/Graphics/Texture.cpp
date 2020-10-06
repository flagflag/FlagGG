#include "Graphics/Texture.h"
#include "Graphics/RenderEngine.h"
#include "Math/Math.h"
#include "Log.h"
#include "bgfx/bgfx.h"
#include "bimg/bimg.h"

namespace FlagGG
{
	namespace Graphics
	{
		static const uint32_t bgfxWrapU[] =
		{
			BGFX_TEXTURE_NONE,
			BGFX_SAMPLER_U_MIRROR,
			BGFX_SAMPLER_U_CLAMP,
			BGFX_SAMPLER_U_BORDER
		};

		static const uint32_t bgfxWrapV[] =
		{
			BGFX_TEXTURE_NONE,
			BGFX_SAMPLER_V_MIRROR,
			BGFX_SAMPLER_V_CLAMP,
			BGFX_SAMPLER_V_BORDER
		};

		static const uint32_t bgfxWrapW[] =
		{
			BGFX_TEXTURE_NONE,
			BGFX_SAMPLER_W_MIRROR,
			BGFX_SAMPLER_W_CLAMP,
			BGFX_SAMPLER_W_BORDER
		};

		static const uint32_t bgfxFilterMode[] =
		{
			BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT, // FILTER_NEAREST
			BGFX_SAMPLER_MIP_POINT, // FILTER_BILINEAR
			BGFX_TEXTURE_NONE, // FILTER_TRILINEAR
			BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC, // FILTER_ANISOTROPIC
			BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT, // FILTER_NEAREST_ANISOTROPIC
			BGFX_TEXTURE_NONE // FILTER_DEFAULT
		};

		Texture::Texture(Core::Context* context) :
			GPUObject(),
			Resource(context)
		{
			mipsToSkip_[0] = 2;
			mipsToSkip_[1] = 1;
			mipsToSkip_[2] = 0;
		}

		Texture::~Texture()
		{
		}

		void Texture::SetNumLevels(UInt32 levels)
		{
			if (usage_ > TEXTURE_RENDERTARGET)
				requestedLevels_ = 1;
			else
				requestedLevels_ = levels;
		}

		void Texture::SetNumLayers(UInt32 layers)
		{
			layers_ = layers;
		}

		void Texture::SetFilterMode(TextureFilterMode mode)
		{
			filterMode_ = mode;
		}

		void Texture::SetAddressMode(TextureCoordinate coord, TextureAddressMode mode)
		{
			addressModes_[coord] = mode;
		}

		void Texture::SetShadowCompare(bool compare)
		{
			shadowCompare_ = compare;
		}

		Int32 Texture::GetWidth() const
		{
			return width_;
		}

		Int32 Texture::GetHeight() const
		{
			return height_;
		}

		Int32 Texture::GetDepth() const
		{
			return depth_;
		}

		Int32 Texture::GetLevelWidth(UInt32 level) const
		{
			if (level > levels_)
			{
				return 0;
			}
			return Math::Max(width_ >> level, 1);
		}

		Int32 Texture::GetLevelHeight(UInt32 level) const
		{
			if (level > levels_)
			{
				return 0;
			}
			return Math::Max(height_ >> level, 1);
		}

		Int32 Texture::GetLevelDepth(UInt32 level) const
		{
			if (level > levels_)
			{
				return 0;
			}
			return Math::Max(depth_ >> level, 1);
		}

		UInt32 Texture::GetRowDataSize(Int32 width) const
		{
			//switch (format_)
			//{
			//case DXGI_FORMAT_R8_UNORM:
			//case DXGI_FORMAT_A8_UNORM:
			//	return (UInt32)width;

			//case DXGI_FORMAT_R8G8_UNORM:
			//case DXGI_FORMAT_R16_UNORM:
			//case DXGI_FORMAT_R16_FLOAT:
			//case DXGI_FORMAT_R16_TYPELESS:
			//	return (UInt32)(width * 2);

			//case DXGI_FORMAT_R8G8B8A8_UNORM:
			//case DXGI_FORMAT_R16G16_UNORM:
			//case DXGI_FORMAT_R16G16_FLOAT:
			//case DXGI_FORMAT_R32_FLOAT:
			//case DXGI_FORMAT_R24G8_TYPELESS:
			//case DXGI_FORMAT_R32_TYPELESS:
			//	return (UInt32)(width * 4);

			//case DXGI_FORMAT_R16G16B16A16_UNORM:
			//case DXGI_FORMAT_R16G16B16A16_FLOAT:
			//	return (UInt32)(width * 8);

			//case DXGI_FORMAT_R32G32B32A32_FLOAT:
			//	return (UInt32)(width * 16);

			//case DXGI_FORMAT_BC1_UNORM:
			//	return (UInt32)(((width + 3) >> 2) * 8);

			//case DXGI_FORMAT_BC2_UNORM:
			//case DXGI_FORMAT_BC3_UNORM:
			//	return (UInt32)(((width + 3) >> 2) * 16);

			//default:
			//	return 0;
			//}
			return 0;
		}

		bool Texture::IsCompressed() const
		{
			// return format_ == DXGI_FORMAT_BC1_UNORM || format_ == DXGI_FORMAT_BC2_UNORM || format_ == DXGI_FORMAT_BC3_UNORM;
			return bimg::isCompressed((bimg::TextureFormat::Enum)format_);
		}

		UInt32 Texture::GetComponents() const
		{
			if (!width_ || IsCompressed())
			{
				return 0;
			}

			return GetRowDataSize(width_) / width_;
		}

		UInt64 Texture::GetFlags() const
		{
			uint64_t flags =
				bgfxWrapU[addressModes_[0]] |
				bgfxWrapV[addressModes_[1]] |
				bgfxWrapW[addressModes_[2]] |
				bgfxFilterMode[filterMode_];

			if (sRGB_)
				flags |= BGFX_TEXTURE_SRGB;

			if ((usage_ == TEXTURE_RENDERTARGET) || (usage_ == TEXTURE_DEPTHSTENCIL))
				flags |= BGFX_TEXTURE_RT;

			if (shadowCompare_)
				flags |= BGFX_SAMPLER_COMPARE_LEQUAL;

			if (multiSample_ > 1)
			{
				// flags |= BGFX_TEXTURE_MSAA_SAMPLE;
				switch (multiSample_)
				{
				case 2:
					flags |= BGFX_TEXTURE_RT_MSAA_X2;
					break;
				case 4:
					flags |= BGFX_TEXTURE_RT_MSAA_X4;
					break;
				case 8:
					flags |= BGFX_TEXTURE_RT_MSAA_X8;
					break;
				case 16:
					flags |= BGFX_TEXTURE_RT_MSAA_X16;
					break;
				default:
					break;
				}
			}

			return flags;
		}

		void Texture::Release()
		{
			ResetHandler(GPUHandler::INVALID);
		}

		void Texture::SetInternalData(const bgfx::Memory* data)
		{
			internalData_ = data;
		}

		UInt32 Texture::CheckMaxLevels(Int32 width, Int32 height, UInt32 requestedLevels)
		{
			UInt32 maxLevels = 1;
			while (width > 1 || height > 1)
			{
				++maxLevels;
				width = width > 1 ? (width >> 1u) : 1;
				height = height > 1 ? (height >> 1u) : 1;
			}

			if (!requestedLevels || maxLevels < requestedLevels)
			{
				return maxLevels;
			}

			return requestedLevels;
		}

		UInt32 Texture::CheckMaxLevels(Int32 width, Int32 height, Int32 depth, UInt32 requestedLevels)
		{
			UInt32 maxLevels = 1;
			while (width > 1 || height > 1)
			{
				++maxLevels;
				width = width > 1 ? (width >> 1u) : 1;
				height = height > 1 ? (height >> 1u) : 1;
				depth = depth > 1 ? (depth >> 1u) : 1;
			}

			if (!requestedLevels || maxLevels < requestedLevels)
			{
				return maxLevels;
			}

			return requestedLevels;
		}

		UInt32 Texture::GetSRGBFormat(UInt32 format)
		{
			//if (format == DXGI_FORMAT_R8G8B8A8_UNORM)
			//	return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			//else if (format == DXGI_FORMAT_BC1_UNORM)
			//	return DXGI_FORMAT_BC1_UNORM_SRGB;
			//else if (format == DXGI_FORMAT_BC2_UNORM)
			//	return DXGI_FORMAT_BC2_UNORM_SRGB;
			//else if (format == DXGI_FORMAT_BC3_UNORM)
			//	return DXGI_FORMAT_BC3_UNORM_SRGB;
			return format;
		}

		UInt32 Texture::GetDSVFormat(UInt32 format)
		{
			//if (format == DXGI_FORMAT_R24G8_TYPELESS)
			//	return DXGI_FORMAT_D24_UNORM_S8_UINT;
			//else if (format == DXGI_FORMAT_R16_TYPELESS)
			//	return DXGI_FORMAT_D16_UNORM;
			//else if (format == DXGI_FORMAT_R32_TYPELESS)
			//	return DXGI_FORMAT_D32_FLOAT;
			return format;
		}

		UInt32 Texture::GetSRVFormat(UInt32 format)
		{
			//if (format == DXGI_FORMAT_R24G8_TYPELESS)
			//	return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			//else if (format == DXGI_FORMAT_R16_TYPELESS)
			//	return DXGI_FORMAT_R16_UNORM;
			//else if (format == DXGI_FORMAT_R32_TYPELESS)
			//	return DXGI_FORMAT_R32_FLOAT;
			return format;
		}

		void Texture::ImageReleaseCb(void* _ptr, void* _userData)
		{
			BX_UNUSED(_ptr);
			bimg::ImageContainer* imageContainer = (bimg::ImageContainer*)_userData;
			bimg::imageFree(imageContainer);
		}

		void Texture::Initialize()
		{
		}

		bool Texture::IsValid()
		{
			return GetHandler();
		}
	}
}
