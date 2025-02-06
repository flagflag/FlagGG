#include "UIView.h"
#include "WebUISystem.h"

#include <Graphics/Window.h>
#include <Graphics/Texture2D.h>
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
	, lastMouseKey_(ultralight::MouseEvent::kButton_None)
{
	renderTexture_ = new Texture2D();
	renderTexture_->SetSize(width, height, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
	renderSurface_ = renderTexture_->GetRenderSurface();

	CreateView(width, height);
}

UIView::UIView(Window* window)
	: webView_(nullptr)
	, backgroundTransparency_(1.0f)
	, lastMouseKey_(ultralight::MouseEvent::kButton_None)
{
	window_ = window;
	viewport_ = window->GetViewport();
	renderSurface_ = viewport_->GetRenderTarget();

	CreateView(window->GetWidth(), window->GetHeight());

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

void UIView::LoadUrl(const String& url)
{
	if (webView_)
	{
		webView_->LoadURL(url.CString());
		htmlUrl_ = url;
		htmlContent_ = nullptr;
	}
}

void UIView::LoadHTML(const String& html)
{
	if (webView_)
	{
		webView_->LoadHTML(html.CString());
		htmlUrl_ = nullptr;
		htmlContent_ = html;
	}
}

void UIView::SetBackgroundTransparency(Real transparency)
{
	if (backgroundTransparency_ != transparency)
	{
		backgroundTransparency_ = transparency;
		if (!webView_->needs_paint())
		{
			if (htmlUrl_.Length())
				webView_->LoadURL(htmlUrl_.CString());
			else
				webView_->LoadHTML(htmlContent_.CString());
		}
	}
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

	if (window_)
	{
		GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_DOWN, UIView::OnKeyDown, this));
		GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_UP, UIView::OnKeyUp, this));
		GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_CHAR, UIView::OnChar, this));
		GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_DOWN, UIView::OnMouseDown, this));
		GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_UP, UIView::OnMouseUp, this));
		GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_MOVE, UIView::OnMouseMove, this));

		GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_WHEEL, UIView::OnMouseWheel, this));
		GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::SET_FOCUS, UIView::OnSetFocus, this));
		GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::KILL_FOCUS, UIView::OnKillFocus, this));
	}
}

void UIView::GatherRenderUITrees(Vector<RenderUITree>& renderUITrees)
{
	if (webView_->needs_paint())
	{
		RenderUITree& item = renderUITrees.EmplaceBack();
		item.uiRoot_ = webViewAdaptor_;
		item.renderSurface_ = renderSurface_;
		auto rect = renderTexture_ ? IntRect(0, 0, renderTexture_->GetWidth(), renderTexture_->GetHeight()) : viewport_->GetSize();
		item.viewport_ = Rect(rect.Left(), rect.Top(), rect.Right(), rect.Bottom());
		item.manualRender_ = !renderTexture_;
		item.webKitRendering_ = true;
		item.backgroundTransparency_ = backgroundTransparency_;
	}
}

void UIView::OnKeyDown(KeyState* keyState, UInt32 keyCode)
{
	if (keyState->GetSender() == window_)
	{
		auto* rawMsgParam = keyState->GetRawMsgParam();
		ultralight::KeyEvent evt(ultralight::KeyEvent::kType_RawKeyDown, rawMsgParam->GetRawParam(1), rawMsgParam->GetRawParam(2), false);
		webView_->FireKeyEvent(evt);
	}
}

void UIView::OnKeyUp(KeyState* keyState, UInt32 keyCode)
{
	if (keyState->GetSender() == window_)
	{
		auto* rawMsgParam = keyState->GetRawMsgParam();
		ultralight::KeyEvent evt(ultralight::KeyEvent::kType_KeyUp, rawMsgParam->GetRawParam(1), rawMsgParam->GetRawParam(2), false);
		webView_->FireKeyEvent(evt);
	}
}

void UIView::OnChar(KeyState* keyState, UInt32 keyCode)
{
	if (keyState->GetSender() == window_)
	{
		auto* rawMsgParam = keyState->GetRawMsgParam();
		ultralight::KeyEvent evt(ultralight::KeyEvent::kType_Char, rawMsgParam->GetRawParam(1), rawMsgParam->GetRawParam(2), false);
		webView_->FireKeyEvent(evt);
	}
}

void UIView::OnMouseDown(KeyState* keyState, MouseKey mouseKey, const IntVector2& mousePos)
{
	if (keyState->GetSender() == window_)
	{
		ultralight::MouseEvent::Button button = ultralight::MouseEvent::kButton_None;
		if (mouseKey == MOUSE_LEFT)
			button = ultralight::MouseEvent::kButton_Left;
		else if(mouseKey == MOUSE_MID)
			button = ultralight::MouseEvent::kButton_Middle;
		else if(mouseKey == MOUSE_RIGHT)
			button = ultralight::MouseEvent::kButton_Right;

		webView_->FireMouseEvent(ultralight::MouseEvent{ ultralight::MouseEvent::kType_MouseDown, mousePos.x_, mousePos.y_, button });

		lastMouseKey_ = button;
	}
}

void UIView::OnMouseUp(KeyState* keyState, MouseKey mouseKey, const IntVector2& mousePos)
{
	if (keyState->GetSender() == window_)
	{
		ultralight::MouseEvent::Button button = ultralight::MouseEvent::kButton_None;
		if (mouseKey == MOUSE_LEFT)
			button = ultralight::MouseEvent::kButton_Left;
		else if (mouseKey == MOUSE_MID)
			button = ultralight::MouseEvent::kButton_Middle;
		else if (mouseKey == MOUSE_RIGHT)
			button = ultralight::MouseEvent::kButton_Right;

		webView_->FireMouseEvent(ultralight::MouseEvent{ ultralight::MouseEvent::kType_MouseUp, mousePos.x_, mousePos.y_, button });

		lastMouseKey_ = ultralight::MouseEvent::kButton_None;
	}
}

void UIView::OnMouseMove(KeyState* keyState, const IntVector2& mousePos, const Vector2& delta)
{
	if (keyState->GetSender() == window_)
	{
		webView_->FireMouseEvent(ultralight::MouseEvent{ ultralight::MouseEvent::kType_MouseMoved, mousePos.x_, mousePos.y_, ultralight::MouseEvent::Button(lastMouseKey_) });
	}
}

void UIView::OnMouseWheel(KeyState* keyState, Int32 delta)
{
	if (keyState->GetSender() == window_)
	{
		webView_->FireScrollEvent(ultralight::ScrollEvent{ ultralight::ScrollEvent::kType_ScrollByPixel, 0, delta });
	}
}

void UIView::OnSetFocus(Window* window)
{
	if (window == window_)
	{
		webView_->Focus();
	}
}

void UIView::OnKillFocus(Window* window)
{
	if (window == window_)
	{
		webView_->Unfocus();
	}
}

}