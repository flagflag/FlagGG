//
// UI基础元素节点
//

#pragma once

#include "Core/Object.h"
#include "Container/Ptr.h"
#include "Math/Color.h"
#include "Graphics/Batch2D.h"

struct YGNode;

namespace FlagGG
{

class Texture2D;

enum class UIQuadCorner
{
	TOPLEFT = 0,
	TOPRIGHT,
	BOTTOMLEFT,
	BOTTOMRIGHT,
	MAX,
};

class FlagGG_API UIElement : public Object
{
	OBJECT_OVERRIDE(UIElement, Object);
public:
	explicit UIElement();

	~UIElement() override;

	// 设置绝对布局的位置（left, top）
	void SetPosition(const Vector2& position);

	// 设置固定宽度
	void SetWidth(float width);

	// 设置固定高度
	void SetHeight(float height);

	// 设置百分比宽度
	void SetWidthPercent(float width);

	// 设置百分比高度
	void SetHeightPercent(float height);

	// 设置颜色
	void SetColor(const Color& color);

	// 设置Quad四个角的颜色
	void SetColor(UIQuadCorner corner, const Color& color);

	// 创建孩子节点
	UIElement* CreateChild();

	// 增加孩子节点
	void AddChild(UIElement* child);

	// 获取所有的孩子节点
	const Vector<SharedPtr<UIElement>>& GetChildren() const { return children_; }

	// 计算布局
	void CalculateLayout(float availableWidth, float availableHeight);

	// 更新UI批次数据（由渲染底层调用）
	void UpdateBatch(VertexVector* vertexVector, Vector<SharedPtr<Batch>>& uiBatches);

private:
	// Yoga布局节点
	YGNode* layoutNode_;

	// 孩子节点
	Vector<SharedPtr<UIElement>> children_;

	// Quad四个角的颜色
	Color colors_[UInt32(UIQuadCorner::MAX)];
};

}
