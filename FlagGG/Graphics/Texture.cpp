#include "Texture.h"
#include "RenderEngine.h"

namespace FlagGG
{
	namespace Graphics
	{
		Texture::~Texture()
		{
			SAFE_RELEASE(sampler_);
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
				puts("CreateSamplerState failed.");

				SAFE_RELEASE(sampler_);

				return;
			}

			ID3D11Resource* resource = nullptr;
			if (Create(resource, shaderResourceView_))
			{
				ResetHandler(resource);
			}
		}

		bool Texture::IsValid()
		{
			return GetHandler() != nullptr && shaderResourceView_ != nullptr && sampler_ != nullptr;
		}
	}
}
