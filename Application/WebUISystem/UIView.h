//
// Web ui view
// 可以关联窗口或者作为RT独立渲染
//

#pragma once

#include <Core/Object.h>
#include <Container/Ptr.h>
#include <Container/Vector.h>
#include <Container/Str.h>

#include "WebUISystem/WebUISystemExport.h"

namespace ultralight
{

class View;

}

namespace FlagGG
{

class Window;
class Viewport;
class GfxRenderSurface;
class UIElement;
struct RenderUITree;

class WebUISystem_API UIView : public Object
{
	OBJECT_OVERRIDE(UIView, Object);
public:
	// 作为RT独立渲染
	UIView(UInt32 width, UInt32 height);

	// 关联窗口，之后跟随窗口resize
	UIView(Window* window);

	~UIView() override;

	// 加载html
	void LoadHTML(const String& html);

	// 设置背景透明度
	void SetBackgroundTransparency(Real transparency);

	// 获取渲染的RT
	GfxRenderSurface* GetRenderSurface() { return renderSurface_; }

	// 收集ui渲染树（作为事件，由UISystem调用）
	void GatherRenderUITrees(Vector<RenderUITree>& renderUITrees);

protected:
	void CreateView(UInt32 width, UInt32 height);

private:
	ultralight::View* webView_;

	Real backgroundTransparency_;

	SharedPtr<UIElement> webViewAdaptor_;

	SharedPtr<Window> window_;

	SharedPtr<Viewport> viewport_;

	SharedPtr<GfxRenderSurface> renderSurface_;
};

}
