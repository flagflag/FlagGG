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

namespace FlagGG
{

class Viewport;
class GfxDevice;
class RenderEngine;
class RenderPipline;
class GfxRenderSurface;
class DrawableComponent;
class Scene;
class Node;
class Camera;
class Octree;
class Light;
struct RenderContext;
struct RenderPiplineContext;

class RenderView : public Object
{
	OBJECT_OVERRIDE(RenderView, Object);
public:
	explicit RenderView();

	~RenderView() override;

	// 定义数据
	void Define(Viewport* viewport);

	// 取消定义，释放指针强引用
	void Undefine();

	// 渲染更新
	void RenderUpdate();

	// 渲染
	void Render();

protected:
	void HandleEndFrame(Real timeStep);

	// 收集视图可见对象
	void CollectVisibilityObjects();

private:
	GfxDevice* gfxDevice_{};

	RenderEngine* renderEngine_{};

	// 默认渲染管线
	SharedPtr<RenderPipline> defaultRenderPipline_;

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

	// 当前帧需要渲染
	bool needRenderView_{};

	// 阴影相机
	SharedPtr<Camera> shadowCamera_;
	SharedPtr<Node> shadowCameraNode_;

	// 没帧重置Octree
	SharedPtr<Octree> octree_;

	// 视口大小
	IntRect viewport_;

	// 缓存 - 八叉树查询结果
	PODVector<DrawableComponent*> tempQueryResults_;

	// 渲染管线上下文
	RenderPiplineContext* renderPiplineContext_{};
};

}
