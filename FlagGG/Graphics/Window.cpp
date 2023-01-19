#include "Graphics/Window.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture.h"
#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/GfxSwapChain.h"
#include "Log.h"

#include <windows.h>
#include <memory>

namespace FlagGG
{

const wchar_t* WindowDevice::className_ = L"Custom D3D11 Window";

Vector<SharedPtr<Window>> WindowDevice::recivers_;

PODVector<DefferedMessage> WindowDevice::defferedMsgs_;

static LRESULT APIENTRY StaticWndProc(HWND handler, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		FLAGGG_LOG_ERROR("create window success.");
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
	for (auto& window : recivers_)
	{
		window->Render();
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

	recivers_.Push(SharedPtr<Window>(wv));
}

void WindowDevice::UnregisterWinMessage(Window* wv)
{
	recivers_.Remove(SharedPtr<Window>(wv));
}


Window::Window(Context* context, void* parentWindow, const IntRect& rect) :
	window_(nullptr),
	parentWindow_(parentWindow),
	rect_(rect),
	context_(context),
	input_(context->GetVariable<Input>("input"))
{
	Create(parentWindow, rect);

	::GetCursorPos(&mousePos_);
}

Window::~Window()
{

}

bool Window::Create(void* parentWindow, const IntRect& rect)
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

	CreateSwapChain();
	UpdateSwapChain(rect.Width(), rect.Height());

	return true;
}

void Window::CreateSwapChain()
{
	gfxSwapChain_ = GfxDevice::GetDevice()->CreateSwapChain(this);
}

void Window::UpdateSwapChain(UInt32 width, UInt32 height)
{
	gfxSwapChain_->Resize(width, height);

	viewport_.Reset();
	viewport_ = new Viewport();
	viewport_->Resize(IntRect(0, 0, width, height));
	viewport_->SetRenderTarget(gfxSwapChain_->GetRenderTarget());
	viewport_->SetDepthStencil(gfxSwapChain_->GetDepthStencil());
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

IntVector2 Window::GetMousePos() const
{
	POINT point;
	::GetCursorPos(&point);
	::ScreenToClient((HWND)window_, &point);
	return IntVector2(point.x, point.y);
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

	RenderEngine::Instance()->RenderBatch(viewport_);

	gfxSwapChain_->Present();
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
			MouseKey key;
			if (message == WM_LBUTTONDOWN) key = MOUSE_LEFT;
			else if (message == WM_RBUTTONDOWN) key = MOUSE_RIGHT;
			else if (message == WM_MBUTTONDOWN) key = MOUSE_MID;
					
			input_->OnMouseDown(nullptr, key);
		}
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		{
			MouseKey key;
			if (message == WM_LBUTTONUP) key = MOUSE_LEFT;
			else if (message == WM_RBUTTONUP) key = MOUSE_RIGHT;
			else if (message == WM_MBUTTONUP) key = MOUSE_MID;

			input_->OnMouseUp(nullptr, key);
		}
		break;
			
	case WM_MOUSEMOVE:
		static POINT mousePos;

		::GetCursorPos(&mousePos);
		input_->OnMouseMove(nullptr,
			Vector2(mousePos.x - mousePos_.x, mousePos.y - mousePos_.y));

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
		context_->SendEvent<Application::WINDOW_CLOSE_HANDLER>(window_);

		break;
	}
}

}
