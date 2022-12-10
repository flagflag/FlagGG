#ifndef __VIEWPORT__
#define __VIEWPORT__

#include "Export.h"

#include "Graphics/RenderSurface.h"
#include "Graphics/RenderContext.h"
#include "Scene/Camera.h"
#include "Container/Ptr.h"
#include "Core/Context.h"
#include "Math/Rect.h"
#include "Scene/Scene.h"

namespace FlagGG
{

// IDXGISwapChain*;
class FlagGG_API Viewport : public RefCounted
{
public:
	~Viewport() override = default;

	UInt32 GetX() const;

	UInt32 GetY() const;

	UInt32 GetWidth() const;

	UInt32 GetHeight() const;

	void Resize(const IntRect& rect);

	const IntRect& GetSize() const;

	RenderSurface* GetRenderTarget() const;

	void SetRenderTarget(RenderSurface* renderTarget);

	RenderSurface* GetDepthStencil() const;

	void SetDepthStencil(RenderSurface* depthStencil);

	Camera* GetCamera() const;

	void SetCamera(Camera* camera);

	Scene* GetScene() const;

	void SetScene(Scene* scene);

	void SetViewport();

protected:
	void CreateRenderTarget();

	void CreateSwapChain();

	void CreateDepthStencilView();
			
protected:
	SharedPtr<RenderSurface> renderTarget_;
	SharedPtr<RenderSurface> depthStencil_;

	SharedPtr<Camera> camera_;

	SharedPtr<Scene> scene_;

	ID3D11DepthStencilView* depthStencialView_{ nullptr };

	IntRect rect_;
};

}

#endif