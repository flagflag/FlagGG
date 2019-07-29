#include "Graphics/Texture.h"
#include "Graphics/RenderEngine.h"
#include "Math/Math.h"
#include "Log.h"

namespace FlagGG
{
	namespace Graphics
	{
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
			SAFE_RELEASE(sampler_);
		}

		void Texture::SetNumLevels(uint32_t levels)
		{
			if (usage_ > TEXTURE_RENDERTARGET)
				requestedLevels_ = 1;
			else
				requestedLevels_ = levels;
		}

		int32_t Texture::GetWidth() const
		{
			return width_;
		}

		int32_t Texture::GetHeight() const
		{
			return height_;
		}

		int32_t Texture::GetDepth() const
		{
			return depth_;
		}

		int32_t Texture::GetLevelWidth(uint32_t level) const
		{
			if (level > levels_)
			{
				return 0;
			}
			return Math::Max(width_ >> level, 1);
		}

		int32_t Texture::GetLevelHeight(uint32_t level) const
		{
			if (level > levels_)
			{
				return 0;
			}
			return Math::Max(height_ >> level, 1);
		}

		int32_t Texture::GetLevelDepth(uint32_t level) const
		{
			if (level > levels_)
			{
				return 0;
			}
			return Math::Max(depth_ >> level, 1);
		}

		uint32_t Texture::GetRowDataSize(int32_t width) const
		{
			switch (format_)
			{
			case DXGI_FORMAT_R8_UNORM:
			case DXGI_FORMAT_A8_UNORM:
				return (uint32_t)width;

			case DXGI_FORMAT_R8G8_UNORM:
			case DXGI_FORMAT_R16_UNORM:
			case DXGI_FORMAT_R16_FLOAT:
			case DXGI_FORMAT_R16_TYPELESS:
				return (uint32_t)(width * 2);

			case DXGI_FORMAT_R8G8B8A8_UNORM:
			case DXGI_FORMAT_R16G16_UNORM:
			case DXGI_FORMAT_R16G16_FLOAT:
			case DXGI_FORMAT_R32_FLOAT:
			case DXGI_FORMAT_R24G8_TYPELESS:
			case DXGI_FORMAT_R32_TYPELESS:
				return (uint32_t)(width * 4);

			case DXGI_FORMAT_R16G16B16A16_UNORM:
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
				return (uint32_t)(width * 8);

			case DXGI_FORMAT_R32G32B32A32_FLOAT:
				return (uint32_t)(width * 16);

			case DXGI_FORMAT_BC1_UNORM:
				return (uint32_t)(((width + 3) >> 2) * 8);

			case DXGI_FORMAT_BC2_UNORM:
			case DXGI_FORMAT_BC3_UNORM:
				return (uint32_t)(((width + 3) >> 2) * 16);

			default:
				return 0;
			}
		}

		bool Texture::IsCompressed() const
		{
			return format_ == DXGI_FORMAT_BC1_UNORM || format_ == DXGI_FORMAT_BC2_UNORM || format_ == DXGI_FORMAT_BC3_UNORM;
		}

		RenderSurface* Texture::GetRenderSurface() const
		{
			return renderSurface_;
		}

		uint32_t Texture::GetComponents() const
		{
			if (!width_ || IsCompressed())
			{
				return 0;
			}

			return GetRowDataSize(width_) / width_;
		}

		uint32_t Texture::CheckMaxLevels(int32_t width, int32_t height, uint32_t requestedLevels)
		{
			uint32_t maxLevels = 1;
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

		uint32_t Texture::GetSRGBFormat(uint32_t format)
		{
			if (format == DXGI_FORMAT_R8G8B8A8_UNORM)
				return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			else if (format == DXGI_FORMAT_BC1_UNORM)
				return DXGI_FORMAT_BC1_UNORM_SRGB;
			else if (format == DXGI_FORMAT_BC2_UNORM)
				return DXGI_FORMAT_BC2_UNORM_SRGB;
			else if (format == DXGI_FORMAT_BC3_UNORM)
				return DXGI_FORMAT_BC3_UNORM_SRGB;
			return format;
		}

		uint32_t Texture::GetDSVFormat(uint32_t format)
		{
			if (format == DXGI_FORMAT_R24G8_TYPELESS)
				return DXGI_FORMAT_D24_UNORM_S8_UINT;
			else if (format == DXGI_FORMAT_R16_TYPELESS)
				return DXGI_FORMAT_D16_UNORM;
			else if (format == DXGI_FORMAT_R32_TYPELESS)
				return DXGI_FORMAT_D32_FLOAT;
			return format;
		}

		uint32_t Texture::GetSRVFormat(uint32_t format)
		{
			if (format == DXGI_FORMAT_R24G8_TYPELESS)
				return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			else if (format == DXGI_FORMAT_R16_TYPELESS)
				return DXGI_FORMAT_R16_UNORM;
			else if (format == DXGI_FORMAT_R32_TYPELESS)
				return DXGI_FORMAT_R32_FLOAT;
			return format;
		}

		void Texture::Initialize()
		{
			D3D11_SAMPLER_DESC samplerDesc;
			memset(&samplerDesc, 0, sizeof(samplerDesc));
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MaxAnisotropy = 4;
			samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

			HRESULT hr = RenderEngine::GetDevice()->CreateSamplerState(&samplerDesc, &sampler_);
			if (hr != 0)
			{
				FLAGGG_LOG_ERROR("CreateSamplerState failed.");

				SAFE_RELEASE(sampler_);

				return;
			}
		}

		bool Texture::IsValid()
		{
			return GetHandler() != nullptr && shaderResourceView_ != nullptr /*&& sampler_ != nullptr*/;
		}
	}
}
