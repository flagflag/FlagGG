//
// 渲染视图
// 缓存渲染相关的数据
//

#pragma once

#include "Core/Object.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"
#include "Graphics/GraphicsDef.h"
#include "Math/Rect.h"

#define OCTREE_QUERY 1

namespace FlagGG
{

class Viewport;
class GfxDevice;
class RenderEngine;
class RenderPipline;
class GfxRenderSurface;
class DrawableComponent;
class Scene;
class Camera;
class Octree;
class Light;
struct RenderContext;
struct VisibleRenderObjects;

class RenderView : public Object
{
	OBJECT_OVERRIDE(RenderView, Object);
public:
	explicit RenderView();

	~RenderView() override;

	// 定义数据
	void Define(RenderPipline* renderPipline, Viewport* viewport);

	// 取消定义，释放指针强引用
	void Undefine();

	// 渲染更新
	void RenderUpdate();

	// 渲染
	void Render();

protected:
	void HandleEndFrame(Real timeStep);

	bool SetShadowMap();

	// 收集视图可见对象
	void CollectVisibilityObjects();

	// 渲染shadowmap
	void RenderShadowMap();

private:
	GfxDevice* gfxDevice_{};

	RenderEngine* renderEngine_{};

	// 渲染管线
	SharedPtr<RenderPipline> renderPipline_;

	// render target
	SharedPtr<GfxRenderSurface> renderTarget_;

	// depth stencil
	SharedPtr<GfxRenderSurface> depthStencil_;
	
	// 每帧重置Scene
	SharedPtr<Scene> scene_;

	// 每帧重置Camera
	SharedPtr<Camera> camera_;

	// 没帧重置Octree
	SharedPtr<Octree> octree_;

	// 视口大小
	IntRect viewport_;

	// 阴影光栅状态
	RasterizerState shadowRasterizerState_;

#if OCTREE_QUERY
	PODVector<DrawableComponent*> tempQueryResults_;
#else
	// 当前视口可见渲染Batch
	PODVector<const RenderContext*> visibleRenderContext_;

	// 投射阴影的渲染Batch
	PODVector<const RenderContext*> shadowCasterRenderContext_;

	PODVector<Light*> visibleLights_;
#endif

	VisibleRenderObjects* visibleRenderObjects_{};
};

}
