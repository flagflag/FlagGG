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
class UIElement;
struct RenderUITree;

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

	// 获取窗口宽度
	UInt32 GetWidth();

	// 获取窗口高度
	UInt32 GetHeight();

	// 获取在窗口坐标系下的鼠标坐标
	IntVector2 GetMousePos() const;

	// 判断窗口是否是最上层可见窗口
	bool IsForegroundWindow() const;

	// 改窗口大小
	void Resize(UInt32 width, UInt32 height);

	// 显示窗口
	void Show();

	// 隐藏窗口
	void Hide();

	// 创建ui根节点
	void CreateUIElement();

	// 设置ui根节点
	void SetUIElementRoot(UIElement* uiRoot);

	// 获取ui根节点
	UIElement* GetUIElement() { return uiRoot_; }

	// 窗口渲染更新
	void RenderUpdate();

	// 渲染窗口
	void Render();

	// 获取视口
	Viewport* GetViewport() const;

	// 获取交换链
	GfxSwapChain* GetSwapChain() { return gfxSwapChain_; }

	// 获取窗口句柄
	void* GetHandle() const { return window_; }

	// 收集ui渲染树（作为事件，由UISystem调用）
	void GatherRenderUITrees(Vector<RenderUITree>& renderUITrees);

	// 窗口事件
	void WinProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	bool Create(void* parentWindow, const IntRect& rect);

	void CreateSwapChain();

	void UpdateSwapChain(UInt32 width, UInt32 height);

private:
	// 视口
	SharedPtr<Viewport> viewport_;

	// 交换链
	SharedPtr<GfxSwapChain> gfxSwapChain_;

	// 窗口ui根节点
	SharedPtr<UIElement> uiRoot_;

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

