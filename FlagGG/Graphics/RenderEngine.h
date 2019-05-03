#ifndef __RENDER_ENGINE__
#define __RENDER_ENGINE__

#include "Export.h"
#include "Graphics/GraphicsDef.h"
#include "Graphics/Viewport.h"
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

			static uint32_t GetFormat(Resource::CompressedFormat format);

			/// Return the API-specific alpha texture format.
			static uint32_t GetAlphaFormat();
			/// Return the API-specific luminance texture format.
			static uint32_t GetLuminanceFormat();
			/// Return the API-specific luminance alpha texture format.
			static uint32_t GetLuminanceAlphaFormat();
			/// Return the API-specific RGB texture format.
			static uint32_t GetRGBFormat();
			/// Return the API-specific RGBA texture format.
			static uint32_t GetRGBAFormat();
			/// Return the API-specific RGBA 16-bit texture format.
			static uint32_t GetRGBA16Format();
			/// Return the API-specific RGBA 16-bit float texture format.
			static uint32_t GetRGBAFloat16Format();
			/// Return the API-specific RGBA 32-bit float texture format.
			static uint32_t GetRGBAFloat32Format();
			/// Return the API-specific RG 16-bit texture format.
			static uint32_t GetRG16Format();
			/// Return the API-specific RG 16-bit float texture format.
			static uint32_t GetRGFloat16Format();
			/// Return the API-specific RG 32-bit float texture format.
			static uint32_t GetRGFloat32Format();
			/// Return the API-specific single channel 16-bit float texture format.
			static uint32_t GetFloat16Format();
			/// Return the API-specific single channel 32-bit float texture format.
			static uint32_t GetFloat32Format();
			/// Return the API-specific linear depth texture format.
			static uint32_t GetLinearDepthFormat();
			/// Return the API-specific hardware depth-stencil texture format.
			static uint32_t GetDepthStencilFormat();
			/// Return the API-specific readable hardware depth format, or 0 if not supported.
			static uint32_t GetReadableDepthFormat();
			/// Return the API-specific texture format from a textual description, for example "rgb".
			static uint32_t GetFormat(const Container::String& formatName);

			static void UpdateMatrix(Camera* camera);

			static void Render(Viewport* viewport);

			static void UpdateVertexData(const uint8_t* vertexs, uint32_t vertexSize, uint32_t vertexCount);

		private:
			static void CreateDevice();

			static void CreateRasterizerState();

			static void CreateMatrixData();

			static ID3D11Device* device_;

			static ID3D11DeviceContext* deviceContext_;

			static ID3D11RasterizerState* rasterizerState_;

			static ID3D11Buffer* matrixData_;

			static MaterialQuality textureQuality_;

			static ID3D11Buffer* d3d11Buffer_;
		};
	}
}

#endif