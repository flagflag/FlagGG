#include "UIView.h"
#include "WebUISystem.h"

#include <Graphics/Window.h>
#include <Core/EventDefine.h>
#include <Core/EventManager.h>
#include <UI/UIEvents.h>
#include <Ultralight/Renderer.h>

#include <UltralightCore2/RenderContext.h>

namespace FlagGG
{

class WebViewUIElement : public UIElement
{
	OBJECT_OVERRIDE(WebViewUIElement, UIElement);
public:
	WebViewUIElement(ultralight::View* view)
		: webView_(view)
	{

	}

	// 更新UI批次数据（由渲染底层调用）
	void UpdateBatch(VertexVector* vertexVector, Vector<SharedPtr<Batch>>& uiBatches) override
	{
		if (webView_->needs_paint())
		{
			GetSubsystem<ultralight::RenderContext>()->SetCallStackRenderBatch(vertexVector, &uiBatches);

			GetSubsystem<WebUISystem>()->GetRenderer()->RenderOnly(&webView_, 1);

			GetSubsystem<ultralight::RenderContext>()->SetCallStackRenderBatch(nullptr, nullptr);
		}
	}

private:
	ultralight::View* webView_;
};

UIView::UIView(UInt32 width, UInt32 height)
	: webView_(nullptr)
	, backgroundTransparency_(1.0f)
{
	CreateView(width, height);
}

UIView::UIView(Window* window)
	: webView_(nullptr)
	, backgroundTransparency_(1.0f)
{
	CreateView(window->GetWidth(), window->GetHeight());

	window_ = window;
	viewport_ = window->GetViewport();
	renderSurface_ = viewport_->GetRenderTarget();

	window_->SetUIElementRoot(webViewAdaptor_);
}

UIView::~UIView()
{
	if (webView_)
	{
		webView_->Release();
		webView_ = nullptr;
	}
}

void UIView::LoadHTML(const String& html)
{
	if (webView_)
	{
		webView_->LoadHTML(html.CString());
	}
}

void UIView::SetBackgroundTransparency(Real transparency)
{
	backgroundTransparency_ = transparency;
}

void UIView::CreateView(UInt32 width, UInt32 height)
{
	if (webView_)
	{
		webView_->Release();
		webView_ = nullptr;
	}

	ultralight::ViewConfig viewConfig;
	viewConfig.initial_device_scale = 1.0;
	viewConfig.is_accelerated = true;
	auto view = GetSubsystem<WebUISystem>()->GetRenderer()->CreateView(width, height, viewConfig, nullptr);
	webView_ = view.get();
	webView_->AddRef();

	webViewAdaptor_ = new WebViewUIElement(webView_);

	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(UIEvent::GATHER_RENDER_UI_TREE, UIView::GatherRenderUITrees, this));
}

void UIView::GatherRenderUITrees(Vector<RenderUITree>& renderUITrees)
{
	if (webView_->needs_paint())
	{
		RenderUITree& item = renderUITrees.EmplaceBack();
		item.uiRoot_ = webViewAdaptor_;
		item.renderSurface_ = renderSurface_;
		auto rect = viewport_->GetSize();
		item.viewport_ = Rect(rect.Left(), rect.Top(), rect.Right(), rect.Bottom());
		item.manualRender_ = true;
		item.webKitRendering_ = true;
		item.backgroundTransparency_ = backgroundTransparency_;
	}
}

}