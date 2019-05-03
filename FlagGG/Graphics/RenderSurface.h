#ifndef __RENDER_TARGET__
#define __RENDER_TARGET__

#include "Export.h"

#include "Graphics/GPUObject.h"
#include "Container/RefCounted.h"
#include "Math/Vector2.h"

namespace FlagGG
{
	namespace Graphics
	{
		// ID3D11RenderTargetView*;
		// ID3D11DepthStencilView*
		class FlagGG_API RenderSurface : public GPUObject, public Container::RefCounted
		{
		public:
			~RenderSurface() override = default;

			bool IsValid() override;

			friend class Texture2D;

		protected:
			void Initialize() override;

		private:
			void* readOnlyView_{ nullptr };
		};
	}
}

#endif