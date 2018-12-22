#include "RenderTarget.h"
#include "RenderEngine.h"

namespace FlagGG
{
	namespace Graphics
	{
		RenderTarget::RenderTarget(ID3D11Texture2D* backbufferTexture) :
			backbufferTexture_(backbufferTexture)
		{
		}

		RenderTarget::~RenderTarget()
		{
			SAFE_RELEASE(backbufferTexture_);
		}

		void RenderTarget::Initialize()
		{
			ID3D11RenderTargetView* renderTargetView = GetObject<ID3D11RenderTargetView>();

			HRESULT hr = RenderEngine::GetDevice()->CreateRenderTargetView(
				backbufferTexture_,
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
