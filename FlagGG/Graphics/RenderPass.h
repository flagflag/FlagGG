//
// Render pass
//

#pragma once

#include "Core/Object.h"
#include "Core/BaseTypes.h"
#include "Graphics/RenderBatch.h"

namespace FlagGG
{

class Camera;
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

	// ����
	virtual void Clear() = 0;

	// �ռ�
	virtual void CollectBatch(RenderPassContext* context) = 0;

	// ����
	virtual void SortBatch() = 0;

	// ��Ⱦ
	virtual void RenderBatch(Camera* camera, UInt32 layer) = 0;
};

// ShadowMap pass
class FlagGG_API ShadowRenderPass : public RenderPass
{
	OBJECT_OVERRIDE(ShadowRenderPass, RenderPass);
public:
	explicit ShadowRenderPass();

	~ShadowRenderPass() override;

	// ����
	void Clear() override;

	// �ռ�
	void CollectBatch(RenderPassContext* context) override;

	// ����
	void SortBatch() override;

	// ��Ⱦ
	void RenderBatch(Camera* camera, UInt32 layer) override;

private:
	// ��Ӱ��դ״̬
	RasterizerState shadowRasterizerState_;

	HashMap<Light*, ShadowRenderContext> shadowRenderContextMap_;
};

// ����pass
class FlagGG_API LitRenderPass : public RenderPass
{
	OBJECT_OVERRIDE(LitRenderPass, RenderPass);
public:
	explicit LitRenderPass();

	~LitRenderPass() override;

	// ����
	void Clear() override;

	// �ռ�
	void CollectBatch(RenderPassContext* context) override;

	// ����
	void SortBatch() override;

	// ��Ⱦ
	void RenderBatch(Camera* camera, UInt32 layer) override;

private:
	HashMap<Light*, LitRenderContext> litRenderContextMap_;
};

// �޹�pass
class FlagGG_API AlphaRenderPass : public RenderPass
{
	OBJECT_OVERRIDE(AlphaRenderPass, RenderPass);
public:
	explicit AlphaRenderPass();

	~AlphaRenderPass() override;

	// ����
	void Clear() override;

	// �ռ�
	void CollectBatch(RenderPassContext* context) override;

	// ����
	void SortBatch() override;

	// ��Ⱦ
	void RenderBatch(Camera* camera, UInt32 layer) override;
};

}
