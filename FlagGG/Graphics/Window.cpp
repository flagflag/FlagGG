#include "Graphics/Window.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture.h"
#include "Log.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

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
				FLAGGG_LOG_ERROR("create window_ success.");
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

		void WindowDevice::Render()
		{
			for (auto& window_ : recivers_)
			{
				window_->Render();
			}
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
			window_(nullptr),
			parentWindow_(parentWindow),
			rect_(rect),
			context_(context),
			input_(context->GetVariable<Core::Input>("input"))
		{
			Create(parentWindow, rect);

			::GetCursorPos(&mousePos_);
		}

		Window::~Window()
		{
		}

		bool Window::Create(void* parentWindow, const Math::IntRect& rect)
		{
			window_ = CreateWindowExW(
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

			if (!window_)
			{
				FLAGGG_LOG_ERROR("CreateWindow failed, error code({}).", GetLastError());
				return false;
			}

			bgfx::FrameBufferHandle handle = bgfx::createFrameBuffer(window_, rect.Width(), rect.Height());
			ResetHandler(handle);

			RenderSurface* renderTargetSurface = new RenderSurface(nullptr);
			renderTargetSurface->ResetHandler(handle);

			RenderSurface* depthStencilSurface = new RenderSurface(nullptr);
			depthStencilSurface->ResetHandler(handle);

			viewport_ = new Viewport();
			viewport_->Resize(Math::IntRect(0, 0, rect.Width(), rect.Height()));
			viewport_->SetRenderTarget(renderTargetSurface);
			viewport_->SetDepthStencil(depthStencilSurface);

			bgfx::PlatformData data;
			data.nwh = window_;
			bgfx::setPlatformData(data);

			CreateSwapChain();
			UpdateSwapChain(rect.Width(), rect.Height());

			return true;
		}

		void Window::CreateSwapChain()
		{
		}

		void Window::UpdateSwapChain(UInt32 width, UInt32 height)
		{
		}

		UInt32 Window::GetWidth()
		{
			RECT rect;
			::GetWindowRect((HWND)window_, &rect);
			return rect.right - rect.left;
		}

		UInt32 Window::GetHeight()
		{
			RECT rect;
			::GetWindowRect((HWND)window_, &rect);
			return rect.bottom - rect.top;
		}

		Math::IntVector2 Window::GetMousePos() const
		{
			POINT point;
			::GetCursorPos(&point);
			::ScreenToClient((HWND)window_, &point);
			return Math::IntVector2(point.x, point.y);
		}

		bool Window::IsForegroundWindow() const
		{
			return ::GetForegroundWindow() == window_;
		}

		void Window::Resize(UInt32 width, UInt32 height)
		{
			::SetWindowPos((HWND)window_, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

			UpdateSwapChain(width, height);

			rect_.right_ = rect_.left_ + width;
			rect_.bottom_ = rect_.top_ + height;
		}

		void* Window::GetWindow()
		{
			return window_;
		}

		void Window::Show()
		{
			::ShowWindow((HWND)window_, SW_SHOW);
		}

		void Window::Hide()
		{
			::ShowWindow((HWND)window_, SW_HIDE);
		}

		void Window::Render()
		{
			RenderEngine::Instance()->Render(viewport_);

			// RenderEngine::Instance()->RenderBatch(viewport_);
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

				if (!input_->IsMouseShow())
				{
					mousePos_.x = GetWidth() / 2;
					mousePos_.y = GetHeight() / 2;
					::ClientToScreen((HWND)window_, &mousePos_);
					::SetCursorPos(mousePos_.x, mousePos_.y);
				}

				break;

			case WM_CLOSE:
				context_->SendEvent<Core::Application::WINDOW_CLOSE_HANDLER>(Core::Application::WINDOW_CLOSE, window_);

				break;
			}
		}

		bool Window::IsValid()
		{
			return window_ && GetHandler();
		}

		void Window::Initialize()
		{
			// do nothing
		}
	}
}
