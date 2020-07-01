#include "Viewport.h"
#include "RenderEngine.h"
#include "bgfx/bgfx.h"

namespace FlagGG
{
	namespace Graphics
	{
		UInt32 Viewport::GetX() const
		{
			return rect_.left_;
		}

		UInt32 Viewport::GetY() const
		{
			return rect_.top_;
		}

		UInt32 Viewport::GetWidth() const
		{
			return rect_.Width();
		}

		UInt32 Viewport::GetHeight() const
		{
			return rect_.Height();
		}

		void Viewport::Resize(const Math::IntRect& rect)
		{
			rect_ = rect;
		}

		const Math::IntRect& Viewport::GetSize() const
		{
			return rect_;
		}

		RenderSurface* Viewport::GetRenderTarget() const
		{
			return renderTarget_;
		}

		void Viewport::SetRenderTarget(RenderSurface* renderTarget)
		{
			renderTarget_ = renderTarget;
		}

		RenderSurface* Viewport::GetDepthStencil() const
		{
			return depthStencil_;
		}

		void Viewport::SetDepthStencil(RenderSurface* depthStencil)
		{
			depthStencil_ = depthStencil;
		}

		Scene::Camera* Viewport::GetCamera() const
		{
			return camera_;
		}

		void Viewport::SetCamera(Scene::Camera* camera)
		{
			camera_ = camera;
		}

		Scene::Scene* Viewport::GetScene() const
		{
			return scene_;
		}

		void Viewport::SetScene(Scene::Scene* scene)
		{
			scene_ = scene;
		}
	}
}

