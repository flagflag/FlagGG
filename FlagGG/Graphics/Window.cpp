#include "Graphics/Window.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture.h"
#include "Log.h"

#include <windows.h>
#include <memory>

namespace FlagGG
{
	namespace Graphics
	{
		const wchar_t* WindowDevice::className_ = L"Custom D3D11 Window";

		Container::Vector<Container::SharedPtr<Window>> WindowDevice::recivers_;

		Container::PODVector<DefferedMessage> WindowDevice::defferedMsgs_;

		static LRESULT APIENTRY StaticWndProc(HWND handler, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_CREATE:
				puts("create window success.");
				break;
			}

			DefferedMessage msg;
			msg.handler_ = handler;
			msg.message_ = message;
			msg.wParam_ = wParam;
			msg.lParam_ = lParam;
			WindowDevice::defferedMsgs_.Push(msg);

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

			for (auto msg = defferedMsgs_.Begin(); msg != defferedMsgs_.End(); ++msg)
			{
				for (auto rec = recivers_.Begin(); rec != recivers_.End(); ++rec)
				{
					if ((*rec)->GetWindow() == msg->handler_)
					{
						(*rec)->WinProc(msg->message_, msg->wParam_, msg->lParam_);
					}
				}
			}

			defferedMsgs_.Clear();
		}

		void WindowDevice::RegisterWinMessage(Window* wv)
		{
			for (const auto& reciver : recivers_)
			{
				if (reciver.Get() == wv)
				{
					return;
				}
			}

			recivers_.Push(Container::SharedPtr<Window>(wv));
		}

		void WindowDevice::UnregisterWinMessage(Window* wv)
		{
			recivers_.Remove(Container::SharedPtr<Window>(wv));
		}


		Window::Window(Core::Context* context, void* parentWindow, const Math::IntRect& rect) :
			window(nullptr),
			parentWindow_(parentWindow),
			rect_(rect),
			input_(context->GetVariable<Core::Input>("input"))
		{

			Create(parentWindow, rect);

			::GetCursorPos(&mousePos_);
		}

		bool Window::Create(void* parentWindow, const Math::IntRect& rect)
		{
			window = CreateWindowExW(
				0,
				WindowDevice::className_,
				L"",
				WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
				rect.left_,
				rect.top_,
				rect.Width(),
				rect.Height(),
				(HWND)parentWindow,
				nullptr,
				nullptr,
				this
				);

			if (!window)
			{
				FLAGGG_LOG_ERROR("CreateWindow failed, error code(%d).", GetLastError());
				return false;
			}

			CreateSwapChain();
			UpdateSwapChain(rect.Width(), rect.Height());

			return true;
		}

		void Window::CreateSwapChain()
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
			swapChainDesc.SampleDesc.Count = (UINT)multiSample_;
			swapChainDesc.SampleDesc.Quality = RenderEngine::GetMultiSampleQuality(swapChainDesc.BufferDesc.Format, multiSample_);
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
				FLAGGG_LOG_ERROR("CreateSwapChain failed.");

				SAFE_RELEASE(swapChain);

				return;
			}

			ResetHandler(swapChain);
		}

		void Window::UpdateSwapChain(uint32_t width, uint32_t height)
		{
			GetObject<IDXGISwapChain>()->ResizeBuffers(1, (UINT)width, (UINT)height,
				DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

			viewport_.Reset();

			ID3D11Texture2D* backBuffer;
			HRESULT hr = GetObject<IDXGISwapChain>()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBuffer);
			if (FAILED(hr))
			{
				SAFE_RELEASE(backBuffer);
				FLAGGG_LOG_ERROR("Faild to get backbuffer from SwapChain.");
				return;
			}

			ID3D11RenderTargetView* renderTargetView;
			hr = RenderEngine::GetDevice()->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
			if (FAILED(hr))
			{
				SAFE_RELEASE(renderTargetView);
				FLAGGG_LOG_ERROR("Failed to create RenderTargetView for SwapChain.");
				return;
			}
			Container::SharedPtr<RenderSurface> renderTarget(new RenderSurface());
			renderTarget->ResetHandler(renderTargetView);

			D3D11_TEXTURE2D_DESC depthDesc;
			memset(&depthDesc, 0, sizeof depthDesc);
			depthDesc.Width = (UINT)width;
			depthDesc.Height = (UINT)height;
			depthDesc.MipLevels = 1;
			depthDesc.ArraySize = 1;
			depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthDesc.SampleDesc.Count = (UINT)multiSample_;
			depthDesc.SampleDesc.Quality = RenderEngine::GetMultiSampleQuality(depthDesc.Format, multiSample_);
			depthDesc.Usage = D3D11_USAGE_DEFAULT;
			depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthDesc.CPUAccessFlags = 0;
			depthDesc.MiscFlags = 0;

			hr = RenderEngine::GetDevice()->CreateTexture2D(&depthDesc, nullptr, &depthTexture_);
			if (FAILED(hr))
			{
				SAFE_RELEASE(depthTexture_);
				FLAGGG_LOG_ERROR("Failed to create depth texture.");
				return;
			}

			ID3D11DepthStencilView* depthStencilView;
			hr = RenderEngine::GetDevice()->CreateDepthStencilView(depthTexture_, nullptr, &depthStencilView);
			if (FAILED(hr))
			{
				SAFE_RELEASE(depthStencilView);
				FLAGGG_LOG_ERROR("Failed to create depth-stencil view.");
				return;
			}
			Container::SharedPtr<RenderSurface> depthStencil(new RenderSurface());
			depthStencil->ResetHandler(depthStencilView);

			viewport_ = new Viewport();
			viewport_->Resize(Math::IntRect(0, 0, width, height));
			viewport_->SetRenderTarget(renderTarget);
			viewport_->SetDepthStencil(depthStencil);
		}

		uint32_t Window::GetWidth()
		{
			RECT rect;
			::GetWindowRect((HWND)window, &rect);
			return rect.right - rect.left;
		}

		uint32_t Window::GetHeight()
		{
			RECT rect;
			::GetWindowRect((HWND)window, &rect);
			return rect.bottom - rect.top;
		}

		void Window::Resize(uint32_t width, uint32_t height)
		{
			::SetWindowPos((HWND)window, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

			UpdateSwapChain(width, height);

			rect_.right_ = rect_.left_ + width;
			rect_.bottom_ = rect_.top_ + height;
		}

		void* Window::GetWindow()
		{
			return window;
		}

		void Window::Show()
		{
			::ShowWindow((HWND)window, SW_SHOW);
		}

		void Window::Hide()
		{
			::ShowWindow((HWND)window, SW_HIDE);
		}

		void Window::Render()
		{
			RenderEngine::Render(viewport_);

			GetObject<IDXGISwapChain>()->Present(0, 0);
		}

		Viewport* Window::GetViewport() const
		{
			return viewport_;
		}

		void Window::WinProc(UINT message, WPARAM wParam, LPARAM lParam)
		{
			if (!input_) return;

			switch (message)
			{
			case WM_KEYDOWN:
				input_->OnKeyDown(nullptr, wParam);

				break;

			case WM_KEYUP:
				input_->OnKeyUp(nullptr, wParam);

				break;

			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
				{
					Core::MouseKey key;
					if (message == WM_LBUTTONDOWN) key = Core::MOUSE_LEFT;
					else if (message == WM_RBUTTONDOWN) key = Core::MOUSE_RIGHT;
					else if (message == WM_MBUTTONDOWN) key = Core::MOUSE_MID;
					
					input_->OnMouseDown(nullptr, key);
				}
				break;

			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
				{
					Core::MouseKey key;
					if (message == WM_LBUTTONUP) key = Core::MOUSE_LEFT;
					else if (message == WM_RBUTTONUP) key = Core::MOUSE_RIGHT;
					else if (message == WM_MBUTTONUP) key = Core::MOUSE_MID;

					input_->OnMouseUp(nullptr, key);
				}
				break;
			
			case WM_MOUSEMOVE:
				static POINT mousePos;

				::GetCursorPos(&mousePos);
				input_->OnMouseMove(nullptr,
					Math::Vector2(mousePos.x - mousePos_.x, mousePos.y - mousePos_.y));

				mousePos_ = mousePos;

				break;
			}
		}

		bool Window::IsValid()
		{
			return window && GetHandler();
		}

		void Window::Initialize()
		{
			// do nothing
		}
	}
}
