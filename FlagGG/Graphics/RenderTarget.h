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
		//ID3D11RenderTargetView*;
		class FlagGG_API RenderTarget : public GPUObject, public Container::RefCounted
		{
		public:
			RenderTarget(ID3D11Resource* resource);

			virtual ~RenderTarget() override;
		
			void Initialize() override;

			bool IsValid() override;

		private:
			ID3D11Resource* resource_{ nullptr };
		};
	}
}

#endif