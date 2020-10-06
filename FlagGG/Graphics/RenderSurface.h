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

		/// FrameBuffer
		class FlagGG_API RenderSurface : public GPUObject, public Container::RefCounted
		{
		public:
			RenderSurface(Texture* parentTexture);

			~RenderSurface() override = default;

			bool IsValid() override;

			Texture* GetParentTexture();

			UInt16 GetBgfxLayer() const { return layer_; }

			friend class Texture2D;

		protected:
			void Initialize() override;

		private:
			Texture* parentTexture_{ nullptr };

			void* readOnlyView_{ nullptr };

			UInt16 layer_{};
		};
	}
}

#endif