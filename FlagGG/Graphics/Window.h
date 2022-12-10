#ifndef __WIN_VIEWPORT__
#define __WIN_VIEWPORT__

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

class FlagGG_API WindowDevice
{
public:
	static void Initialize();

	static void Uninitialize();

	static void Update();

	static void Render();

	static void RegisterWinMessage(Window* wv);

	static void UnregisterWinMessage(Window* wv);

	static const wchar_t* className_;

	static Vector<SharedPtr<Window>> recivers_;

	static PODVector<DefferedMessage> defferedMsgs_;
};

class FlagGG_API Window : public GPUObject, public RefCounted
{
public:
	Window(Context* context, void* parentWindow, const IntRect& rect);

	~Window() override;

	UInt32 GetWidth();

	UInt32 GetHeight();

	IntVector2 GetMousePos() const;

	bool IsForegroundWindow() const;

	void Resize(UInt32 width, UInt32 height);

	void* GetWindow();

	void Show();

	void Hide();

	void Render();

	Viewport* GetViewport() const;

	void WinProc(UINT message, WPARAM wParam, LPARAM lParam);

	bool IsValid() override;

protected:

	void Initialize() override;

	bool Create(void* parentWindow, const IntRect& rect);

	void CreateSwapChain();

	void UpdateSwapChain(UInt32 width, UInt32 height);

private:
	Context* context_;
	Input* input_;

	ID3D11Texture2D* depthTexture_{ nullptr };
	SharedPtr<Viewport> viewport_;

	UInt32 multiSample_{ 1 };

	bool sRGB_{ true };

	void* window{ nullptr };

	void* parentWindow_{ nullptr };

	IntRect rect_;

	UInt32 vertexSize_{ 0 };
	UInt32 vertexCount_{ 0 };

	POINT mousePos_;
};

}

#endif
