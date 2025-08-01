#include "Core/EventManager.h"
#include "Graphics/Window.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture.h"
#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/GfxSwapChain.h"
#include "UI/UISystem.h"
#include "UI/UIEvents.h"
#include "Log.h"

#if PLATFORM_WINDOWS
#include <windows.h>
#include <windowsx.h>
#endif
#include <memory>

namespace FlagGG
{

#if PLATFORM_WINDOWS
struct DefferedMessage
{
	HWND handler_;
	UINT message_;
	WPARAM wParam_;
	LPARAM lParam_;
};
#endif

const wchar_t* WindowDevice::className_ = L"FlagGGWindow";

Vector<SharedPtr<Window>> WindowDevice::recivers_;

#if PLATFORM_WINDOWS
PODVector<DefferedMessage> WindowDevice::defferedMsgs_;

static LRESULT APIENTRY StaticWndProc(HWND handler, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		FLAGGG_LOG_INFO("create window success.");
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
#endif

void WindowDevice::Initialize()
{
#if PLATFORM_WINDOWS
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
#endif
}

void WindowDevice::Uninitialize()
{
#if PLATFORM_WINDOWS
	UnregisterClassW(className_, nullptr);
#endif
}

void WindowDevice::Update()
{
#if PLATFORM_WINDOWS
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
			if ((*rec)->GetHandle() == msg->handler_)
			{
				(*rec)->WinProc(msg->message_, msg->wParam_, msg->lParam_);
			}
		}
	}

	defferedMsgs_.Clear();
#endif
}

void WindowDevice::RenderUpdate()
{
	for (auto& window : recivers_)
	{
		window->RenderUpdate();
	}
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


Window::Window(void* parentWindow, const IntRect& rect) :
	window_(nullptr),
	rect_(rect)
{
	Create(parentWindow, rect);

#if PLATFORM_WINDOWS
	::GetCursorPos(&mousePos_);
#endif
}

Window::~Window()
{

}

bool Window::Create(void* parentWindow, const IntRect& rect)
{
#if PLATFORM_WINDOWS
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
#endif

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
#if PLATFORM_WINDOWS
	RECT rect;
	::GetWindowRect((HWND)window_, &rect);
	return rect.right - rect.left;
#else
	return 0u;
#endif
}

UInt32 Window::GetHeight()
{
#if PLATFORM_WINDOWS
	RECT rect;
	::GetWindowRect((HWND)window_, &rect);
	return rect.bottom - rect.top;
#else
	return 0u;
#endif
}

IntVector2 Window::GetMousePos() const
{
#if PLATFORM_WINDOWS
	POINT point;
	::GetCursorPos(&point);
	::ScreenToClient((HWND)window_, &point);
	return IntVector2(point.x, point.y);
#else
	return IntVector2::ZERO;
#endif
}

bool Window::IsForegroundWindow() const
{
#if PLATFORM_WINDOWS
	return ::GetForegroundWindow() == window_;
#else
	return false;
#endif
}

void Window::Resize(UInt32 width, UInt32 height)
{
#if PLATFORM_WINDOWS
	::SetWindowPos((HWND)window_, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
#endif

	UpdateSwapChain(width, height);

	rect_.right_ = rect_.left_ + width;
	rect_.bottom_ = rect_.top_ + height;
}

void Window::Show()
{
#if PLATFORM_WINDOWS
	::ShowWindow((HWND)window_, SW_SHOW);
#endif
}

void Window::Hide()
{
#if PLATFORM_WINDOWS
	::ShowWindow((HWND)window_, SW_HIDE);
#endif
}

void Window::CreateUIElement()
{
	if (!uiRoot_)
	{
		uiRoot_ = new UIElement();

		GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(UIEvent::GATHER_RENDER_UI_TREE, Window::GatherRenderUITrees, this));
	}
}

void Window::SetUIElementRoot(UIElement* uiRoot)
{
	if (!uiRoot_)
	{
		uiRoot_ = uiRoot;
	}
}

void Window::RenderUpdate()
{
	RenderEngine::Instance().RenderUpdate(viewport_);
}

void Window::Render()
{
	RenderEngine::Instance().Render(viewport_);

	GetSubsystem<UISystem>()->Render(uiRoot_);

	gfxSwapChain_->Present();
}

Viewport* Window::GetViewport() const
{
	return viewport_;
}

void Window::GatherRenderUITrees(Vector<RenderUITree>& renderUITrees)
{
	RenderUITree& item = renderUITrees.EmplaceBack();
	item.uiRoot_ = uiRoot_;
	item.renderSurface_ = viewport_->GetRenderTarget();
	auto rect = viewport_->GetSize();
	item.viewport_ = Rect(rect.Left(), rect.Top(), rect.Right(), rect.Bottom());
	item.manualRender_ = true;
}

#if PLATFORM_WINDOWS
void Window::WinProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	auto* input = GetSubsystem<Input>();

	RawMsgParam rawMsgParam;
	rawMsgParam.Clear();
	rawMsgParam.AddRawParam((UInt64)message);
	rawMsgParam.AddRawParam((UInt64)wParam);
	rawMsgParam.AddRawParam((UInt64)lParam);

	KeyState keyState(this, false, false, false, &rawMsgParam);

	switch (message)
	{
	case WM_KEYDOWN:
		input->OnKeyDown(&keyState, wParam);
		break;

	case WM_KEYUP:
		input->OnKeyUp(&keyState, wParam);
		break;

	case WM_CHAR:
		input->OnChar(&keyState, wParam);
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		SetCapture((HWND)window_);
		input->OnMouseDown(&keyState, MOUSE_LEFT, IntVector2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		break;

	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		SetCapture((HWND)window_);
		input->OnMouseDown(&keyState, MOUSE_RIGHT, IntVector2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		break;

	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
		SetCapture((HWND)window_);
		input->OnMouseDown(&keyState, MOUSE_MID, IntVector2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		input->OnMouseUp(&keyState, MOUSE_LEFT, IntVector2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		break;

	case WM_RBUTTONUP:
		ReleaseCapture();
		input->OnMouseUp(&keyState, MOUSE_RIGHT, IntVector2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		break;

	case WM_MBUTTONUP:
		ReleaseCapture();
		input->OnMouseUp(&keyState, MOUSE_MID, IntVector2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		break;
			
	case WM_MOUSEMOVE:
		static POINT mousePos;

		::GetCursorPos(&mousePos);
		input->OnMouseMove(&keyState, IntVector2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)),
			Vector2(mousePos.x - mousePos_.x, mousePos.y - mousePos_.y));

		mousePos_ = mousePos;

		if (!input->IsMouseShow())
		{
			mousePos_.x = GetWidth() / 2;
			mousePos_.y = GetHeight() / 2;
			::ClientToScreen((HWND)window_, &mousePos_);
			::SetCursorPos(mousePos_.x, mousePos_.y);
		}

		break;

	case WM_MOUSEWHEEL:
		input->OnMouseWheel(&keyState, GET_WHEEL_DELTA_WPARAM(wParam));
		break;

	case WM_SETFOCUS:
		input->OnSetFocus(this);
		break;

	case WM_KILLFOCUS:
		input->OnKillFocus(this);
		break;

	case WM_CLOSE:
		GetSubsystem<EventManager>()->SendEvent<Application::WINDOW_CLOSE_HANDLER>(window_);
		break;
	}
}
#endif

}
