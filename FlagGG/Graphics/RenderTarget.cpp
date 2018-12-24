#include "RenderTarget.h"
#include "RenderEngine.h"

namespace FlagGG
{
	namespace Graphics
	{
		RenderTarget::RenderTarget(ID3D11Resource* resource) :
			resource_(resource)
		{
		}

		RenderTarget::~RenderTarget()
		{
			SAFE_RELEASE(resource_);
		}

		void RenderTarget::Initialize()
		{
			ID3D11RenderTargetView* renderTargetView = GetObject<ID3D11RenderTargetView>();

			HRESULT hr = RenderEngine::GetDevice()->CreateRenderTargetView(
				resource_,
				nullptr,
				&renderTargetView
			);

			if (hr != 0)
			{
				puts("CreateRenderTargetView failed.");
				
				SAFE_RELEASE(renderTargetView);
			
				return;
			}

			ResetHandler(renderTargetView);
		}
	}
}
