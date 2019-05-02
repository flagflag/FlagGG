#ifndef __RENDER_ENGINE__
#define __RENDER_ENGINE__

#include "Export.h"
#include "Graphics/GraphicsDef.h"
#include "Resource/Image.h"

#include <d3d11.h>

#include <stdio.h>
#include <stdint.h>

#define SAFE_RELEASE(p) \
	if ((p)) \
	{ \
		((IUnknown*)p)->Release();  p = nullptr; \
	}

namespace FlagGG
{
	namespace Graphics
	{
		class Camera;

		class FlagGG_API RenderEngine
		{
		public:
			static void Initialize();

			static void Uninitialize();

			static ID3D11Device* GetDevice();

			static ID3D11DeviceContext* GetDeviceContext();

			static bool CheckMultiSampleSupport(DXGI_FORMAT format, uint32_t sampleCount);

			static uint32_t GetMultiSampleQuality(DXGI_FORMAT format, uint32_t sampleCount);

			static void SetTextureQuality(MaterialQuality quality);

			static MaterialQuality GetTextureQuality();

			static uint32_t GetAlphaFormat();

			static uint32_t GetRGBAFormat();

			static uint32_t GetFormat(Resource::CompressedFormat format);

			static void UpdateMatrix(Camera* camera);

		private:
			static void CreateDevice();

			static void CreateRasterizerState();

			static void CreateMatrixData();

			static ID3D11Device* device_;

			static ID3D11DeviceContext* deviceContext_;

			static ID3D11RasterizerState* rasterizerState_;

			static ID3D11Buffer* matrixData_;

			static MaterialQuality textureQuality_;
		};
	}
}

#endif