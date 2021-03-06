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
	namespace Graphics
	{
		// IDXGISwapChain*;
		class FlagGG_API Viewport : public Container::RefCounted
		{
		public:
			~Viewport() override = default;

			UInt32 GetX() const;

			UInt32 GetY() const;

			UInt32 GetWidth() const;

			UInt32 GetHeight() const;

			void Resize(const Math::IntRect& rect);

			const Math::IntRect& GetSize() const;

			RenderSurface* GetRenderTarget() const;

			void SetRenderTarget(RenderSurface* renderTarget);

			RenderSurface* GetDepthStencil() const;

			void SetDepthStencil(RenderSurface* depthStencil);

			Scene::Camera* GetCamera() const;

			void SetCamera(Scene::Camera* camera);

			Scene::Scene* GetScene() const;

			void SetScene(Scene::Scene* scene);

			void SetViewport();

		protected:
			void CreateRenderTarget();

			void CreateSwapChain();

			void CreateDepthStencilView();
			
		protected:
			Container::SharedPtr<RenderSurface> renderTarget_;
			Container::SharedPtr<RenderSurface> depthStencil_;

			Container::SharedPtr<Scene::Camera> camera_;

			Container::SharedPtr<Scene::Scene> scene_;

			ID3D11DepthStencilView* depthStencialView_{ nullptr };

			Math::IntRect rect_;
		};
	}
}

#endif