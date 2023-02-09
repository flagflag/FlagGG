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

// ����pass
class FlagGG_API RenderPass : public Object
{
	OBJECT_OVERRIDE(RenderPass, Object);
public:
	explicit RenderPass();

	~RenderPass() override;

	// �ռ�
	virtual void CollectBatch(RenderPassContext* context) = 0;

	// ����
	virtual void SortBatch() = 0;

	// ��Ⱦ
	virtual void RenderBatch(UInt32 layer) = 0;
};

// ShadowMap pass
class FlagGG_API ShadowRenderPass : public RenderPass
{
	OBJECT_OVERRIDE(ShadowRenderPass, RenderPass);
public:
	explicit ShadowRenderPass();

	~ShadowRenderPass() override;

	// �ռ�
	void CollectBatch(RenderPassContext* context) override;

	// ����
	void SortBatch() override;

	// ��Ⱦ
	void RenderBatch(UInt32 layer) override;
};

// ����pass
class FlagGG_API LitRenderPass : public RenderPass
{
	OBJECT_OVERRIDE(LitRenderPass, RenderPass);
public:
	explicit LitRenderPass();

	~LitRenderPass() override;

	// �ռ�
	void CollectBatch(RenderPassContext* context) override;

	// ����
	void SortBatch() override;

	// ��Ⱦ
	void RenderBatch(UInt32 layer) override;
};

// �޹�pass
class FlagGG_API AlphaRenderPass : public RenderPass
{
	OBJECT_OVERRIDE(AlphaRenderPass, RenderPass);
public:
	explicit AlphaRenderPass();

	~AlphaRenderPass() override;

	// �ռ�
	void CollectBatch(RenderPassContext* context) override;

	// ����
	void SortBatch() override;

	// ��Ⱦ
	void RenderBatch(UInt32 layer) override;
};

}
