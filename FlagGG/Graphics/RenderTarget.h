#ifndef __RENDER_TARGET__
#define __RENDER_TARGET__

#include "GPUObject.h"
#include "Math/Vector2.h"

namespace FlagGG
{
	namespace Graphics
	{
		//ID3D11RenderTargetView*;
		class RenderTarget : public GPUObject
		{
		public:
			RenderTarget(ID3D11Resource* resource);

			virtual ~RenderTarget() override;
		
			void Initialize() override;

		private:
			ID3D11Resource* resource_{ nullptr };
		};
	}
}

#endif