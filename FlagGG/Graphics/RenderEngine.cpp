#include "Graphics/RenderEngine.h"
#include "Graphics/Camera.h"
#include "Graphics/Texture.h"
#include "Math/Math.h"
#include "Log.h"

#include <assert.h>

namespace FlagGG
{
	namespace Graphics
	{
		ID3D11Device* RenderEngine::device_ = nullptr;

		ID3D11DeviceContext* RenderEngine::deviceContext_ = nullptr;

		ID3D11RasterizerState* RenderEngine::rasterizerState_ = nullptr;

		ID3D11Buffer* RenderEngine::constBuffer_[MAX_CONST_BUFFER_COUNT] = { 0 };

		MaterialQuality RenderEngine::textureQuality_ = QUALITY_HIGH;

		ID3D11Buffer* RenderEngine::vertexBuffers_[MAX_VERTEX_BUFFER_COUNT] = { 0 };

		Container::HashMap<uint64_t, Container::SharedPtr<VertexFormat>> RenderEngine::vertexFormatCache_;

		struct MatrixData
		{
			Math::Matrix4 world;
			Math::Matrix4 view;
			Math::Matrix4 projection;
		};

		void RenderEngine::CreateDevice()
		{
			unsigned createDeviceFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
			//createDeviceFlags |=  D3D11_CREATE_DEVICE_DEBUG;
#endif

			HRESULT hr = D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				createDeviceFlags,
				nullptr,
				0,
				D3D11_SDK_VERSION,
				&device_,
				nullptr,
				&deviceContext_
				);

			if (hr != 0)
			{
				puts("D3D11CreateDevice failed.");

				SAFE_RELEASE(device_);
				SAFE_RELEASE(deviceContext_);

				return;
			}
		}

		void RenderEngine::CreateRasterizerState()
		{
			D3D11_RASTERIZER_DESC stateDesc;
			memset(&stateDesc, 0, sizeof(stateDesc));
			stateDesc.FillMode = D3D11_FILL_SOLID;
			stateDesc.CullMode = D3D11_CULL_BACK;
			stateDesc.FrontCounterClockwise = false;
			stateDesc.DepthBias = 0;
			stateDesc.DepthBiasClamp = 0.0f;

			stateDesc.SlopeScaledDepthBias = 0;
			stateDesc.DepthClipEnable = false;
			stateDesc.ScissorEnable = false;
			stateDesc.MultisampleEnable = false;
			stateDesc.AntialiasedLineEnable = false;

			HRESULT hr = device_->CreateRasterizerState(&stateDesc, &rasterizerState_);
			if (hr != 0)
			{
				puts("CreateRasterizerState failed.");

				SAFE_RELEASE(rasterizerState_);

				return;
			}

			deviceContext_->RSSetState(rasterizerState_);
		}

		void RenderEngine::CreateMatrixData()
		{
			D3D11_BUFFER_DESC bufferDesc;
			memset(&bufferDesc, 0, sizeof(bufferDesc));
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth = sizeof(MatrixData);
			HRESULT hr = RenderEngine::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &constBuffer_[CONST_BUFFER_MATRIX]);
			if (FAILED(hr))
			{
				FLAGGG_LOG_ERROR("Failed to Create ConstBuffer for Normal MatrixData.");
				SAFE_RELEASE(constBuffer_[CONST_BUFFER_MATRIX]);
				return;
			}

			// Ä¬ÈÏ×î´ó¹Ç÷ÀÊý
			bufferDesc.ByteWidth = sizeof(Math::Matrix3x4) * GetMaxBonesNum();
			hr = RenderEngine::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &constBuffer_[CONST_BUFFER_SKIN]);
			if (FAILED(hr))
			{
				FLAGGG_LOG_ERROR("Failed to Create ConstBuffer for Skin MatrixData.");
				SAFE_RELEASE(constBuffer_[CONST_BUFFER_SKIN]);
				return;
			}
		}

		void RenderEngine::Initialize()
		{
			CreateDevice();

			CreateRasterizerState();

			CreateMatrixData();
		}

		void RenderEngine::Uninitialize()
		{
			SAFE_RELEASE(device_);
			SAFE_RELEASE(deviceContext_);
			SAFE_RELEASE(rasterizerState_);
			for (uint32_t i = 0; i < MAX_CONST_BUFFER_COUNT; ++i)
			{
				SAFE_RELEASE(constBuffer_[i]);
			}
		}

		ID3D11Device* RenderEngine::GetDevice()
		{
			return device_;
		}

		ID3D11DeviceContext* RenderEngine::GetDeviceContext()
		{
			return deviceContext_;
		}

		bool RenderEngine::CheckMultiSampleSupport(DXGI_FORMAT format, uint32_t sampleCount)
		{
			if (sampleCount < 2)
			{
				return true;
			}

			UINT numLevels = 0;
			if (FAILED(GetDevice()->CheckMultisampleQualityLevels(format, sampleCount, &numLevels)))
			{
				return false;
			}

			return numLevels > 0;
		}

		uint32_t RenderEngine::GetMultiSampleQuality(DXGI_FORMAT format, uint32_t sampleCount)
		{
			if (sampleCount < 2)
			{
				return 0;
			}

			if (GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_1)
			{
				return 0xffffffff;
			}

			UINT numLevels = 0;
			if (FAILED(GetDevice()->CheckMultisampleQualityLevels(format, sampleCount, &numLevels)) || !numLevels)
			{
				return 0;
			}

			return numLevels - 1;
		}

		void RenderEngine::SetTextureQuality(MaterialQuality quality)
		{
			textureQuality_ = quality;
		}

		MaterialQuality RenderEngine::GetTextureQuality()
		{
			return textureQuality_;
		}

		uint32_t RenderEngine::GetFormat(Resource::CompressedFormat format)
		{
			switch (format)
			{
			case Resource::CF_RGBA:
				return DXGI_FORMAT_R8G8B8A8_UNORM;

			case Resource::CF_DXT1:
				return DXGI_FORMAT_BC1_UNORM;

			case Resource::CF_DXT3:
				return DXGI_FORMAT_BC2_UNORM;

			case Resource::CF_DXT5:
				return DXGI_FORMAT_BC3_UNORM;

			default:
				return 0;
			}
		}

		uint32_t RenderEngine::GetAlphaFormat()
		{
			return DXGI_FORMAT_A8_UNORM;
		}

		uint32_t RenderEngine::GetLuminanceFormat()
		{
			return DXGI_FORMAT_R8_UNORM;
		}

		uint32_t RenderEngine::GetLuminanceAlphaFormat()
		{
			return DXGI_FORMAT_R8G8_UNORM;
		}

		uint32_t RenderEngine::GetRGBFormat()
		{
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		}

		uint32_t RenderEngine::GetRGBAFormat()
		{
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		}

		uint32_t RenderEngine::GetRGBA16Format()
		{
			return DXGI_FORMAT_R16G16B16A16_UNORM;
		}

		uint32_t RenderEngine::GetRGBAFloat16Format()
		{
			return DXGI_FORMAT_R16G16B16A16_FLOAT;
		}

		uint32_t RenderEngine::GetRGBAFloat32Format()
		{
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		uint32_t RenderEngine::GetRG16Format()
		{
			return DXGI_FORMAT_R16G16_UNORM;
		}

		uint32_t RenderEngine::GetRGFloat16Format()
		{
			return DXGI_FORMAT_R16G16_FLOAT;
		}

		uint32_t RenderEngine::GetRGFloat32Format()
		{
			return DXGI_FORMAT_R32G32_FLOAT;
		}

		uint32_t RenderEngine::GetFloat16Format()
		{
			return DXGI_FORMAT_R16_FLOAT;
		}

		uint32_t RenderEngine::GetFloat32Format()
		{
			return DXGI_FORMAT_R32_FLOAT;
		}

		uint32_t RenderEngine::GetLinearDepthFormat()
		{
			return DXGI_FORMAT_R32_FLOAT;
		}

		uint32_t RenderEngine::GetDepthStencilFormat()
		{
			return DXGI_FORMAT_R24G8_TYPELESS;
		}

		uint32_t RenderEngine::GetReadableDepthFormat()
		{
			return DXGI_FORMAT_R24G8_TYPELESS;
		}

		uint32_t RenderEngine::GetFormat(const Container::String& formatName)
		{
			Container::String nameLower = formatName.ToLower().Trimmed();

			if (nameLower == "a")
				return GetAlphaFormat();
			if (nameLower == "l")
				return GetLuminanceFormat();
			if (nameLower == "la")
				return GetLuminanceAlphaFormat();
			if (nameLower == "rgb")
				return GetRGBFormat();
			if (nameLower == "rgba")
				return GetRGBAFormat();
			if (nameLower == "rgba16")
				return GetRGBA16Format();
			if (nameLower == "rgba16f")
				return GetRGBAFloat16Format();
			if (nameLower == "rgba32f")
				return GetRGBAFloat32Format();
			if (nameLower == "rg16")
				return GetRG16Format();
			if (nameLower == "rg16f")
				return GetRGFloat16Format();
			if (nameLower == "rg32f")
				return GetRGFloat32Format();
			if (nameLower == "r16f")
				return GetFloat16Format();
			if (nameLower == "r32f" || nameLower == "float")
				return GetFloat32Format();
			if (nameLower == "lineardepth" || nameLower == "depth")
				return GetLinearDepthFormat();
			if (nameLower == "d24s8")
				return GetDepthStencilFormat();
			if (nameLower == "readabledepth" || nameLower == "hwdepth")
				return GetReadableDepthFormat();

			return GetRGBFormat();
		}

		uint32_t RenderEngine::GetMaxBonesNum()
		{
			return 64;
		}

		void RenderEngine::UpdateMatrix(Camera* camera, const Math::Matrix3x4* worldTransform, uint32_t num)
		{
			if (!camera) return;
			if (!worldTransform || !num) return;

			D3D11_MAPPED_SUBRESOURCE mappedResource;

			// ÆÕÍ¨×ø±ê¾ØÕó
			RenderEngine::GetDeviceContext()->Map(constBuffer_[CONST_BUFFER_MATRIX], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			MatrixData* dataPtr = static_cast<MatrixData*>(mappedResource.pData);
			dataPtr->world = Math::Matrix4::IDENTITY.Transpose();
			dataPtr->view = camera->GetViewMatrix().Transpose();
			dataPtr->projection = camera->GetProjectionMatrix().Transpose();
			RenderEngine::GetDeviceContext()->Unmap(constBuffer_[CONST_BUFFER_MATRIX], 0);

			// ÃÉÆ¤¾ØÕó
			RenderEngine::GetDeviceContext()->Map(constBuffer_[CONST_BUFFER_SKIN], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			uint32_t realNum = Math::Min(GetMaxBonesNum(), num);
			memcpy(mappedResource.pData, worldTransform, sizeof(Math::Matrix3x4) * realNum);
			RenderEngine::GetDeviceContext()->Unmap(constBuffer_[CONST_BUFFER_SKIN], 0);

			unsigned int bufferNumber = MAX_CONST_BUFFER;
			RenderEngine::GetDeviceContext()->VSSetConstantBuffers(0, bufferNumber, constBuffer_);
		}

		void RenderEngine::Render(Viewport* viewport)
		{
			if (!viewport) return;

			viewport->SetViewport();

			ID3D11DeviceContext* deviceContext = RenderEngine::GetDeviceContext();

			ID3D11RenderTargetView* renderTargetView = viewport->GetRenderTarget()->GetObject<ID3D11RenderTargetView>();
			ID3D11DepthStencilView* depthStencilView = viewport->GetDepthStencil()->GetObject<ID3D11DepthStencilView>();
			deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

			float color[] = { 0.0, 0.0f, 0.0f, 1.0f };
			deviceContext->ClearRenderTargetView(renderTargetView, color);
			deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);

			Scene::Scene* scene = viewport->GetScene();
			Container::Vector<RenderContext> renderContexts;
			scene->Render(renderContexts);

			for (const auto& renderContext : renderContexts)
			{
				RenderEngine::UpdateMatrix(viewport->GetCamera(), renderContext.worldTransform_, renderContext.numWorldTransform_);

				uint32_t vertexCount = 0;
				uint32_t vertexSize = 0;
				uint32_t vertexOffset = 0;
				uint32_t vertexBufferCount = Math::Min<uint32_t>(renderContext.vertexBuffers_->Size(), MAX_VERTEX_BUFFER_COUNT);
				
				VertexBuffer* tempBuffer[MAX_VERTEX_BUFFER_COUNT + 1] = { 0 };
				
				for (uint32_t i = 0; i < vertexBufferCount; ++i)
				{
					const Container::SharedPtr<VertexBuffer>& vertexBuffer = (*renderContext.vertexBuffers_)[i];
					vertexBuffers_[i] = vertexBuffer->GetObject<ID3D11Buffer>();
					vertexSize = vertexBuffer->GetVertexSize();
					vertexCount += vertexBuffer->GetVertexCount();

					tempBuffer[i] = vertexBuffer.Get();
				}

				VertexFormat* vertexFormat = CacheVertexFormat(renderContext.VSShader_, tempBuffer);

				deviceContext->IASetVertexBuffers(0, vertexBufferCount, vertexBuffers_, &vertexSize, &vertexOffset);			
				//deviceContext->VSSetShaderResources(0, 1, &renderContext.texture_->shaderResourceView_);
				//deviceContext->VSSetSamplers(0, 1, &batch.GetTexture()->sampler_);
				deviceContext->IASetInputLayout(vertexFormat->GetObject<ID3D11InputLayout>());
				deviceContext->VSSetShader(renderContext.VSShader_->GetObject<ID3D11VertexShader>(), nullptr, 0);
				deviceContext->PSSetShader(renderContext.PSShader_->GetObject<ID3D11PixelShader>(), nullptr, 0);
				deviceContext->PSSetShaderResources(0, 1, &renderContext.texture_->shaderResourceView_);
				deviceContext->PSSetSamplers(0, 1, &renderContext.texture_->sampler_);

				switch (renderContext.primitiveType_)
				{
				case PRIMITIVE_LINE:
					deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
					break;

				case PRIMITIVE_TRIANGLE:
					deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					break;
				}

				deviceContext->Draw(vertexCount, 0);
			}
		}

		VertexFormat* RenderEngine::CacheVertexFormat(Shader* VSShader, VertexBuffer** vertexBuffer)
		{
			auto hashValue = reinterpret_cast<uint64_t>(VSShader);
			auto it = vertexFormatCache_.Find(hashValue);
			if (it != vertexFormatCache_.End())
			{
				return it->second_;
			}

			Container::SharedPtr<VertexFormat> vertexFormat(new VertexFormat(VSShader, vertexBuffer));
			vertexFormatCache_[hashValue] = vertexFormat;

			return vertexFormat;
		}
	}
}