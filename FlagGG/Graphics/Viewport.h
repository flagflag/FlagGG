#ifndef __VIEWPORT__
#define __VIEWPORT__

#include "RenderTarget.h"

namespace FlagGG
{
	namespace Graphics
	{
		// IDXGISwapChain*;
		class Viewport : public GPUObject
		{
		public:
			virtual ~Viewport() = default;

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

			void SetViewport();

			RenderTarget* renderTarget_{ nullptr };
		};
	}
}

#endif