#include "Graphics/RenderEngine.h"
#include "Graphics/Camera.h"
#include "Graphics/Texture.h"
#include "Scene/Light.h"
#include "Math/Math.h"
#include "Log.h"
#include "IOFrame/Buffer/IOBufferAux.h"

#include <assert.h>

namespace FlagGG
{
	namespace Graphics
	{
		struct MatrixData
		{
			Math::Matrix3x4 world_;
			Math::Matrix4	view_;
			Math::Matrix4	projection_;
		};

		RenderEngine::RenderEngine()
		{
			INIT_ARRAY(constGPUBuffer_, nullptr);
			INIT_ARRAY(vertexBuffers_, nullptr);
			INIT_ARRAY(vertexSize_, 0);
			INIT_ARRAY(vertexOffset_, 0);
			INIT_ARRAY(shaderResourceView_, nullptr);
			INIT_ARRAY(samplerState_, nullptr);

			shaderParameters_.AddParametersDefine<float>(SP_DELTA_TIME);
			shaderParameters_.AddParametersDefine<float>(SP_ELAPSED_TIME);
			shaderParameters_.AddParametersDefine<Math::Vector3>(SP_CAMERA_POS);
			shaderParameters_.AddParametersDefine<Math::Vector3>(SP_LIGHT_POS);
			shaderParameters_.AddParametersDefine<Math::Vector3>(SP_LIGHT_DIR);
		}

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
				FLAGGG_LOG_ERROR("D3D11CreateDevice failed.");

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
				FLAGGG_LOG_ERROR("CreateRasterizerState failed.");

				SAFE_RELEASE(rasterizerState_);

				return;
			}

			deviceContext_->RSSetState(rasterizerState_);
		}

		void RenderEngine::Initialize()
		{
			CreateDevice();

			CreateRasterizerState();
		}

		void RenderEngine::Uninitialize()
		{
			SAFE_RELEASE(device_);
			SAFE_RELEASE(deviceContext_);
			SAFE_RELEASE(rasterizerState_);
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

		ShaderParameters& RenderEngine::GetShaderParameters()
		{
			return shaderParameters_;
		}

		void RenderEngine::SetShaderParameter(Camera* camera, const RenderContext* renderContext)
		{
			if (!renderContext)
				return;

			for (uint32_t i = 0; i < MAX_CONST_BUFFER; ++i)
			{
				constGPUBuffer_[i] = nullptr;
			}

			// 世界信息，蒙皮信息
			if (camera && renderContext->worldTransform_ && renderContext->numWorldTransform_)
			{
				auto* buffer = constBuffer_[CONST_BUFFER_WORLD].LockDynamicBuffer();
				IOFrame::Buffer::WriteMatrix3x4(buffer, *renderContext->worldTransform_);
				IOFrame::Buffer::WriteMatrix4x4(buffer, camera->GetViewMatrix().Transpose());
				IOFrame::Buffer::WriteMatrix4x4(buffer, camera->GetProjectionMatrix().Transpose());
				constBuffer_[CONST_BUFFER_WORLD].UnlockDynamicBuffer();
				constGPUBuffer_[CONST_BUFFER_WORLD] = constBuffer_[CONST_BUFFER_WORLD].GetObject<ID3D11Buffer>();

				if (renderContext->geometryType_ == GEOMETRY_SKINNED)
				{
					uint32_t realNum = Math::Min(GetMaxBonesNum(), renderContext->numWorldTransform_);
					uint32_t dataSize = realNum * sizeof(Math::Matrix3x4);
					constBuffer_[CONST_BUFFER_SKIN].SetSize(dataSize);
					auto* buffer = constBuffer_[CONST_BUFFER_SKIN].LockStaticBuffer(0, dataSize);
					buffer->WriteStream(renderContext->worldTransform_, dataSize);
					constBuffer_[CONST_BUFFER_SKIN].UnlockStaticBuffer();
					constGPUBuffer_[CONST_BUFFER_SKIN] = constBuffer_[CONST_BUFFER_SKIN].GetObject<ID3D11Buffer>();
				}
			}

			// 通用的Shader参数
			shaderParameters_.WriteToBuffer(&constBuffer_[CONST_BUFFER_COMMON]);
			constGPUBuffer_[CONST_BUFFER_COMMON] = constBuffer_[CONST_BUFFER_COMMON].GetObject<ID3D11Buffer>();

			deviceContext_->VSSetConstantBuffers(0, MAX_CONST_BUFFER, constGPUBuffer_);
		}

		void RenderEngine::SetVertexBuffers(const Container::Vector<Container::SharedPtr<VertexBuffer>>& vertexBuffers)
		{
			cacheVertexBuffers_ = &vertexBuffers;

			auto vertexBufferCount = Math::Min<uint32_t>(vertexBuffers.Size(), MAX_VERTEX_BUFFER_COUNT);

			for (uint32_t i = 0; i < vertexBufferCount; ++i)
			{
				const Container::SharedPtr<VertexBuffer>& vertexBuffer = vertexBuffers[i];
				vertexBuffers_[i] = vertexBuffer->GetObject<ID3D11Buffer>();
				vertexSize_[i] = vertexBuffer->GetVertexSize();
				vertexOffset_[i] = 0;
			}

			deviceContext_->IASetVertexBuffers(0, vertexBufferCount, vertexBuffers_, vertexSize_, vertexOffset_);
		}

		void RenderEngine::SetIndexBuffer(IndexBuffer* indexBuffer)
		{
			deviceContext_->IASetIndexBuffer(indexBuffer->GetObject<ID3D11Buffer>(),
				indexBuffer->GetIndexSize() == sizeof(uint16_t) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
		}

		void RenderEngine::SetVertexShader(Shader* shader)
		{
			cacheVSShader_ = shader;
			deviceContext_->VSSetShader(shader->GetObject<ID3D11VertexShader>(), nullptr, 0);
		}

		void RenderEngine::SetPixelShader(Shader* shader)
		{
			deviceContext_->PSSetShader(shader->GetObject<ID3D11PixelShader>(), nullptr, 0);
		}

		void RenderEngine::SetTexture(Texture* texture)
		{
			//deviceContext->VSSetShaderResources(0, 1, &texture->shaderResourceView_);
			//deviceContext->VSSetSamplers(0, 1, &texture->sampler_);	
			deviceContext_->PSSetShaderResources(0, 1, &texture->shaderResourceView_);
			deviceContext_->PSSetSamplers(0, 1, &texture->sampler_);
		}

		void RenderEngine::SetTextures(const Container::Vector<Container::SharedPtr<Texture>>& textures)
		{
			for (uint32_t i = 0; i < MAX_TEXTURE_CLASS; ++i)
			{
				if (i < textures.Size() && textures[i])
				{
					shaderResourceView_[i] = textures[i]->shaderResourceView_;
					samplerState_[i] = textures[i]->sampler_;
				}
				else
				{
					shaderResourceView_[i] = defaultTextures[i] ? defaultTextures[i]->shaderResourceView_ : nullptr;
					samplerState_[i] = defaultTextures[i] ? defaultTextures[i]->sampler_ : nullptr;
				}
			}
			//deviceContext->VSSetShaderResources(0, MAX_TEXTURE_CLASS, shaderResourceView_);
			//deviceContext->VSSetSamplers(0, MAX_TEXTURE_CLASS, samplerState_);
			deviceContext_->PSSetShaderResources(0, MAX_TEXTURE_CLASS, shaderResourceView_);
			deviceContext_->PSSetSamplers(0, MAX_TEXTURE_CLASS, samplerState_);
		}

		void RenderEngine::SetDefaultTextures(TextureClass index, Texture* texture)
		{
			defaultTextures[index] = texture;
		}

		void RenderEngine::SetPrimitiveType(PrimitiveType primitiveType)
		{
			switch (primitiveType)
			{
			case PRIMITIVE_LINE:
				deviceContext_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
				break;

			case PRIMITIVE_TRIANGLE:
				deviceContext_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				break;
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

		void RenderEngine::DrawCall(uint32_t indexStart, uint32_t indexCount)
		{
			auto vertexBufferCount = Math::Min<uint32_t>(cacheVertexBuffers_->Size(), MAX_VERTEX_BUFFER_COUNT);
			VertexBuffer* tempBuffer[MAX_VERTEX_BUFFER_COUNT + 1] = { 0 };
			for (uint32_t i = 0; i < vertexBufferCount; ++i)
			{
				tempBuffer[i] = (*cacheVertexBuffers_)[i].Get();
			}
			VertexFormat* vertexFormat = CacheVertexFormat(cacheVSShader_, tempBuffer);

			deviceContext_->IASetInputLayout(vertexFormat->GetObject<ID3D11InputLayout>());
			deviceContext_->DrawIndexed(indexCount, indexStart, 0);
		}

		void RenderEngine::SetRenderTarget(Viewport* viewport)
		{
			auto* renderTargetView = viewport->GetRenderTarget()->GetObject<ID3D11RenderTargetView>();
			auto* depthStencilView = viewport->GetDepthStencil()->GetObject<ID3D11DepthStencilView>();
			static const Math::Color color(0.0f, 0.0f, 0.0f, 1.0f);

			deviceContext_->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
			deviceContext_->ClearRenderTargetView(renderTargetView, color.Data());
			deviceContext_->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);
		}

		void RenderEngine::SetShaderMap()
		{
			if (defaultTextures[TEXTURE_CLASS_SHADOWMAP])
			{
				auto* shadowMap = defaultTextures[TEXTURE_CLASS_SHADOWMAP]->GetRenderSurface()->GetObject<ID3D11DepthStencilView>();
				deviceContext_->OMSetRenderTargets(0, nullptr, shadowMap);
				deviceContext_->ClearDepthStencilView(shadowMap, D3D11_CLEAR_DEPTH, 1.0, 0);
			}
		}

		void RenderEngine::Render(Viewport* viewport)
		{
			if (!viewport) return;

			Scene::Scene* scene = viewport->GetScene();
			Container::PODVector<RenderContext*> renderContexts;
			scene->Render(renderContexts);

			viewport->SetViewport();

			Container::PODVector<Scene::Light*> lights;
			scene->GetLights(lights);
			for (auto light : lights)
			{
				SetShaderMap();
				for (const auto& renderContext : renderContexts)
				{
					if (!renderContext->renderPass_)
						continue;
					auto it = renderContext->renderPass_->Find(RENDER_PASS_TYPE_SHADOW);
					if (it != renderContext->renderPass_->End())
					{
						SetShaderParameter(light->GetCamera(), renderContext);
						SetVertexShader(it->second_.vertexShader_);
						SetPixelShader(it->second_.pixelShader_);
						for (const auto& geometry : renderContext->geometries_)
						{
							SetVertexBuffers(geometry->GetVertexBuffers());
							SetIndexBuffer(geometry->GetIndexBuffer());
							SetPrimitiveType(geometry->GetPrimitiveType());
							DrawCall(geometry->GetIndexStart(), geometry->GetIndexCount());
						}
					}
				}
			}

			SetRenderTarget(viewport);
			for (const auto& renderContext : renderContexts)
			{
				SetShaderParameter(viewport->GetCamera(), renderContext);
				SetVertexShader(renderContext->vertexShader_);
				SetPixelShader(renderContext->pixelShader_);
				if (renderContext->texture_)
					SetTexture(renderContext->texture_);
				else
					SetTextures(renderContext->textures_);

				for (const auto& geometry : renderContext->geometries_)
				{
					SetVertexBuffers(geometry->GetVertexBuffers());
					SetIndexBuffer(geometry->GetIndexBuffer());
					SetPrimitiveType(geometry->GetPrimitiveType());
					DrawCall(geometry->GetIndexStart(), geometry->GetIndexCount());
				}
			}
		}

		RenderEngine* RenderEngine::Instance()
		{
			if (!renderEngine_)
				renderEngine_ = new RenderEngine();

			return renderEngine_;
		}

		RenderEngine* RenderEngine::renderEngine_ = nullptr;
	}
}