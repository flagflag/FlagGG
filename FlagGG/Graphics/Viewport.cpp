#include "Viewport.h"
#include "RenderEngine.h"

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

		void Viewport::SetViewport()
		{
			//D3D11_VIEWPORT d3d11Viewport;
			//d3d11Viewport.TopLeftX = GetX();
			//d3d11Viewport.TopLeftY = GetY();
			//d3d11Viewport.Width = GetWidth();
			//d3d11Viewport.Height = GetHeight();
			//d3d11Viewport.MinDepth = 0.0f;
			//d3d11Viewport.MaxDepth = 1.0f;

			//RenderEngine::Instance()->GetDeviceContext()->RSSetViewports(1, &d3d11Viewport);
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

