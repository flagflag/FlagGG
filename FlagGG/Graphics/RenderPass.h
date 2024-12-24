//
// Render pass
//

#pragma once

#include "Core/Object.h"
#include "Core/BaseTypes.h"
#include "Graphics/RenderBatch.h"
#include "Math/SphericalHarmonicsL2.h"

namespace FlagGG
{

class Camera;
class Light;
class DrawableComponent;
class Texture2D;
class TextureCube;

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

	// 清理
	virtual void Clear() = 0;

	// 收集
	virtual void CollectBatch(RenderPassContext* context) = 0;

	// 排序
	virtual void SortBatch() = 0;

	// 渲染
	virtual void RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer) = 0;

	// 返回是否有渲染批次
	virtual bool HasAnyBatch() const = 0;
};

// ShadowMap pass
class FlagGG_API ShadowRenderPass : public RenderPass
{
	OBJECT_OVERRIDE(ShadowRenderPass, RenderPass);
public:
	explicit ShadowRenderPass();

	~ShadowRenderPass() override;

	// 清理
	void Clear() override;

	// 收集
	void CollectBatch(RenderPassContext* context) override;

	// 排序
	void SortBatch() override;

	// 渲染
	void RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer) override;

	// 返回是否有渲染批次
	bool HasAnyBatch() const override;

private:
	// 阴影光栅状态
	RasterizerState rasterizerState_;

	// 阴影深度模板状态
	DepthStencilState depthStencilState_;

	HashMap<Light*, ShadowRenderContext> shadowRenderContextMap_;
};

// 光照pass
class FlagGG_API LitRenderPass : public RenderPass
{
	OBJECT_OVERRIDE(LitRenderPass, RenderPass);
public:
	explicit LitRenderPass();

	~LitRenderPass() override;

	// 清理
	void Clear() override;

	// 收集
	void CollectBatch(RenderPassContext* context) override;

	// 排序
	void SortBatch() override;

	// 渲染
	void RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer) override;

	// 返回是否有渲染批次
	bool HasAnyBatch() const override;

private:
	HashMap<Light*, LitRenderContext> litRenderContextMap_;

	Vector4 shaderConstants_[SphericalHarmonicsL2::kVec4Count];
	SharedPtr<TextureCube> iblCube_;
};

// 水体pass
class FlagGG_API WaterRenderPass : public RenderPass
{
	OBJECT_OVERRIDE(WaterRenderPass, RenderPass);
public:
	explicit WaterRenderPass();

	~WaterRenderPass() override;

	// 清理
	void Clear() override;

	// 收集
	void CollectBatch(RenderPassContext* context) override;

	// 排序
	void SortBatch() override;

	// 渲染
	void RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer) override;

	// 返回是否有渲染批次
	bool HasAnyBatch() const override { return renderBatchQueue_.renderBatches_.Size() || renderBatchQueue_.renderBatchGroups_.Size(); }

private:
	RenderBatchQueue renderBatchQueue_;

	SharedPtr<Texture2D> refractionTexture_;
};

// 无光pass
class FlagGG_API AlphaRenderPass : public RenderPass
{
	OBJECT_OVERRIDE(AlphaRenderPass, RenderPass);
public:
	explicit AlphaRenderPass();

	~AlphaRenderPass() override;

	// 清理
	void Clear() override;

	// 收集
	void CollectBatch(RenderPassContext* context) override;

	// 排序
	void SortBatch() override;

	// 渲染
	void RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer) override;

	// 返回是否有渲染批次
	bool HasAnyBatch() const override { return renderBatchQueue_.renderBatches_.Size() || renderBatchQueue_.renderBatchGroups_.Size(); }

private:
	RenderBatchQueue renderBatchQueue_;
};

// 延迟base pass
class FlagGG_API DeferredBaseRenderPass : public RenderPass
{
	OBJECT_OVERRIDE(DeferredBaseRenderPass, RenderPass);
public:
	explicit DeferredBaseRenderPass();

	~DeferredBaseRenderPass() override;

	// 清理
	void Clear() override;

	// 收集
	void CollectBatch(RenderPassContext* context) override;

	// 排序
	void SortBatch() override;

	// 渲染
	void RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer) override;

	// 返回是否有渲染批次
	bool HasAnyBatch() const override { return renderBatchQueue_.renderBatches_.Size() || renderBatchQueue_.renderBatchGroups_.Size(); }

private:
	RenderBatchQueue renderBatchQueue_;
};

// Compute cluster pass
class FlagGG_API ComputeClusterLightPass : public RenderPass
{
public:

};

}
