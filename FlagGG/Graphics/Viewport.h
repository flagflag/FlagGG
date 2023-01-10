#pragma once

#include "Export.h"

#include "Graphics/RenderContext.h"
#include "Scene/Camera.h"
#include "Container/Ptr.h"
#include "Core/Context.h"
#include "Math/Rect.h"
#include "Scene/Scene.h"

#include "GfxDevice/GfxRenderSurface.h"

namespace FlagGG
{

class GfxRenderSurface;

class FlagGG_API Viewport : public RefCounted
{
public:
	~Viewport() override;

	UInt32 GetX() const;

	UInt32 GetY() const;

	UInt32 GetWidth() const;

	UInt32 GetHeight() const;

	void Resize(const IntRect& rect);

	const IntRect& GetSize() const;

	GfxRenderSurface* GetRenderTarget() const;

	void SetRenderTarget(GfxRenderSurface* renderTarget);

	GfxRenderSurface* GetDepthStencil() const;

	void SetDepthStencil(GfxRenderSurface* depthStencil);

	Camera* GetCamera() const;

	void SetCamera(Camera* camera);

	Scene* GetScene() const;

	void SetScene(Scene* scene);

	void SetViewport();
			
protected:
	SharedPtr<GfxRenderSurface> renderTarget_;
	SharedPtr<GfxRenderSurface> depthStencil_;

	SharedPtr<Camera> camera_;

	SharedPtr<Scene> scene_;

	IntRect rect_;
};

}
