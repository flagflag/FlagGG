//
// 渲染管线
// 以一定的规则渲染收集好的渲染数据
//

#pragma once

#include "Core/Object.h"
#include "Container/Vector.h"
#include "Container/Ptr.h"

struct lua_State;

namespace FlagGG
{

class GfxRenderSurface;
class RenderPass;
struct RenderContext;
class DrawableComponent;
class Camera;
class Light;
class Probe;

// 渲染管线上下文
struct FlagGG_API RenderPiplineContext
{
	void Clear();

	// RenderTarget
	GfxRenderSurface* renderTarget_{};
	// DepthStencil
	GfxRenderSurface* depthStencil_{};
	// 相机
	Camera* camera_{};
	// 视图内可见渲染对象
	PODVector<DrawableComponent*> drawables_;
	// 视图内可见灯光
	PODVector<Light*> lights_;
	// 视图内可见探针
	PODVector<Probe*> probes_;
};

struct FlagGG_API LitRenderObjects
{
	void Clear();

	Light* light_;
	PODVector<DrawableComponent*> drawables_;
};

class FlagGG_API RenderPipline : public Object
{
	OBJECT_OVERRIDE(RenderPipline, Object);
public:
	explicit RenderPipline();

	~RenderPipline() override;

	// 是否渲染shadowmap
	virtual bool RenderShadowMap() const = 0;

	// 获取视图内可见对象集合
	virtual RenderPiplineContext& GetRenderPiplineContext() = 0;

	// 清理
	virtual void Clear() = 0;

	// 收集batch
	virtual void CollectBatch() = 0;

	// 渲染前的准备
	virtual void PrepareRender() = 0;

	// 渲染
	virtual void Render() = 0;
};

class FlagGG_API CommonRenderPipline : public RenderPipline
{
public:
	explicit CommonRenderPipline();

	~CommonRenderPipline() override;

	// 是否渲染shadowmap
	bool RenderShadowMap() const override { return true; }

	// 获取视图内可见对象集合
	RenderPiplineContext& GetRenderPiplineContext() override { return renderPiplineContext_; }

	// 清理
	void Clear() override;

protected:
	void CollectLitBatch();

	void CollectUnlitBatch();

protected:
	RenderPiplineContext renderPiplineContext_;

	Vector<LitRenderObjects> litRenderObjectsResult_;

	//ShadowRenderContext shadowRenderContext_;
	//LitRenderContext litRenderContext_;
	//UnlitRenderContext unlitRenderContext_;

	SharedPtr<RenderPass> shadowRenderPass_;
	SharedPtr<RenderPass> litRenderPass_[2];
	SharedPtr<RenderPass> alphaRenderPass_;
};

// 前向渲染管线
class FlagGG_API ForwardRenderPipline : public CommonRenderPipline
{
	OBJECT_OVERRIDE(ForwardRenderPipline, CommonRenderPipline);
public:
	explicit ForwardRenderPipline();

	~ForwardRenderPipline() override;

	// 收集batch
	void CollectBatch() override;

	// 渲染前的准备
	void PrepareRender() override;

	// 渲染
	void Render() override;
};

// 延迟渲染管线
class FlagGG_API DefferedRenderPipline : public CommonRenderPipline
{
	OBJECT_OVERRIDE(DefferedRenderPipline, CommonRenderPipline);
public:
	explicit DefferedRenderPipline();

	~DefferedRenderPipline() override;

	// 收集batch
	void CollectBatch() override;

	// 渲染前的准备
	void PrepareRender() override;

	// 渲染
	void Render() override;
};

class FlagGG_API ScriptRenderPipline : public RenderPipline
{
	OBJECT_OVERRIDE(ScriptRenderPipline, RenderPipline);
public:
	explicit ScriptRenderPipline(lua_State* state);

	~ScriptRenderPipline() override;

	// 是否渲染shadowmap
	bool RenderShadowMap() const override { return true; }

	// 收集batch
	void CollectBatch() override;

	// 渲染前的准备
	void PrepareRender() override;

	// 渲染
	void Render() override;

private:
	lua_State* state_;
};

}
