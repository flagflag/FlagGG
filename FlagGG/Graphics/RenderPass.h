//
// Render pass
//

#pragma once

#include "Core/Object.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{

class Light;
class DrawableComponent;

struct FlagGG_API RenderPassContext
{
	Light* light_;
	DrawableComponent* drawable_;
};

// 基类pass
class FlagGG_API RenderPass : public Object
{
	OBJECT_OVERRIDE(RenderPass, Object);
public:
	explicit RenderPass();

	~RenderPass() override;

	// 收集
	virtual void CollectBatch(RenderPassContext* context) = 0;

	// 排序
	virtual void SortBatch() = 0;

	// 渲染
	virtual void RenderBatch(UInt32 layer) = 0;
};

// ShadowMap pass
class FlagGG_API ShadowRenderPass : public RenderPass
{
	OBJECT_OVERRIDE(ShadowRenderPass, RenderPass);
public:
	explicit ShadowRenderPass();

	~ShadowRenderPass() override;

	// 收集
	void CollectBatch(RenderPassContext* context) override;

	// 排序
	void SortBatch() override;

	// 渲染
	void RenderBatch(UInt32 layer) override;
};

// 光照pass
class FlagGG_API LitRenderPass : public RenderPass
{
	OBJECT_OVERRIDE(LitRenderPass, RenderPass);
public:
	explicit LitRenderPass();

	~LitRenderPass() override;

	// 收集
	void CollectBatch(RenderPassContext* context) override;

	// 排序
	void SortBatch() override;

	// 渲染
	void RenderBatch(UInt32 layer) override;
};

// 无光pass
class FlagGG_API AlphaRenderPass : public RenderPass
{
	OBJECT_OVERRIDE(AlphaRenderPass, RenderPass);
public:
	explicit AlphaRenderPass();

	~AlphaRenderPass() override;

	// 收集
	void CollectBatch(RenderPassContext* context) override;

	// 排序
	void SortBatch() override;

	// 渲染
	void RenderBatch(UInt32 layer) override;
};

}
