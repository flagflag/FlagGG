#include "GfxSwapChainD3D11.h"
#include "GfxD3D11Defines.h"
#include "GfxDeviceD3D11.h"
#include "Graphics/Window.h"

namespace FlagGG
{

GfxSwapChainD3D11::GfxSwapChainD3D11(Window* window) :
	GfxSwapChain(window)
{
	HWND handler = (HWND)window->GetWindow();

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	memset(&swapChainDesc, 0, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = window->GetWidth();
	swapChainDesc.BufferDesc.Height = window->GetHeight();
	swapChainDesc.BufferDesc.Format = sRGB_ ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = handler;
	swapChainDesc.SampleDesc.Count = (UINT)multiSample_;
	swapChainDesc.SampleDesc.Quality = GfxDeviceD3D11::Instance()->GetMultiSampleQuality(swapChainDesc.BufferDesc.Format, multiSample_);
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGIDevice* dxgiDevice = nullptr;
	GfxDeviceD3D11::Instance()->GetD3D11Device()->QueryInterface(IID_IDXGIDevice, (void**)&dxgiDevice);
	IDXGIAdapter* dxgiAdapter = nullptr;
	dxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&dxgiAdapter);
	IDXGIFactory* dxgiFactory = nullptr;
	dxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&dxgiFactory);
	HRESULT hr = dxgiFactory->CreateSwapChain(GfxDeviceD3D11::Instance()->GetD3D11Device(), &swapChainDesc, &swapChain_);
	dxgiFactory->MakeWindowAssociation(handler, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);

	D3D11_SAFE_RELEASE(dxgiDevice);
	D3D11_SAFE_RELEASE(dxgiAdapter);
	D3D11_SAFE_RELEASE(dxgiFactory);

	if (hr != 0)
	{
		FLAGGG_LOG_ERROR("CreateSwapChain failed.");
		D3D11_SAFE_RELEASE(swapChain_);
	}
}

GfxSwapChainD3D11::~GfxSwapChainD3D11()
{
	D3D11_SAFE_RELEASE(swapChain_);
	D3D11_SAFE_RELEASE(depthTexture_);
}

void GfxSwapChainD3D11::Resize(UInt32 width, UInt32 height)
{
	swapChain_->ResizeBuffers(1, (UINT)width, (UINT)height,
		DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	ID3D11Texture2D* backBuffer;
	HRESULT hr = swapChain_->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBuffer);
	if (FAILED(hr))
	{
		D3D11_SAFE_RELEASE(backBuffer);
		FLAGGG_LOG_ERROR("Faild to get backbuffer from SwapChain.");
		return;
	}

	ID3D11RenderTargetView* renderTargetView;
	hr = GfxDeviceD3D11::Instance()->GetD3D11Device()->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
	if (FAILED(hr))
	{
		D3D11_SAFE_RELEASE(renderTargetView);
		FLAGGG_LOG_ERROR("Failed to create RenderTargetView for SwapChain.");
		return;
	}

	renderTarget_ = new GfxRenderSurfaceD3D11();
	renderTarget_->SetRenderTargetView(renderTargetView);

	D3D11_TEXTURE2D_DESC depthDesc;
	memset(&depthDesc, 0, sizeof depthDesc);
	depthDesc.Width = (UINT)width;
	depthDesc.Height = (UINT)height;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = (UINT)multiSample_;
	depthDesc.SampleDesc.Quality = GfxDeviceD3D11::Instance()->GetMultiSampleQuality(depthDesc.Format, multiSample_);
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	hr = GfxDeviceD3D11::Instance()->GetD3D11Device()->CreateTexture2D(&depthDesc, nullptr, &depthTexture_);
	if (FAILED(hr))
	{
		D3D11_SAFE_RELEASE(depthTexture_);
		FLAGGG_LOG_ERROR("Failed to create depth texture.");
		return;
	}

	ID3D11DepthStencilView* depthStencilView;
	hr = GfxDeviceD3D11::Instance()->GetD3D11Device()->CreateDepthStencilView(depthTexture_, nullptr, &depthStencilView);
	if (FAILED(hr))
	{
		D3D11_SAFE_RELEASE(depthStencilView);
		FLAGGG_LOG_ERROR("Failed to create depth-stencil view.");
		return;
	}

	depthStencil_ = new GfxRenderSurfaceD3D11();
	depthStencil_->SetDepthStencilView(depthStencilView);
}

void GfxSwapChainD3D11::Present()
{
	swapChain_->Present(0, 0);
}

}
