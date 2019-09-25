#include "Graphics/RenderEngine.h"
#include "Graphics/Camera.h"
#include "Graphics/Texture.h"
#include "Scene/Node.h"
#include "Scene/Component.h"
#include "Scene/Light.h"
#include "Math/Math.h"
#include "Log.h"
#include "IOFrame/Buffer/IOBufferAux.h"

#include <assert.h>

namespace FlagGG
{
	namespace Graphics
	{
		RenderEngine::RenderEngine()
		{
			shaderParameters_.AddParametersDefine<Math::Matrix3x4>(SP_WORLD_MATRIX);
			shaderParameters_.AddParametersDefine<Math::Matrix4>(SP_VIEW_MATRIX);
			shaderParameters_.AddParametersDefine<Math::Matrix4>(SP_PROJECTION_MATRIX);
			shaderParameters_.AddParametersDefine<float>(SP_DELTA_TIME);
			shaderParameters_.AddParametersDefine<float>(SP_ELAPSED_TIME);
			shaderParameters_.AddParametersDefine<Math::Vector3>(SP_CAMERA_POS);
			shaderParameters_.AddParametersDefine<Math::Vector3>(SP_LIGHT_POS);
			shaderParameters_.AddParametersDefine<Math::Vector3>(SP_LIGHT_DIR);
			shaderParameters_.AddParametersDefine<Math::Matrix4>(SP_LIGHT_VIEW_MATRIX);
			shaderParameters_.AddParametersDefine<Math::Matrix4>(SP_LIGHT_PROJ_MATRIX);
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

			// 视图矩阵，投影矩阵，蒙皮矩阵等
			if (camera && renderContext->worldTransform_ && renderContext->numWorldTransform_)
			{
				shaderParameters_.SetValue(SP_WORLD_MATRIX, *renderContext->worldTransform_);
				shaderParameters_.SetValue(SP_VIEW_MATRIX, camera->GetViewMatrix().Transpose());
				shaderParameters_.SetValue(SP_PROJECTION_MATRIX, camera->GetProjectionMatrix().Transpose());
				shaderParameters_.SetValue(SP_CAMERA_POS, camera->GetPosition());

				if (renderContext->geometryType_ == GEOMETRY_SKINNED)
				{
					skinMatrix_ = renderContext->worldTransform_;
					numSkinMatrix_ = renderContext->numWorldTransform_;
				}
			}
		}

		void RenderEngine::SetVertexBuffers(const Container::Vector<Container::SharedPtr<VertexBuffer>>& vertexBuffers)
		{
			for (auto vertexBuffer : vertexBuffers)
				vertexBuffers_.Push(vertexBuffer);
			vertexBufferDirty_ = true;
		}

		void RenderEngine::SetIndexBuffer(IndexBuffer* indexBuffer)
		{
			indexBuffer_ = indexBuffer;
			indexBufferDirty_ = true;
		}

		void RenderEngine::SetVertexShader(Shader* shader)
		{
			vertexShader_ = shader;
			vertexShaderDirty_ = true;
		}

		void RenderEngine::SetPixelShader(Shader* shader)
		{
			pixelShader_ = shader;
			pixelShaderDirty_ = true;
		}

		void RenderEngine::SetTextures(const Container::Vector<Container::SharedPtr<Texture>>& textures)
		{
			for (uint32_t i = 0; i < MAX_TEXTURE_CLASS; ++i)
			{
				textures_[i] = i < textures.Size() && textures[i] ? textures[i] : defaultTextures_[i];
			}

			texturesDirty_ = true;
		}

		void RenderEngine::SetDefaultTextures(TextureClass index, Texture* texture)
		{
			defaultTextures_[index] = texture;
		}

		void RenderEngine::SetPrimitiveType(PrimitiveType primitiveType)
		{
			primitiveType_ = primitiveType;
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
			static VertexBuffer* vertexBuffers[MAX_VERTEX_BUFFER_COUNT + 1] = { 0 };

			// 设置图元类型
			switch (primitiveType_)
			{
			case PRIMITIVE_LINE:
				deviceContext_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
				break;

			case PRIMITIVE_TRIANGLE:
				deviceContext_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				break;
			}

			// 设置vertexBuffer
			if (vertexBufferDirty_)
			{
				static ID3D11Buffer* d3dVertexBuffers[MAX_VERTEX_BUFFER_COUNT] = { nullptr };
				static uint32_t d3dVertexBufferCount{ 0 };
				static uint32_t d3dVertexSize[MAX_VERTEX_BUFFER_COUNT] = { 0 };
				static uint32_t d3dVertexOffset[MAX_VERTEX_BUFFER_COUNT] = { 0 };

				d3dVertexBufferCount = Math::Min<uint32_t>(vertexBuffers_.Size(), MAX_VERTEX_BUFFER_COUNT);

				for (uint32_t i = 0; i < d3dVertexBufferCount; ++i)
				{
					const Container::SharedPtr<VertexBuffer>& vertexBuffer = vertexBuffers_[i];
					vertexBuffers[i] = vertexBuffer;
					d3dVertexBuffers[i] = vertexBuffer->GetObject<ID3D11Buffer>();
					d3dVertexSize[i] = vertexBuffer->GetVertexSize();
					d3dVertexOffset[i] = 0;
				}

				deviceContext_->IASetVertexBuffers(0, d3dVertexBufferCount, d3dVertexBuffers, d3dVertexSize, d3dVertexOffset);
				vertexBufferDirty_ = false;
			}


			// 设置indexBuffer
			if (indexBufferDirty_)
			{
				deviceContext_->IASetIndexBuffer(indexBuffer_->GetObject<ID3D11Buffer>(),
					indexBuffer_->GetIndexSize() == sizeof(uint16_t) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
				indexBufferDirty_ = false;
			}

			// 设置vertexShader
			if (vertexShaderDirty_)
			{
				deviceContext_->VSSetShader(vertexShader_->GetObject<ID3D11VertexShader>(), nullptr, 0);
				vertexShaderDirty_ = false;
			}

			// 设置pxielShader
			if (pixelShaderDirty_)
			{
				deviceContext_->PSSetShader(pixelShader_->GetObject<ID3D11PixelShader>(), nullptr, 0);
				pixelShaderDirty_ = false;
			}

			// 设置constBuffer
			{
				CopyShaderParameterToBuffer(vertexShader_, vsConstantBuffer_);
				CopyShaderParameterToBuffer(pixelShader_, psConstantBuffer_);

				static ID3D11Buffer* d3dVSConstantBuffer[MAX_CONST_BUFFER_COUNT] = { nullptr };
				static ID3D11Buffer* d3dPSConstantBuffer[MAX_CONST_BUFFER_COUNT] = { nullptr };
				for (uint32_t i = 0; i < MAX_CONST_BUFFER; ++i)
				{
					d3dVSConstantBuffer[i] = vsConstantBuffer_[i].GetObject<ID3D11Buffer>();
					d3dPSConstantBuffer[i] = psConstantBuffer_[i].GetObject<ID3D11Buffer>();
				}

				deviceContext_->VSSetConstantBuffers(0, MAX_CONST_BUFFER, d3dVSConstantBuffer);
				deviceContext_->PSSetConstantBuffers(0, MAX_CONST_BUFFER, d3dPSConstantBuffer);
			}

			// 设置vertex格式
			VertexFormat* vertexFormat = CacheVertexFormat(vertexShader_, vertexBuffers);
			deviceContext_->IASetInputLayout(vertexFormat->GetObject<ID3D11InputLayout>());

			// 设置纹理
			if (texturesDirty_)
			{
				static ID3D11ShaderResourceView* shaderResourceView[MAX_TEXTURE_CLASS] = { nullptr };
				static ID3D11SamplerState* samplerState[MAX_TEXTURE_CLASS] = { nullptr };

				for (uint32_t i = 0; i < MAX_TEXTURE_CLASS; ++i)
				{
					if (textures_[i])
					{
						shaderResourceView[i] = textures_[i]->shaderResourceView_;
						samplerState[i] = textures_[i]->sampler_;
					}
					else
					{
						shaderResourceView[i] = nullptr;
						samplerState[i] = nullptr;
					}
				}

				//deviceContext->VSSetShaderResources(0, MAX_TEXTURE_CLASS, shaderResourceView_);
				//deviceContext->VSSetSamplers(0, MAX_TEXTURE_CLASS, samplerState_);
				deviceContext_->PSSetShaderResources(0, MAX_TEXTURE_CLASS, shaderResourceView);
				deviceContext_->PSSetSamplers(0, MAX_TEXTURE_CLASS, samplerState);
				texturesDirty_ = false;
			}

			// 设置渲染表面和深度模板
			if (renderTargetDirty_)
			{
				auto* renderTargetView = renderTarget_->GetObject<ID3D11RenderTargetView>();
				auto* depthStencilView = depthStencil_->GetObject<ID3D11DepthStencilView>();
				Math::Color color(0.0f, 0.0f, 0.0f, 1.0f);

				if (renderShadowMap_)
				{
					renderTargetView = defaultTextures_[TEXTURE_CLASS_SHADOWMAP]->GetRenderSurface()->GetObject<ID3D11RenderTargetView>();
					color = Math::Color(1.0f, 1.0f, 1.0f, 1.0f);
				}

				deviceContext_->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
				deviceContext_->ClearRenderTargetView(renderTargetView, color.Data());
				deviceContext_->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);
				renderTargetDirty_ = false;
			}

			// draw call
			deviceContext_->DrawIndexed(indexCount, indexStart, 0);
		}

		void RenderEngine::CopyShaderParameterToBuffer(Shader* shader, ConstantBuffer* buffer)
		{
			const auto& constantBufferVariableDesc = shader->GetContantBufferVariableDesc();
			for (auto it = constantBufferVariableDesc.Begin(); it != constantBufferVariableDesc.End(); ++it)
			{
				if (it->first_ < MAX_CONST_BUFFER)
				{
					auto& constantBuffer = buffer[it->first_];
					const auto& bufferDesc = it->second_;
					constantBuffer.SetSize(bufferDesc.size_);
					char* data = static_cast<char*>(constantBuffer.Lock(0, bufferDesc.size_));
					for (const auto& variableDesc : bufferDesc.variableDescs_)
					{
						auto it2 = shaderParameters_.descs.Find(variableDesc.name_);
						if (it2 != shaderParameters_.descs.End())
						{
							shaderParameters_.dataBuffer_->Seek(it2->second_.offset_);
							shaderParameters_.dataBuffer_->ReadStream(data + variableDesc.offset_,
								Math::Min(variableDesc.size_, it2->second_.size_));
						}
						else if (Container::StringHash(variableDesc.name_) == SP_SKIN_MATRICES)
						{
							memcpy(data + variableDesc.offset_, skinMatrix_,
								Math::Min(variableDesc.size_, numSkinMatrix_ * sizeof(Math::Matrix3x4)));
						}
					}
					constantBuffer.Unlock();
				}
			}
		}

		void RenderEngine::SetRenderTarget(Viewport* viewport, bool renderShadowMap)
		{
			renderTarget_ = viewport->GetRenderTarget();
			depthStencil_ = viewport->GetDepthStencil();
			renderShadowMap_ = renderShadowMap;
			renderTargetDirty_ = true;
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
			SetRenderTarget(viewport, true);
			for (auto light : lights)
			{
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

			SetRenderTarget(viewport, false);
			for (const auto& renderContext : renderContexts)
			{
				if (lights.Size() > 0 &&
					renderContext->renderPass_ &&
					renderContext->renderPass_->Contains(RENDER_PASS_TYPE_SHADOW))
				{
					Scene::Node* lightNode = lights[0]->GetNode();
					Camera* lightCamera = lights[0]->GetCamera();
					shaderParameters_.SetValue(SP_LIGHT_POS, lightNode->GetWorldPosition());
					shaderParameters_.SetValue(SP_LIGHT_DIR, lightNode->GetWorldRotation().EulerAngles().Normalized());
					shaderParameters_.SetValue(SP_LIGHT_VIEW_MATRIX, lightCamera->GetViewMatrix().Transpose());
					shaderParameters_.SetValue(SP_LIGHT_PROJ_MATRIX, lightCamera->GetProjectionMatrix().Transpose());
				}

				SetShaderParameter(viewport->GetCamera(), renderContext);
				SetVertexShader(renderContext->vertexShader_);
				SetPixelShader(renderContext->pixelShader_);
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