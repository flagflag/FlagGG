//
// 视口
//

#pragma once

#include "Export.h"

#include "Graphics/RenderContext.h"
#include "Scene/Camera.h"
#include "Container/Ptr.h"
#include "Core/Context.h"
#include "Math/Rect.h"
#include "Scene/Scene.h"

namespace FlagGG
{

class GfxRenderSurface;
class RenderView;
class RenderPipline;

class FlagGG_API Viewport : public RefCounted
{
public:
	explicit Viewport();

	~Viewport() override;

	// 获取视口x坐标
	UInt32 GetX() const { return rect_.left_; }

	// 获取视口的Y坐标
	UInt32 GetY() const { return rect_.top_; }

	// 获取视口的宽度
	UInt32 GetWidth() const { return rect_.Width(); }

	// 获取视口的高度
	UInt32 GetHeight() const { return rect_.Height(); }

	// 重置视口大小
	void Resize(const IntRect& rect);

	// 获取视口的大小
	const IntRect& GetSize() const { return rect_; }

	// 获取RenderTarget
	GfxRenderSurface* GetRenderTarget() const { return renderTarget_; }

	// 设置RenderTarget
	void SetRenderTarget(GfxRenderSurface* renderTarget);

	// 获取DepthStencil
	GfxRenderSurface* GetDepthStencil() const { return depthStencil_; }

	// 设置DepthStencil
	void SetDepthStencil(GfxRenderSurface* depthStencil);

	// 获取相机
	Camera* GetCamera() const { return camera_; }

	// 设置相机
	void SetCamera(Camera* camera);

	// 获取场景
	Scene* GetScene() const { return scene_; }

	// 设置场景
	void SetScene(Scene* scene);

	// 设置渲染管线
	void SetRenderPipline(RenderPipline* renderPipline);

	// 获取渲染管线
	RenderPipline* GetRenderPipline() const { return renderPipline_; }

	// 获取or创建渲染视图
	RenderView* GetOrCreateRenderView();

	// 重置渲染视图
	void ResetRenderView();
			
protected:
	SharedPtr<RenderView> renderView_;

	SharedPtr<RenderPipline> renderPipline_;

	SharedPtr<GfxRenderSurface> renderTarget_;
	SharedPtr<GfxRenderSurface> depthStencil_;

	SharedPtr<Camera> camera_;

	SharedPtr<Scene> scene_;

	IntRect rect_;
};

}
