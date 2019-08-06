#ifndef __RENDER_ENGINE__
#define __RENDER_ENGINE__

#include "Export.h"
#include "Graphics/GraphicsDef.h"
#include "Graphics/Viewport.h"
#include "Graphics/VertexFormat.h"
#include "Graphics/RenderContext.h"
#include "Resource/Image.h"
#include "Container/HashMap.h"

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
			void Initialize();

			void Uninitialize();

			ID3D11Device* GetDevice();

			ID3D11DeviceContext* GetDeviceContext();

			bool CheckMultiSampleSupport(DXGI_FORMAT format, uint32_t sampleCount);

			uint32_t GetMultiSampleQuality(DXGI_FORMAT format, uint32_t sampleCount);

			void SetTextureQuality(MaterialQuality quality);

			MaterialQuality GetTextureQuality();

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

			// ×î´ó¹Ç÷ÀÊý
			static uint32_t GetMaxBonesNum();

			void UpdateMatrix(Camera* camera, const RenderContext* renderContext);

			void SetVertexBuffers(const Container::Vector<Container::SharedPtr<VertexBuffer>>& vertexBuffers);

			void SetIndexBuffer(IndexBuffer* indexBuffer);

			void SetVertexShader(Shader* shader);

			void SetPixelShader(Shader* shader);

			void SetTexture(Texture* texture);

			void SetTextures(const Container::Vector<Container::SharedPtr<Texture>>& textures);

			void SetDefaultTextures(TextureClass index, Texture* texture);

			void SetPrimitiveType(PrimitiveType primitiveType);

			void DrawCall(uint32_t indexStart, uint32_t indexCount);

			void RenderBegin(Viewport* viewport);

			void RenderEnd(Viewport* viewport);

			void Render(Viewport* viewport);

			VertexFormat* CacheVertexFormat(Shader* VSShader, VertexBuffer** vertexBuffer);

			static RenderEngine* Instance();

		private:
			enum ConstBufferType
			{
				CONST_BUFFER_MATRIX = 0,
				CONST_BUFFER_SKIN,
				MAX_CONST_BUFFER,
			};

			void CreateDevice();

			void CreateRasterizerState();

			void CreateMatrixData();

			ID3D11Device* device_{ nullptr };
			ID3D11DeviceContext* deviceContext_{ nullptr };
			ID3D11RasterizerState* rasterizerState_{ nullptr };

			ID3D11Buffer* constBuffer_[MAX_CONST_BUFFER_COUNT]{ 0 };

			MaterialQuality textureQuality_{ QUALITY_HIGH };

			ID3D11Buffer* vertexBuffers_[MAX_VERTEX_BUFFER_COUNT]{ 0 };
			uint32_t vertexSize_[MAX_VERTEX_BUFFER_COUNT]{ 0 };
			uint32_t vertexOffset_[MAX_VERTEX_BUFFER_COUNT]{ 0 };

			Container::SharedPtr<Texture> defaultTextures[MAX_TEXTURE_CLASS];
			ID3D11ShaderResourceView* shaderResourceView_[MAX_TEXTURE_CLASS]{ 0 };
			ID3D11SamplerState* samplerState_[MAX_TEXTURE_CLASS]{ 0 };

			const Container::Vector<Container::SharedPtr<VertexBuffer>>* cacheVertexBuffers_{ nullptr };
			Shader* cacheVSShader_{ nullptr };

			Container::HashMap<uint64_t, Container::SharedPtr<VertexFormat>> vertexFormatCache_;

			static RenderEngine* renderEngine_;
		};
	}
}

#endif