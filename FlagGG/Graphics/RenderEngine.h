#ifndef __RENDER_ENGINE__
#define __RENDER_ENGINE__

#include <d3d11.h>

#include <stdio.h>

#define SAFE_RELEASE(p) \
	if ((p)) \
	{ \
		((IUnknown*)p)->Release();  p = nullptr; \
	}

namespace FlagGG
{
	namespace Graphics
	{
		class RenderEngine
		{
		public:
			static void Initialize();

			static void Uninitialize();

			static ID3D11Device* GetDevice();

			static ID3D11DeviceContext* GetDeviceContext();

			static void Update();

		private:
			static void CreateDevice();

			static void CreateShader();

			static ID3D11Device* device_;

			static ID3D11DeviceContext* deviceContext_;

			static ID3D11VertexShader* vertexShader_;

			static ID3D11PixelShader* pixelShader_;

			static ID3D11InputLayout* inputLayout_;
		};
	}
}

#endif