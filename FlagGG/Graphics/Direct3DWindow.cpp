#include "Direct3DWindow.h"
#include "Allocator/SmartMemory.hpp"

#include <d3dcompiler.h>

#include <xnamath.h>

#include <stdio.h>
#include <string>
#include <fstream>

#define SAFE_RELEASE(p) \
	if ((p)) \
	{ \
		((IUnknown*)p)->Release();  p = nullptr; \
	}

namespace FlagGG
{
	namespace TestGraphics
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

		Direct3DWindow::Direct3DWindow(HWND parentHandler, unsigned x, unsigned y, unsigned width, unsigned height) :
			parentHandler_(parentHandler)
		{
			StartupWindow(x, y, width, height);

			CreateDevice(width, height, true);

			UpdateSwapChain(width, height);
		}

		void Direct3DWindow::StartupWindow(unsigned x, unsigned y, unsigned width, unsigned height)
		{
			if (handler_)
			{
				puts("window had startup.");
				return;
			}

			handler_ = CreateWindowExW(
				0,
				WindowDevice::className_,
				L"",
				WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
				x,
				y,
				width,
				height,
				parentHandler_,
				nullptr,
				nullptr,
				this
				);

			if (!handler_)
			{
				printf("create window faile, error code(%d).\n", GetLastError());
			}
		}

		void Direct3DWindow::CreateDevice(unsigned width, unsigned height, bool sRGB)
		{
			if (!device_)
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

			if (swapChain_)
			{
				SAFE_RELEASE(swapChain_);
			}

			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			memset(&swapChainDesc, 0, sizeof(swapChainDesc));
			swapChainDesc.BufferCount = 1;
			swapChainDesc.BufferDesc.Width = width;
			swapChainDesc.BufferDesc.Height = height;
			swapChainDesc.BufferDesc.Format = sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.OutputWindow = handler_;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.Windowed = TRUE;
			swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			IDXGIDevice* dxgiDevice = nullptr;
			device_->QueryInterface(IID_IDXGIDevice, (void**)&dxgiDevice);
			IDXGIAdapter* dxgiAdapter = nullptr;
			dxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&dxgiAdapter);
			IDXGIFactory* dxgiFactory = nullptr;
			dxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&dxgiFactory);
			HRESULT hr = dxgiFactory->CreateSwapChain(device_, &swapChainDesc, &swapChain_);
			dxgiFactory->MakeWindowAssociation(handler_, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);

			SAFE_RELEASE(dxgiDevice);
			SAFE_RELEASE(dxgiAdapter);
			SAFE_RELEASE(dxgiFactory);

			if (hr != 0)
			{
				puts("CreateSwapChain failed.");

				SAFE_RELEASE(swapChain_);

				return;
			}
		}

		void Direct3DWindow::UpdateSwapChain(unsigned width, unsigned height)
		{
			ID3D11RenderTargetView* nullView = nullptr;
			deviceContext_->OMSetRenderTargets(1, &nullView, nullptr);

			swapChain_->ResizeBuffers(1, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

			// 创建默认的RenderTargetView
			ID3D11Texture2D* backbufferTexture;
			HRESULT hr = swapChain_->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backbufferTexture);
			if (hr != 0)
			{
				puts("swap chain GetBuffer failed.");

				SAFE_RELEASE(backbufferTexture);

				return;
			}

			hr = device_->CreateRenderTargetView(backbufferTexture, nullptr, &renderTargetView_);
			if (hr != 0)
			{
				puts("CreateRenderTargetView failed.");

				SAFE_RELEASE(renderTargetView_);

				return;
			}

			isRenderTargetDirty_ = true;

			//// 创建默认的深度模板texture和view
			//	D3D11_TEXTURE2D_DESC depthDesc;
			//	memset(&depthDesc, 0, sizeof(depthDesc));
			//	depthDesc.Width = width;
			//	depthDesc.Height = height;
			//	depthDesc.MipLevels = 1;
			//	depthDesc.ArraySize = 1;
			//	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			//	depthDesc.SampleDesc.Count = 1;
			//	depthDesc.SampleDesc.Quality = 0;
			//	depthDesc.Usage = D3D11_USAGE_DEFAULT;
			//	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			//	depthDesc.CPUAccessFlags = 0;
			//	depthDesc.MiscFlags = 0;
			//	hr = device_->CreateTexture2D(&depthDesc, nullptr, &depthTexture_);
			//	if (hr != 0)
			//	{
			//		puts("CreateTexture2D failed.");
			//
			//		SAFE_RELEASE(depthTexture_);
			//
			//		return;
			//	}
			//
			//	hr = device_->CreateDepthStencilView(depthTexture_, nullptr, &depthStencilView_);
			//	if (hr != 0)
			//	{
			//		puts("CreateDepthStencilView failed.");
			//
			//		SAFE_RELEASE(depthStencilView_);
			//
			//		return;
			//	}

			RECT rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = width;
			rect.bottom = height;
			SetViewport(rect);
		}

		void Direct3DWindow::SetViewport(const RECT& rect)
		{
			D3D11_VIEWPORT d3d11Viewport;
			d3d11Viewport.TopLeftX = rect.left;
			d3d11Viewport.TopLeftY = rect.top;
			d3d11Viewport.Width = rect.right - rect.left;
			d3d11Viewport.Height = rect.bottom - rect.top;
			d3d11Viewport.MinDepth = 0.0f;
			d3d11Viewport.MaxDepth = 1.0f;

			deviceContext_->RSSetViewports(1, &d3d11Viewport);
		}

		void Direct3DWindow::Draw()
		{
			if (isRenderTargetDirty_)
			{
				deviceContext_->OMSetRenderTargets(1, &renderTargetView_, nullptr);

				isRenderTargetDirty_ = false;
			}

			if (isD3d11BufferDirty_)
			{
				unsigned vertexSize = 12;
				unsigned vertexOffset = 0;

				deviceContext_->IASetInputLayout(inputLayout_);

				deviceContext_->IASetVertexBuffers(0, 1, &d3d11Buffer_, &vertexSize, &vertexOffset);

				isD3d11BufferDirty_ = false;
			}

			if (isTextureDirty_)
			{
				//deviceContext_->VSSetSamplers(0, 1, &sampler_);
				//deviceContext_->PSSetSamplers(0, 1, &sampler_);

				isTextureDirty_ = false;
			}

			if (isShaderDirty_)
			{
				deviceContext_->VSSetShader(vertexShader_, nullptr, 0);
				deviceContext_->PSSetShader(pixelShader_, nullptr, 0);

				isShaderDirty_ = false;
			}

			float color[] = { 0.5, 0.5f, 0.5f, 1.0f };
			deviceContext_->ClearRenderTargetView(renderTargetView_, color);

			deviceContext_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			deviceContext_->Draw(3, 0);

			swapChain_->Present(0, 0);
		}

		void Direct3DWindow::Update()
		{
			Draw();

			// Logic Tick
		}

		void Direct3DWindow::Show()
		{
			::ShowWindow(handler_, SW_SHOW);
		}

		void Direct3DWindow::Hide()
		{
			::ShowWindow(handler_, SW_HIDE);
		}

		void WriteData(void* data)
		{
			float* dest = (float*)data;

			dest[0] = 0.5f;
			dest[1] = 0.5f;
			dest[2] = 0.5f;

			dest[3] = 0.5f;
			dest[4] = -0.5f;
			dest[5] = 0.5f;

			dest[6] = -0.5f;
			dest[7] = -0.5f;
			dest[8] = 0.5f;
		}

		struct VertexPos
		{
			XMFLOAT3 pos;
		};

		void Direct3DWindow::AddDefaultGraphics()
		{
			CreateShader();

			CreateSamper();

			D3D11_BUFFER_DESC bufferDesc;
			memset(&bufferDesc, 0, sizeof(bufferDesc));
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //D3D11_USAGE_DEFAULT
			bufferDesc.ByteWidth = 12 * 3;

			HRESULT hr = device_->CreateBuffer(&bufferDesc, nullptr, (ID3D11Buffer**)&d3d11Buffer_);
			if (hr != 0)
			{
				puts("Device CreateBuffer failed.");

				SAFE_RELEASE(d3d11Buffer_);

				return;
			}

			void* data = nullptr;

			D3D11_MAPPED_SUBRESOURCE mappedData;
			mappedData.pData = nullptr;

			hr = deviceContext_->Map(d3d11Buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData); //D3D11_MAP_WRITE
			if (hr != 0 || !mappedData.pData)
			{
				puts("Map Data failed.");

				return;
			}

			WriteData(mappedData.pData);

			deviceContext_->Unmap(d3d11Buffer_, 0);

			isD3d11BufferDirty_ = true;
		}

		static ID3DBlob* GetShaderCode(const std::wstring& path, int type)
		{
			char* entryPoint = nullptr;
			char* profile = nullptr;
			//unsigned flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
			unsigned flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
			flags |= D3DCOMPILE_DEBUG;
#endif

			if (type == 0) // VS
			{
				entryPoint = "VS";
				profile = "vs_4_0";
			}
			else // PS
			{
				entryPoint = "PS";
				profile = "ps_4_0";
				//flags |= D3DCOMPILE_PREFER_FLOW_CONTROL;
			}

			std::ifstream stream;
			stream.open(path, std::ios::in | std::ios::binary | std::ios::ate);
			if (!stream.is_open())
			{
				return nullptr;
			}

			std::streamoff sourceCodeSize = stream.tellg();
			//std::unique_ptr<char[]> sourceCode(new char[sourceCodeSize]);
			Allocator::SmartMemory<char> sourceCode(sourceCodeSize);

			stream.seekg(0, std::ios::beg);
			stream.read(sourceCode.get(), sourceCodeSize);

			ID3DBlob* shaderCode = nullptr;
			ID3DBlob* errorMsgs = nullptr;

			D3D_SHADER_MACRO macro[] = {
				{
					"D3D11",
					"1"
				},
				{
					"COMPILEVS",
					"1"
				},
				{
					"MAXBONES",
					"100"
				},
				{
					nullptr,
					nullptr
				}
			};

			HRESULT hr = D3DCompile(
				sourceCode.get(),
				sourceCodeSize,
				nullptr,//"Test",
				nullptr,//macro,
				nullptr,
				entryPoint,
				profile,
				flags,
				0,
				&shaderCode,
				&errorMsgs
			);
			if (hr != 0)
			{
				puts("D3DCompile failed.");

				if (errorMsgs)
				{
					printf("%s\n", errorMsgs->GetBufferPointer());

					SAFE_RELEASE(errorMsgs);
				}

				return nullptr;
			}

			ID3DBlob* strippedCode = nullptr;
			hr = D3DStripShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(),
				D3DCOMPILER_STRIP_REFLECTION_DATA | D3DCOMPILER_STRIP_DEBUG_INFO | D3DCOMPILER_STRIP_TEST_BLOBS, &strippedCode);
			if (hr != 0)
			{
				puts("D3DStripShader failed.");

				return nullptr;
			}

			SAFE_RELEASE(shaderCode);
			SAFE_RELEASE(errorMsgs);

			return strippedCode;
		}

		void Direct3DWindow::CreateShader()
		{		
			ID3DBlob* VSCode = GetShaderCode(L"E:\\Shader\\test.hlsl", 0);
			if (VSCode)
			{
				HRESULT hr = device_->CreateVertexShader(
					VSCode->GetBufferPointer(),
					VSCode->GetBufferSize(),
					nullptr,
					(ID3D11VertexShader**)&vertexShader_
					);
				if (hr != 0)
				{
					puts("CreateVertexShader failed.");

					SAFE_RELEASE(vertexShader_);
					SAFE_RELEASE(VSCode);

					return;
				}
			}

			D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
			{
				{
					"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,

					0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
				}
			};

			unsigned int totalLayoutElements = ARRAYSIZE(solidColorLayout);

			HRESULT hr = device_->CreateInputLayout(
				solidColorLayout,
				totalLayoutElements,
				VSCode->GetBufferPointer(),
				VSCode->GetBufferSize(),
				&inputLayout_
			);
			if (hr != 0)
			{
				puts("CreateInputLayout failed.");

				return;
			}

			SAFE_RELEASE(VSCode);
					
			ID3DBlob* PSCode = GetShaderCode(L"E:\\Shader\\test.hlsl", 1);
			if (PSCode)
			{
				HRESULT hr = device_->CreatePixelShader(
					PSCode->GetBufferPointer(),
					PSCode->GetBufferSize(),
					nullptr,
					(ID3D11PixelShader**)&pixelShader_
				);
				if (hr != 0)
				{
					puts("CreatePixelShader failed.");

					SAFE_RELEASE(pixelShader_);
					SAFE_RELEASE(PSCode);

					return;
				}
			}
			SAFE_RELEASE(PSCode);

			isShaderDirty_ = true;
		}

		void Direct3DWindow::CreateSamper()
		{
			D3D11_SAMPLER_DESC samplerDesc;
			memset(&samplerDesc, 0, sizeof(samplerDesc));
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MaxAnisotropy = 4;
			samplerDesc.MinLOD = -INFINITY;
			samplerDesc.MaxLOD = INFINITY;
			samplerDesc.BorderColor[0] = 0;
			samplerDesc.BorderColor[1] = 0xff;
			samplerDesc.BorderColor[2] = 0;
			samplerDesc.BorderColor[3] = 0;

			HRESULT hr = device_->CreateSamplerState(&samplerDesc, (ID3D11SamplerState**)&sampler_);
			if (hr != 0)
			{
				puts("CreateSamplerState failed.");

				SAFE_RELEASE(sampler_);

				return;
			}

			isTextureDirty_ = true;
		}
	}
}
