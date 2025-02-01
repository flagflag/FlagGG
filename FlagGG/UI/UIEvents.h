//
// UI事件和一些结构定义（前向声明解耦）
//

#pragma once

#include "Core/EventCore.h"
#include "GfxDevice/GfxRenderSurface.h"
#include "Math/Rect.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"
#include "UI/UIElement.h"

namespace FlagGG
{

struct RenderUITree
{
	// ui根节点
	SharedPtr<UIElement> uiRoot_;
	// 渲染surface
	SharedPtr<GfxRenderSurface> renderSurface_;
	// 渲染区域
	Rect viewport_;
	// 主动调用渲染
	bool manualRender_{};
	// 渲染webkit-ui
	bool webKitRendering_{};
	// 背景透明度（只有webkit有效）
	Real backgroundTransparency_{ 1.0f };
};

namespace UIEvent
{
	DEFINE_EVENT(GATHER_RENDER_UI_TREE, void(Vector<RenderUITree>& renderUITrees));
}

}
