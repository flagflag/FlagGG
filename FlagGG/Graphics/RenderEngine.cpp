#include "RenderEngine.h"

namespace FlagGG
{
	namespace Graphics
	{
		ID3D11Device* RenderEngine::device_ = nullptr;

		ID3D11DeviceContext* RenderEngine::deviceContext_ = nullptr;

		void RenderEngine::CreateDevice()
		{
			HRESULT hr = D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				0,
				nullptr,
				0,
				D3D11_SDK_VERSION,
				&device_,
				nullptr,
				&deviceContext_
				);

			if (hr != 0)
			{
				puts("D3D11CreateDevice failed.");

				SAFE_RELEASE(device_);
				SAFE_RELEASE(deviceContext_);

				return;
			}
		}

		void RenderEngine::Initialize()
		{
			CreateDevice();
		}

		void RenderEngine::Uninitialize()
		{
			SAFE_RELEASE(device_);
			SAFE_RELEASE(deviceContext_);
		}

		ID3D11Device* RenderEngine::GetDevice()
		{
			return device_;
		}

		ID3D11DeviceContext* RenderEngine::GetDeviceContext()
		{
			return deviceContext_;
		}
	}
}