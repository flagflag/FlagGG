//
// 窗口
// 目前只支持了Windows平台
//

#pragma once

#include "Export.h"

#include "Graphics/Viewport.h"
#include "Graphics/Batch.h"
#include "Graphics/RenderContext.h"
#include "Core/DeviceEvent.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"

#include <stdint.h>

namespace FlagGG
{

struct DefferedMessage
{
	HWND handler_;
	UINT message_;
	WPARAM wParam_;
	LPARAM lParam_;
};

class Window;
class GfxSwapChain;

class FlagGG_API WindowDevice
{
public:
	static void Initialize();

	static void Uninitialize();

	static void Update();

	static void RenderUpdate();

	static void Render();

	static void RegisterWinMessage(Window* wv);

	static void UnregisterWinMessage(Window* wv);

	static const wchar_t* className_;

	static Vector<SharedPtr<Window>> recivers_;

	static PODVector<DefferedMessage> defferedMsgs_;
};

class FlagGG_API Window : public Object
{
	OBJECT_OVERRIDE(Window, Object);
public:
	Window(void* parentWindow, const IntRect& rect);

	~Window() override;

	UInt32 GetWidth();

	UInt32 GetHeight();

	IntVector2 GetMousePos() const;

	bool IsForegroundWindow() const;

	void Resize(UInt32 width, UInt32 height);

	void* GetWindow();

	void Show();

	void Hide();

	void RenderUpdate();

	void Render();

	Viewport* GetViewport() const;

	void WinProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	bool Create(void* parentWindow, const IntRect& rect);

	void CreateSwapChain();

	void UpdateSwapChain(UInt32 width, UInt32 height);

private:
	Input* input_;

	SharedPtr<Viewport> viewport_;

	SharedPtr<GfxSwapChain> gfxSwapChain_;

	UInt32 multiSample_{ 1 };

	bool sRGB_{ true };

	void* window_{ nullptr };

	void* parentWindow_{ nullptr };

	IntRect rect_;

	UInt32 vertexSize_{ 0 };
	UInt32 vertexCount_{ 0 };

	POINT mousePos_;
};

}

