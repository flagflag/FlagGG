#include "Texture2D.h"
#include "RenderEngine.h"

#include "DDS/DDSTextureLoader.h"

#include <d3dx11.h>

namespace FlagGG
{
	namespace Graphics
	{
		Texture2D::Texture2D(const std::wstring& texturePath) :
			texturePath_(texturePath)
		{
		}

		bool Texture2D::Create(ID3D11Resource*& resource, ID3D11ShaderResourceView*& resourceView)
		{
#ifndef USE_DDS
			if (texturePath_.empty())
			{
				D3D11_TEXTURE2D_DESC textureDesc;
				memset(&textureDesc, 0, sizeof(textureDesc));
				textureDesc.MipLevels = 1;
				textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				textureDesc.SampleDesc.Count = 1;
				textureDesc.SampleDesc.Quality = 0;
				textureDesc.Usage = D3D11_USAGE_DEFAULT; // D3D11_USAGE_DYNAMIC D3D11_USAGE_DEFAULT
				textureDesc.ArraySize = 1;
				textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
				textureDesc.CPUAccessFlags = 0;
				textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
				
				// 目前还不知道动态大小的图片要怎么搞。。。。
				textureDesc.Width = 100;
				textureDesc.Height = 100;

				HRESULT hr = RenderEngine::GetDevice()->CreateTexture2D(&textureDesc, nullptr, (ID3D11Texture2D**)&resource);
				if (hr != 0)
				{
					puts("CreateTexture2D failed.");

					SAFE_RELEASE(resource);

					return false;
				}
			}
			else
			{
				D3DX11_IMAGE_LOAD_INFO loadInfo;
				memset(&loadInfo, 0, sizeof(loadInfo));
				loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				loadInfo.MipLevels = D3DX11_DEFAULT;
				loadInfo.MipFilter = D3DX11_FILTER_LINEAR;

				HRESULT hr = D3DX11CreateTextureFromFileW(
					RenderEngine::GetDevice(),
					texturePath_.data(),
					&loadInfo,
					nullptr,
					&resource,
					nullptr
					);
				if (hr != 0)
				{
					puts("D3DX11CreateTextureFromFile failed.");

					SAFE_RELEASE(resource);

					return false;
				}
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			memset(&shaderResourceViewDesc, 0, sizeof(shaderResourceViewDesc));
			shaderResourceViewDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			HRESULT hr = RenderEngine::GetDevice()->CreateShaderResourceView(resource, &shaderResourceViewDesc, &resourceView);
			if (hr != 0)
			{
				puts("CreateShaderResourceView failed.");

				SAFE_RELEASE(resourceView);

				return false;
			}
#else
			HRESULT hr = DirectX::CreateDDSTextureFromFile(
				RenderEngine::GetDevice(),
				RenderEngine::GetDeviceContext(),
				texturePath_.data(),
				&resource,
				&resourceView
				);
			if (hr != 0)
			{
				puts("CreateDDSTextureFromFile failed.");

				return false;
			}
#endif

			return true;
		}

		void Texture2D::SetData(int x, int y, unsigned width, unsigned height, const void* data)
		{
			ID3D11Texture2D* resource = GetObject<ID3D11Texture2D>();
			if (resource)
			{
				// 下面这段瞎几把写的，还不知道用法是啥
				D3D11_BOX destBox;
				destBox.left = x;
				destBox.top = y;
				destBox.right = x + width;
				destBox.bottom = y + height;
				destBox.front = 0;
				destBox.back = 1;
				RenderEngine::GetDeviceContext()->UpdateSubresource(resource, 1, &destBox, data, width * 16, 0);
			}
		}
	}
}