#include "UIElement.h"
#include "Math/Rect.h"
#include "Math/Color.h"
#include "Graphics/Texture2D.h"

#include <yoga/Yoga.h>
#include <yoga/YGNode.h>

namespace FlagGG
{

UIElement::UIElement()
	: layoutNode_(YGNodeNew())
{
	YGNodeStyleSetFlexDirection(layoutNode_, YGFlexDirectionColumn);
}

UIElement::~UIElement()
{
	if (layoutNode_)
	{
		YGNodeFree(layoutNode_);
		layoutNode_ = nullptr;
	}
}

void UIElement::SetPosition(const Vector2& position)
{
	YGNodeStyleSetPosition(layoutNode_, YGEdgeLeft, position.x_);
	YGNodeStyleSetPosition(layoutNode_, YGEdgeTop, position.y_);
}

void UIElement::SetWidth(float width)
{
	YGNodeStyleSetWidth(layoutNode_, width);
}

void UIElement::SetHeight(float height)
{
	YGNodeStyleSetHeight(layoutNode_, height);
}

void UIElement::SetWidthPercent(float width)
{
	YGNodeStyleSetWidthPercent(layoutNode_, width);
}

void UIElement::SetHeightPercent(float height)
{
	YGNodeStyleSetHeightPercent(layoutNode_, height);
}

void UIElement::SetColor(const Color& color)
{
	colors_[UInt32(UIQuadCorner::TOPLEFT)]     = color;
	colors_[UInt32(UIQuadCorner::TOPRIGHT)]    = color;
	colors_[UInt32(UIQuadCorner::BOTTOMLEFT)]  = color;
	colors_[UInt32(UIQuadCorner::BOTTOMRIGHT)] = color;
}

void UIElement::SetColor(UIQuadCorner corner, const Color& color)
{
	colors_[UInt32(corner)] = color;
}

UIElement* UIElement::CreateChild()
{
	SharedPtr<UIElement> uiElement(new UIElement());

	AddChild(uiElement);

	return uiElement;
}

void UIElement::AddChild(UIElement* child)
{
	children_.Push(SharedPtr<UIElement>(child));

	YGNodeInsertChild(layoutNode_, child->layoutNode_, YGNodeGetChildCount(layoutNode_));
}

void UIElement::CalculateLayout(float availableWidth, float availableHeight)
{
	YGNodeCalculateLayout(layoutNode_, availableWidth, availableHeight, YGDirectionLTR);
}

void UIElement::UpdateBatch(VertexVector* vertexVector, Vector<SharedPtr<Batch>>& uiBatches)
{
	float x1 = YGNodeLayoutGetLeft(layoutNode_);
	float y1 = YGNodeLayoutGetTop(layoutNode_);
	float x2 = x1 + YGNodeLayoutGetWidth(layoutNode_);
	float y2 = y1 + YGNodeLayoutGetHeight(layoutNode_);

	SharedPtr<Batch2D> batch(new Batch2D(vertexVector));

	batch->AddTriangle(
		Vector2(x1, y1), Vector2(x1, y2), Vector2(x2, y2),
		Vector2(0, 0), Vector2(1, 0), Vector2(1, 1),
		colors_[UInt32(UIQuadCorner::TOPLEFT)].ToUInt(),
		colors_[UInt32(UIQuadCorner::BOTTOMLEFT)].ToUInt(),
		colors_[UInt32(UIQuadCorner::BOTTOMRIGHT)].ToUInt()
	);

	batch->AddTriangle(
		Vector2(x1, y1), Vector2(x2, y2), Vector2(x2, y1),
		Vector2(0, 0), Vector2(1, 1), Vector2(0, 1),
		colors_[UInt32(UIQuadCorner::TOPLEFT)].ToUInt(),
		colors_[UInt32(UIQuadCorner::BOTTOMRIGHT)].ToUInt(),
		colors_[UInt32(UIQuadCorner::TOPRIGHT)].ToUInt()
	);

	uiBatches.Push(batch);
}

}
