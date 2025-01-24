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
};

namespace UIEvent
{
	DEFINE_EVENT(GATHER_RENDER_UI_TREE, void(Vector<RenderUITree>& renderUITrees));
}

}
