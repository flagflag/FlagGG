#ifndef __RENDER_ENGINE__
#define __RENDER_ENGINE__

#include "Export.h"
#include "Graphics/GraphicsDef.h"
#include "Graphics/Viewport.h"
#include "Graphics/VertexFormat.h"
#include "Graphics/RenderContext.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/ShaderParameter.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Batch.h"
#include "Scene/Camera.h"
#include "Resource/Image.h"
#include "Container/HashMap.h"
#include "Utility/Singleton.h"
#include "AsyncFrame/Mutex.h"
#include "Core/Context.h"

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
		class FlagGG_API RenderEngine : public Utility::Singleton<RenderEngine, AsyncFrame::NullMutex, Core::Context*>
		{
		public:
			RenderEngine(Core::Context* context);

			void Initialize();

			void Uninitialize();

			ID3D11Device* GetDevice();

			ID3D11DeviceContext* GetDeviceContext();

			bool CheckMultiSampleSupport(DXGI_FORMAT format, UInt32 sampleCount);

			UInt32 GetMultiSampleQuality(DXGI_FORMAT format, UInt32 sampleCount);

			void SetTextureQuality(MaterialQuality quality);

			MaterialQuality GetTextureQuality();

			static UInt32 GetFormat(Resource::CompressedFormat format);

			/// Return the API-specific alpha texture format.
			static UInt32 GetAlphaFormat();
			/// Return the API-specific luminance texture format.
			static UInt32 GetLuminanceFormat();
			/// Return the API-specific luminance alpha texture format.
			static UInt32 GetLuminanceAlphaFormat();
			/// Return the API-specific RGB texture format.
			static UInt32 GetRGBFormat();
			/// Return the API-specific RGBA texture format.
			static UInt32 GetRGBAFormat();
			/// Return the API-specific RGBA 16-bit texture format.
			static UInt32 GetRGBA16Format();
			/// Return the API-specific RGBA 16-bit float texture format.
			static UInt32 GetRGBAFloat16Format();
			/// Return the API-specific RGBA 32-bit float texture format.
			static UInt32 GetRGBAFloat32Format();
			/// Return the API-specific RG 16-bit texture format.
			static UInt32 GetRG16Format();
			/// Return the API-specific RG 16-bit float texture format.
			static UInt32 GetRGFloat16Format();
			/// Return the API-specific RG 32-bit float texture format.
			static UInt32 GetRGFloat32Format();
			/// Return the API-specific single channel 16-bit float texture format.
			static UInt32 GetFloat16Format();
			/// Return the API-specific single channel 32-bit float texture format.
			static UInt32 GetFloat32Format();
			/// Return the API-specific linear depth texture format.
			static UInt32 GetLinearDepthFormat();
			/// Return the API-specific hardware depth-stencil texture format.
			static UInt32 GetDepthStencilFormat();
			/// Return the API-specific readable hardware depth format, or 0 if not supported.
			static UInt32 GetReadableDepthFormat();
			/// Return the API-specific texture format from a textual description, for example "rgb".
			static UInt32 GetFormat(const Container::String& formatName);

			// 最大骨骼数
			static UInt32 GetMaxBonesNum();

			ShaderParameters& GetShaderParameters();

			void PostRenderBatch(const Container::Vector<Container::SharedPtr<Batch>>& batches);

			void SetDefaultTextures(TextureClass index, Texture* texture);

			void SetRasterizerState(RasterizerState rasterizerState);

			void SetShaderParameter(Scene::Camera* camera, const RenderContext* renderContext);

			void SetVertexBuffers(const Container::Vector<Container::SharedPtr<VertexBuffer>>& vertexBuffers);

			void SetIndexBuffer(IndexBuffer* indexBuffer);

			void SetVertexShader(Shader* shader);

			void SetPixelShader(Shader* shader);

			void SetTextures(const Container::Vector<Container::SharedPtr<Texture>>& textures);

			void SetPrimitiveType(PrimitiveType primitiveType);

			void SetRenderTarget(Viewport* viewport, bool renderShadowMap = false);

			void PreDraw();

			void DrawCallIndexed(UInt32 indexStart, UInt32 indexCount);

			void DrawCall(UInt32 vertexStart, UInt32 vertexCount);

			void Render(Viewport* viewport);

			void RenderBatch(Viewport* viewport);

		protected:
			void CopyShaderParameterToBuffer(Shader* shader, ConstantBuffer* buffer);

			VertexFormat* CacheVertexFormat(Shader* VSShader, VertexBuffer** vertexBuffer);

		private:
			enum ConstBufferType
			{
				CONST_BUFFER_WORLD = 0,
				CONST_BUFFER_SKIN,
				CONST_BUFFER_VS,
				CONST_BUFFER_PS,
				MAX_CONST_BUFFER,
			};

			void CreateDevice();

			void CreateShadowRasterizerState();

			ID3D11Device* device_{ nullptr };
			ID3D11DeviceContext* deviceContext_{ nullptr };

			RasterizerState rasterizerState_;
			RasterizerState shadowRasterizerState_;
			bool rasterizerStateDirty_{ false };
			Container::HashMap<UInt32, ID3D11RasterizerState*> rasterizerStates_;

			ConstantBuffer vsConstantBuffer_[MAX_CONST_BUFFER_COUNT];
			ConstantBuffer psConstantBuffer_[MAX_CONST_BUFFER_COUNT];
			const Math::Matrix3x4* skinMatrix_{ nullptr };
			UInt32 numSkinMatrix_{ 0u };

			MaterialQuality textureQuality_{ QUALITY_HIGH };

			PrimitiveType primitiveType_;

			Container::Vector<Container::SharedPtr<VertexBuffer>> vertexBuffers_;
			bool vertexBufferDirty_{ false };

			Container::SharedPtr<IndexBuffer> indexBuffer_;
			bool indexBufferDirty_{ false };

			Container::SharedPtr<Shader> vertexShader_{ nullptr };
			bool vertexShaderDirty_{ false };

			Container::SharedPtr<Shader> pixelShader_{ nullptr };
			bool pixelShaderDirty_{ false };

			Container::SharedPtr<Texture> textures_[MAX_TEXTURE_CLASS];
			Container::SharedPtr<Texture> defaultTextures_[MAX_TEXTURE_CLASS];
			Container::SharedPtr<Texture2D> envTexture_;
			bool texturesDirty_{ false };

			Container::HashMap<uint64_t, Container::SharedPtr<VertexFormat>> vertexFormatCache_;

			ShaderParameters shaderParameters_;
			Container::SharedPtr<ShaderParameters> inShaderParameters_;

			Container::SharedPtr<RenderSurface> renderTarget_;
			Container::SharedPtr<RenderSurface> depthStencil_;
			bool renderShadowMap_{ false };
			bool renderTargetDirty_{ false };

			Container::Vector<Container::SharedPtr<Batch>> batches_;

			Core::Context* context_;
		};
	}
}

#endif