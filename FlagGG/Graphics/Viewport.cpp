#include "Viewport.h"
#include "RenderEngine.h"

namespace FlagGG
{
	namespace Graphics
	{
		Viewport::Viewport(Core::Context* context) :
			context_(context)
		{ }

		Viewport::~Viewport()
		{
			SAFE_RELEASE(depthStencialView_);
		}

		void Viewport::Initialize()
		{	
			CreateSwapChain();

			CreateRenderTarget();

			SetViewport();
		}

		bool Viewport::IsValid()
		{
			return GetHandler() != nullptr && renderTarget_ != nullptr && renderTarget_->IsValid();
		}

		RenderTarget* Viewport::GetRenderTarget()
		{
			return renderTarget_;
		}

		void Viewport::CreateSwapChain()
		{
			IDXGISwapChain* swapChain = nullptr;

			HWND handler_ = (HWND)GetWindow();

			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			memset(&swapChainDesc, 0, sizeof(swapChainDesc));
			swapChainDesc.BufferCount = 1;
			swapChainDesc.BufferDesc.Width = GetWidth();
			swapChainDesc.BufferDesc.Height = GetHeight();
			swapChainDesc.BufferDesc.Format = sRGB_ ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.OutputWindow = handler_;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.Windowed = TRUE;
			swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			IDXGIDevice* dxgiDevice = nullptr;
			RenderEngine::GetDevice()->QueryInterface(IID_IDXGIDevice, (void**)&dxgiDevice);
			IDXGIAdapter* dxgiAdapter = nullptr;
			dxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&dxgiAdapter);
			IDXGIFactory* dxgiFactory = nullptr;
			dxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&dxgiFactory);
			HRESULT hr = dxgiFactory->CreateSwapChain(RenderEngine::GetDevice(), &swapChainDesc, &swapChain);
			dxgiFactory->MakeWindowAssociation(handler_, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);

			SAFE_RELEASE(dxgiDevice);
			SAFE_RELEASE(dxgiAdapter);
			SAFE_RELEASE(dxgiFactory);

			if (hr != 0)
			{
				puts("CreateSwapChain failed.");

				SAFE_RELEASE(swapChain);

				return;
			}

			ResetHandler(swapChain);
		}

		void Viewport::CreateRenderTarget()
		{
			ID3D11Texture2D* backbufferTexture;
			HRESULT hr = GetObject<IDXGISwapChain>()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backbufferTexture);
			if (hr != 0)
			{
				puts("GetBuffer failed.");

				return;
			}

			renderTarget_ = new RenderTarget(backbufferTexture);
			renderTarget_->Initialize();
		}

		void Viewport::CreateDepthStencilView()
		{
			D3D11_TEXTURE2D_DESC depthStencilDesc;
			depthStencilDesc.Width = GetWidth();
			depthStencilDesc.Height = GetHeight();
			depthStencilDesc.MipLevels = 1;
			depthStencilDesc.ArraySize = 1;
			depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilDesc.SampleDesc.Count = 1;
			depthStencilDesc.SampleDesc.Quality = 0;
			depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
			depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; //°ó¶¨µ½OM
			depthStencilDesc.CPUAccessFlags = 0;
			depthStencilDesc.MiscFlags = 0;

			ID3D11Texture2D* depthStencilBuffer;
			HRESULT hr = RenderEngine::GetDevice()->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
			if (hr != 0)
			{
				puts("CreateTexture2D failed.");

				return;
			}

			RenderEngine::GetDevice()->CreateDepthStencilView(depthStencilBuffer, nullptr, &depthStencialView_);
		}

		void Viewport::SetViewport()
		{
			D3D11_VIEWPORT d3d11Viewport;
			d3d11Viewport.TopLeftX = 0;
			d3d11Viewport.TopLeftY = 0;
			d3d11Viewport.Width = GetWidth();
			d3d11Viewport.Height = GetHeight();
			d3d11Viewport.MinDepth = 0.0f;
			d3d11Viewport.MaxDepth = 1.0f;

			RenderEngine::GetDeviceContext()->RSSetViewports(1, &d3d11Viewport);
		}

		Camera* Viewport::GetCamera()
		{
			return camera_;
		}

		void Viewport::SetCamera(Camera* camera)
		{
			camera_ = camera;
		}
	}
}

