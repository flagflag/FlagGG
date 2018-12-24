#include "WinViewport.h"
#include "RenderEngine.h"
#include "Texture.h"

#include <windows.h>

namespace FlagGG
{
	namespace Graphics
	{
		const wchar_t* WindowDevice::className_ = L"Custom D3D11 Window";

		static LRESULT APIENTRY StaticWndProc(HWND handler, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_CREATE:
				puts("create window success.");
				break;
			}

			return DefWindowProc(handler, message, wParam, lParam);
		}

		void WindowDevice::Initialize()
		{
			WNDCLASSEXW cls;

			memset(&cls, 0, sizeof(cls));
			cls.cbSize = sizeof(cls);
			cls.style = CS_DBLCLKS | CS_OWNDC;
			cls.lpfnWndProc = StaticWndProc;
			cls.hInstance = nullptr;
			cls.hIcon = nullptr;
			cls.lpszClassName = className_;
			cls.hIconSm = nullptr;

			RegisterClassExW(&cls);
		}

		void WindowDevice::Uninitialize()
		{
			UnregisterClassW(className_, nullptr);
		}

		void WindowDevice::Update()
		{
			MSG msg;
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		WinViewport::WinViewport(void* parentWindow, unsigned x, unsigned y, unsigned width, unsigned height) :
			parentWindow_(parentWindow)
		{
			window = CreateWindowExW(
				0,
				WindowDevice::className_,
				L"",
				WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
				x,
				y,
				width,
				height,
				(HWND)parentWindow_,
				nullptr,
				nullptr,
				this
				);

			if (!window)
			{
				printf("CreateWindow failed, error code(%d).\n", GetLastError());
			}
		}

		unsigned WinViewport::GetWidth()
		{
			RECT rect;
			::GetWindowRect((HWND)window, &rect);
			return rect.right - rect.left;
		}

		unsigned WinViewport::GetHeight()
		{
			RECT rect;
			::GetWindowRect((HWND)window, &rect);
			return rect.bottom - rect.top;
		}

		void WinViewport::Resize(unsigned width, unsigned height)
		{
			::SetWindowPos((HWND)window, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

			DXGI_MODE_DESC desc;
			memset(&desc, 0, sizeof(desc));
			desc.Width = width;
			desc.Height = height;
			desc.Format = sRGB_ ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
			GetObject<IDXGISwapChain>()->ResizeTarget(&desc);

			GetObject<IDXGISwapChain>()->ResizeBuffers(
				1, width, height,
				sRGB_ ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
				);
		}

		void* WinViewport::GetWindow()
		{
			return window;
		}

		void WinViewport::Show()
		{
			::ShowWindow((HWND)window, SW_SHOW);
		}

		void WinViewport::Hide()
		{
			::ShowWindow((HWND)window, SW_HIDE);
		}

		void WinViewport::UpdateVertexData(const unsigned char* vertexs, unsigned vertexSize, unsigned vertexCount)
		{
			if (vertexSize != vertexSize_ || vertexCount != vertexCount_)
			{
				SAFE_RELEASE(d3d11Buffer_);
			}

			if (!d3d11Buffer_)
			{
				D3D11_BUFFER_DESC bufferDesc;
				memset(&bufferDesc, 0, sizeof(bufferDesc));
				bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //D3D11_USAGE_DEFAULT
				bufferDesc.ByteWidth = vertexSize * vertexCount;

				HRESULT hr = RenderEngine::GetDevice()->CreateBuffer(&bufferDesc, nullptr, (ID3D11Buffer**)&d3d11Buffer_);
				if (hr != 0)
				{
					puts("Device CreateBuffer failed.");

					SAFE_RELEASE(d3d11Buffer_);

					return;
				}
			}

			D3D11_MAPPED_SUBRESOURCE mappedData;
			mappedData.pData = nullptr;

			HRESULT hr = RenderEngine::GetDeviceContext()->Map(d3d11Buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData); //D3D11_MAP_WRITE
			if (hr != 0 || !mappedData.pData)
			{
				puts("Map Data failed.");

				return;
			}

			memcpy(mappedData.pData, vertexs, vertexSize * vertexCount);

			RenderEngine::GetDeviceContext()->Unmap(d3d11Buffer_, 0);
		}

		void WinViewport::Render(const Batch* batch)
		{
			if (!batch) return;

			RenderEngine::Update();

			ID3D11DeviceContext* deviceContext = RenderEngine::GetDeviceContext();

			ID3D11RenderTargetView* renderTargetView = GetRenderTarget()->GetObject<ID3D11RenderTargetView>();
			deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
	
			unsigned vertexSize = batch->GetVertexSize();
			unsigned vertexCount = batch->GetVertexCount();
			unsigned vertexOffset = 0;
			if (vertexCount > 0)
			{
				UpdateVertexData(&(*batch->GetVertexs())[0], vertexSize, vertexCount);
			}
			deviceContext->IASetVertexBuffers(0, 1, &d3d11Buffer_, &vertexSize, &vertexOffset);

			//deviceContext->VSSetSamplers(0, 1, &batch.GetTexture()->sampler_);		
			deviceContext->PSSetShaderResources(0, 1, &batch->GetTexture()->shaderResourceView_);
			deviceContext->PSSetSamplers(0, 1, &batch->GetTexture()->sampler_);

			deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);

			float color[] = { 0.5, 0.5f, 0.5f, 1.0f };
			deviceContext->ClearRenderTargetView(renderTargetView, color);

			deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			deviceContext->Draw(vertexCount, 0);
			GetObject<IDXGISwapChain>()->Present(0, 0);
		}
	}
}
