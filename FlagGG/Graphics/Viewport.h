#ifndef __VIEWPORT__
#define __VIEWPORT__

#include "Export.h"

#include "RenderTarget.h"
#include "Container/Ptr.h"

namespace FlagGG
{
	namespace Graphics
	{
		// IDXGISwapChain*;
		class FlagGG_API Viewport : public GPUObject
		{
		public:
			~Viewport() override;

			void Initialize() override;

			bool IsValid() override;

			virtual unsigned GetWidth() = 0;

			virtual unsigned GetHeight() = 0;

			virtual void Resize(unsigned width, unsigned height) = 0;

			virtual void* GetWindow() = 0;

			RenderTarget* GetRenderTarget();

			bool sRGB_{ true };

		private:
			void CreateRenderTarget();

			void CreateSwapChain();

			void CreateDepthStencilView();

			void SetViewport();

			Container::SharedPtr<RenderTarget> renderTarget_;

		protected:
			ID3D11DepthStencilView* depthStencialView_{ nullptr };
		};
	}
}

#endif