#ifndef __RENDER_TARGET__
#define __RENDER_TARGET__

#include "Export.h"

#include "Graphics/GPUObject.h"
#include "Container/Ptr.h"
#include "Math/Vector2.h"

namespace FlagGG
{
	namespace Graphics
	{
		class Texture;

		// ID3D11RenderTargetView*;
		// ID3D11DepthStencilView*
		class FlagGG_API RenderSurface : public GPUObject, public Container::RefCounted
		{
		public:
			RenderSurface(Texture* parentTexture);

			~RenderSurface() override = default;

			bool IsValid() override;

			Texture* GetParentTexture();

			friend class Texture2D;

		protected:
			void Initialize() override;

		private:
			Texture* parentTexture_{ nullptr };

			void* readOnlyView_{ nullptr };
		};
	}
}

#endif