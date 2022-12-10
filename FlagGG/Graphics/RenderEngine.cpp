#include "Graphics/RenderEngine.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Scene/Node.h"
#include "Scene/Component.h"
#include "Scene/Light.h"
#include "Math/Math.h"
#include "Log.h"
#include "IOFrame/Buffer/IOBufferAux.h"
#include "Resource/ResourceCache.h"

#include <assert.h>

namespace FlagGG
{

RenderEngine::RenderEngine(Context* context) :
	context_(context)
{
	shaderParameters_.AddParametersDefine<Matrix3x4>(SP_WORLD_MATRIX);
	shaderParameters_.AddParametersDefine<Matrix3x4>(SP_VIEW_MATRIX);
	shaderParameters_.AddParametersDefine<Matrix4>(SP_PROJVIEW_MATRIX);
	shaderParameters_.AddParametersDefine<float>(SP_DELTA_TIME);
	shaderParameters_.AddParametersDefine<float>(SP_ELAPSED_TIME);
	shaderParameters_.AddParametersDefine<Vector3>(SP_CAMERA_POS);
	shaderParameters_.AddParametersDefine<Vector3>(SP_LIGHT_POS);
	shaderParameters_.AddParametersDefine<Vector3>(SP_LIGHT_DIR);
	shaderParameters_.AddParametersDefine<Matrix3x4>(SP_LIGHT_VIEW_MATRIX);
	shaderParameters_.AddParametersDefine<Matrix4>(SP_LIGHT_PROJVIEW_MATRIX);
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

void RenderEngine::CreateShadowRasterizerState()
{
	shadowRasterizerState_.depthWrite_ = true;
	shadowRasterizerState_.scissorTest_ = false;
	shadowRasterizerState_.fillMode_ = FILL_SOLID;
	shadowRasterizerState_.cullMode_ = CULL_FRONT;
}

void RenderEngine::Initialize()
{
	CreateDevice();

	CreateShadowRasterizerState();
}

void RenderEngine::Uninitialize()
{
	SAFE_RELEASE(device_);
	SAFE_RELEASE(deviceContext_);
	for (auto it = rasterizerStates_.Begin(); it != rasterizerStates_.End(); ++it)
	{
		SAFE_RELEASE(it->second_);
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

bool RenderEngine::CheckMultiSampleSupport(DXGI_FORMAT format, UInt32 sampleCount)
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

UInt32 RenderEngine::GetMultiSampleQuality(DXGI_FORMAT format, UInt32 sampleCount)
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

UInt32 RenderEngine::GetFormat(CompressedFormat format)
{
	switch (format)
	{
	case CF_RGBA:
		return DXGI_FORMAT_R8G8B8A8_UNORM;

	case CF_DXT1:
		return DXGI_FORMAT_BC1_UNORM;

	case CF_DXT3:
		return DXGI_FORMAT_BC2_UNORM;

	case CF_DXT5:
		return DXGI_FORMAT_BC3_UNORM;

	default:
		return 0;
	}
}

UInt32 RenderEngine::GetAlphaFormat()
{
	return DXGI_FORMAT_A8_UNORM;
}

UInt32 RenderEngine::GetLuminanceFormat()
{
	return DXGI_FORMAT_R8_UNORM;
}

UInt32 RenderEngine::GetLuminanceAlphaFormat()
{
	return DXGI_FORMAT_R8G8_UNORM;
}

UInt32 RenderEngine::GetRGBFormat()
{
	return DXGI_FORMAT_R8G8B8A8_UNORM;
}

UInt32 RenderEngine::GetRGBAFormat()
{
	return DXGI_FORMAT_R8G8B8A8_UNORM;
}

UInt32 RenderEngine::GetRGBA16Format()
{
	return DXGI_FORMAT_R16G16B16A16_UNORM;
}

UInt32 RenderEngine::GetRGBAFloat16Format()
{
	return DXGI_FORMAT_R16G16B16A16_FLOAT;
}

UInt32 RenderEngine::GetRGBAFloat32Format()
{
	return DXGI_FORMAT_R32G32B32A32_FLOAT;
}

UInt32 RenderEngine::GetRG16Format()
{
	return DXGI_FORMAT_R16G16_UNORM;
}

UInt32 RenderEngine::GetRGFloat16Format()
{
	return DXGI_FORMAT_R16G16_FLOAT;
}

UInt32 RenderEngine::GetRGFloat32Format()
{
	return DXGI_FORMAT_R32G32_FLOAT;
}

UInt32 RenderEngine::GetFloat16Format()
{
	return DXGI_FORMAT_R16_FLOAT;
}

UInt32 RenderEngine::GetFloat32Format()
{
	return DXGI_FORMAT_R32_FLOAT;
}

UInt32 RenderEngine::GetLinearDepthFormat()
{
	return DXGI_FORMAT_R32_FLOAT;
}

UInt32 RenderEngine::GetDepthStencilFormat()
{
	return DXGI_FORMAT_R24G8_TYPELESS;
}

UInt32 RenderEngine::GetReadableDepthFormat()
{
	return DXGI_FORMAT_R24G8_TYPELESS;
}

UInt32 RenderEngine::GetFormat(const String& formatName)
{
	String nameLower = formatName.ToLower().Trimmed();

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

UInt32 RenderEngine::GetMaxBonesNum()
{
	return 64;
}

ShaderParameters& RenderEngine::GetShaderParameters()
{
	return shaderParameters_;
}

void RenderEngine::PostRenderBatch(const Vector<SharedPtr<Batch>>& batches)
{
	batches_.Clear();
	for (const auto& batch : batches)
	{
		batches_.Push(batch);
	}
}

void RenderEngine::SetShaderParameter(Camera* camera, const RenderContext* renderContext)
{
	if (!renderContext)
		return;

	// 视图矩阵，投影矩阵，蒙皮矩阵等
	if (camera && renderContext->worldTransform_ && renderContext->numWorldTransform_)
	{
		shaderParameters_.SetValue(SP_WORLD_MATRIX, *renderContext->worldTransform_);
		shaderParameters_.SetValue(SP_VIEW_MATRIX, camera->GetViewMatrix());
		shaderParameters_.SetValue(SP_PROJVIEW_MATRIX, camera->GetProjectionMatrix() * camera->GetViewMatrix());
		shaderParameters_.SetValue(SP_CAMERA_POS, camera->GetNode()->GetWorldPosition());

		if (renderContext->geometryType_ == GEOMETRY_SKINNED)
		{
			skinMatrix_ = renderContext->worldTransform_;
			numSkinMatrix_ = renderContext->numWorldTransform_;
		}
	}

	inShaderParameters_ = renderContext->shaderParameters_;
}

void RenderEngine::SetVertexBuffers(const Vector<SharedPtr<VertexBuffer>>& vertexBuffers)
{
	vertexBuffers_.Clear();
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

void RenderEngine::SetTextures(const Vector<SharedPtr<Texture>>& textures)
{
	for (UInt32 i = 0; i < MAX_TEXTURE_CLASS; ++i)
	{
		textures_[i] = i < textures.Size() && textures[i] ? textures[i] : defaultTextures_[i];
	}

	texturesDirty_ = true;
}

void RenderEngine::SetDefaultTextures(TextureClass index, Texture* texture)
{
	defaultTextures_[index] = texture;
}

void RenderEngine::SetRasterizerState(RasterizerState rasterizerState)
{
	rasterizerState_ = rasterizerState;
	rasterizerStateDirty_ = true;
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

	SharedPtr<VertexFormat> vertexFormat(new VertexFormat(VSShader, vertexBuffer));
	vertexFormatCache_[hashValue] = vertexFormat;

	return vertexFormat;
}

static const D3D11_FILL_MODE d3d11FillMode[] =
{
	D3D11_FILL_WIREFRAME,
	D3D11_FILL_SOLID,
};

static const D3D11_CULL_MODE d3d11CullMode[] = 
{
	D3D11_CULL_NONE,
	D3D11_CULL_FRONT,
	D3D11_CULL_BACK,
};

void RenderEngine::PreDraw()
{
	static VertexBuffer* vertexBuffers[MAX_VERTEX_BUFFER_COUNT + 1] = { 0 };

	// 设置渲染模式
	if (rasterizerStateDirty_)
	{
		UInt32 stateHash = rasterizerState_.GetHash();
		if (!rasterizerStates_.Contains(stateHash))
		{
			D3D11_RASTERIZER_DESC stateDesc;
			memset(&stateDesc, 0, sizeof(stateDesc));
			stateDesc.FillMode = d3d11FillMode[rasterizerState_.fillMode_];
			stateDesc.CullMode = d3d11CullMode[rasterizerState_.cullMode_];
			stateDesc.FrontCounterClockwise = false;
			stateDesc.DepthBias = 0;
			stateDesc.DepthBiasClamp = 0.0f;

			stateDesc.SlopeScaledDepthBias = 0;
			stateDesc.DepthClipEnable = false;
			stateDesc.ScissorEnable = rasterizerState_.scissorTest_ ? true : false;
			stateDesc.MultisampleEnable = false;
			stateDesc.AntialiasedLineEnable = false;

			ID3D11RasterizerState* newRasterizerState = nullptr;

			HRESULT hr = device_->CreateRasterizerState(&stateDesc, &newRasterizerState);
			if (hr != 0)
			{
				FLAGGG_LOG_ERROR("CreateRasterizerState failed.");

				SAFE_RELEASE(newRasterizerState);

				return;
			}

			rasterizerStates_.Insert(MakePair(stateHash, newRasterizerState));
		}

		deviceContext_->RSSetState(rasterizerStates_[stateHash]);
		rasterizerStateDirty_ = false;
	}

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
		static UInt32 d3dVertexBufferCount{ 0 };
		static UInt32 d3dVertexSize[MAX_VERTEX_BUFFER_COUNT] = { 0 };
		static UInt32 d3dVertexOffset[MAX_VERTEX_BUFFER_COUNT] = { 0 };

		d3dVertexBufferCount = Min<UInt32>(vertexBuffers_.Size(), MAX_VERTEX_BUFFER_COUNT);

		for (UInt32 i = 0; i < d3dVertexBufferCount; ++i)
		{
			const SharedPtr<VertexBuffer>& vertexBuffer = vertexBuffers_[i];
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
			indexBuffer_->GetIndexSize() == sizeof(UInt16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
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
		for (UInt32 i = 0; i < MAX_CONST_BUFFER; ++i)
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

		for (UInt32 i = 0; i < MAX_TEXTURE_CLASS; ++i)
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

		if (!shaderResourceView[TEXTURE_CLASS_ENVIRONMENT] &&
			pixelShader_->GetTextureDesc().Contains(TEXTURE_CLASS_ENVIRONMENT))
		{
			// 直接渲染到backbuffer，需要创建一张环境贴图，然后将backbuffer拷贝到环境贴图
			Texture* renderTargetTexture = renderTarget_->GetParentTexture();

			if (!envTexture_)
			{
				envTexture_ = new Texture2D(nullptr);
				envTexture_->SetNumLevels(1);
				envTexture_->Initialize();
			}

			if (!renderTargetTexture)
			{
				//envTexture_->SetSize(400, 400, GetRGBFormat(), TEXTURE_DYNAMIC);
			}
			// 渲染到贴图，直接把当前贴图作为环境贴图采样
			// 修正：不能直接作为环境贴图传入，在shader里会变成NoResource，可能是应为renderTarget不能即读又写吧
			else
			{
				if (envTexture_->GetWidth() != renderTargetTexture->GetWidth() ||
					envTexture_->GetHeight() != renderTargetTexture->GetHeight())
					envTexture_->SetSize(renderTargetTexture->GetWidth(), renderTargetTexture->GetHeight(), GetRGBFormat(), TEXTURE_DYNAMIC);
				SharedArrayPtr<char> data(new char[envTexture_->GetWidth() * envTexture_->GetHeight() * envTexture_->GetComponents()]);
				static_cast<Texture2D*>(renderTargetTexture)->GetData(0, data);
				envTexture_->SetData(0, 0, 0, envTexture_->GetWidth(), envTexture_->GetHeight(), data);
			}

			if (envTexture_)
			{
				shaderResourceView[TEXTURE_CLASS_ENVIRONMENT] = envTexture_->shaderResourceView_;
				samplerState[TEXTURE_CLASS_ENVIRONMENT] = envTexture_->sampler_;
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
		Color color(0.0f, 0.0f, 0.0f, 1.0f);

		if (renderShadowMap_)
		{
			renderTargetView = defaultTextures_[TEXTURE_CLASS_SHADOWMAP]->GetRenderSurface()->GetObject<ID3D11RenderTargetView>();
			color = Color(1.0f, 1.0f, 1.0f, 1.0f);
		}

		if (rasterizerState_.depthWrite_)
		{
			deviceContext_->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
			deviceContext_->ClearRenderTargetView(renderTargetView, color.Data());
			deviceContext_->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);
		}
		else
		{
			deviceContext_->OMSetRenderTargets(1, &renderTargetView, nullptr);
		}
		renderTargetDirty_ = false;
	}
}

void RenderEngine::DrawCallIndexed(UInt32 indexStart, UInt32 indexCount)
{
	PreDraw();

	// draw call index
	deviceContext_->DrawIndexed(indexCount, indexStart, 0);
}

void RenderEngine::DrawCall(UInt32 vertexStart, UInt32 vertexCount)
{
	PreDraw();

	// draw call
	deviceContext_->Draw(vertexCount, vertexStart);
}

void RenderEngine::CopyShaderParameterToBuffer(Shader* shader, ConstantBuffer* buffer)
{
	if (!shader || !buffer)
		return;

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
				auto CopyParam = [&](ShaderParameters& shaderParam)
				{
					auto it2 = shaderParam.descs.Find(variableDesc.name_);
					if (it2 != shaderParam.descs.End())
					{
						shaderParam.dataBuffer_->Seek(it2->second_.offset_);
						shaderParam.dataBuffer_->ReadStream(data + variableDesc.offset_,
							Min(variableDesc.size_, it2->second_.size_));
					}
					else if (StringHash(variableDesc.name_) == SP_SKIN_MATRICES)
					{
						memcpy(data + variableDesc.offset_, skinMatrix_,
							Min(variableDesc.size_, numSkinMatrix_ * (UInt32)sizeof(Matrix3x4)));
					}
				};

				CopyParam(shaderParameters_);
				if (inShaderParameters_)
					CopyParam(*inShaderParameters_);
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

	Camera* camera = viewport->GetCamera();
	if (!camera)
		return;

	Scene* scene = viewport->GetScene();
	if (!scene)
		return;
	PODVector<RenderContext*> renderContexts;
	scene->Render(renderContexts);

	viewport->SetViewport();

	float aspect = (float)viewport->GetWidth() / viewport->GetHeight();

	PODVector<Light*> lights;
	scene->GetLights(lights);
			
	SetRenderTarget(viewport, true);

	// 放射相机不处理阴影
	if (!camera->GetUseReflection())
	{
		for (auto light : lights)
		{
			for (const auto& renderContext : renderContexts)
			{
				if ((renderContext->viewMask_ & camera->GetViewMask()) != renderContext->viewMask_)
					continue;
				if (!renderContext->renderPass_)
					continue;
				auto it = renderContext->renderPass_->Find(RENDER_PASS_TYPE_SHADOW);
				if (it != renderContext->renderPass_->End())
				{
					light->SetAspect(aspect);
					SetRasterizerState(shadowRasterizerState_);
					SetShaderParameter(light, renderContext);
					SetVertexShader(it->second_.vertexShader_);
					SetPixelShader(it->second_.pixelShader_);
					for (const auto& geometry : renderContext->geometries_)
					{
						SetVertexBuffers(geometry->GetVertexBuffers());
						SetIndexBuffer(geometry->GetIndexBuffer());
						SetPrimitiveType(geometry->GetPrimitiveType());
						DrawCallIndexed(geometry->GetIndexStart(), geometry->GetIndexCount());
					}
				}
			}
		}
	}
	else
	{
		// 这步操作用来清理阴影贴图
		PreDraw();
	}

	SetRenderTarget(viewport, false);
	for (const auto& renderContext : renderContexts)
	{
		if ((renderContext->viewMask_ & camera->GetViewMask()) != renderContext->viewMask_)
			continue;

		if (lights.Size() > 0
			// 要不要传灯光的参数，与是否加了阴影通道没关系
			/* &&
			renderContext->renderPass_ &&
			renderContext->renderPass_->Contains(RENDER_PASS_TYPE_SHADOW)*/)
		{
			Node* lightNode = lights[0]->GetNode();
			lights[0]->SetAspect(aspect);
			shaderParameters_.SetValue(SP_LIGHT_POS, lightNode->GetWorldPosition());
			shaderParameters_.SetValue(SP_LIGHT_DIR, lightNode->GetWorldRotation() * Vector3::FORWARD);
			shaderParameters_.SetValue(SP_LIGHT_VIEW_MATRIX, lights[0]->GetViewMatrix());
			shaderParameters_.SetValue(SP_LIGHT_PROJVIEW_MATRIX, lights[0]->GetProjectionMatrix() * lights[0]->GetViewMatrix());
		}

		camera->SetAspect(aspect);
		SetRasterizerState(renderContext->rasterizerState_);
		SetShaderParameter(camera, renderContext);
		SetVertexShader(renderContext->vertexShader_);
		SetPixelShader(renderContext->pixelShader_);
		SetTextures(renderContext->textures_);

		for (const auto& geometry : renderContext->geometries_)
		{
			SetVertexBuffers(geometry->GetVertexBuffers());
			SetIndexBuffer(geometry->GetIndexBuffer());
			SetPrimitiveType(geometry->GetPrimitiveType());
			DrawCallIndexed(geometry->GetIndexStart(), geometry->GetIndexCount());
		}
	}
}

void RenderEngine::RenderBatch(Viewport* viewport)
{
	static PODVector<VertexElement> vertexElement =
	{
		VertexElement(VE_VECTOR3, SEM_POSITION, 0),
		VertexElement(VE_VECTOR2, SEM_TEXCOORD, 0),
		VertexElement(VE_UBYTE4, SEM_COLOR, 0)
	};
	static SharedPtr<VertexBuffer> vertexBuffer;
	static Vector<SharedPtr<VertexBuffer>> vertexBuffers;
	static SharedPtr<Shader> vertexShader;
	static SharedPtr<Shader> pixelShader;
	static SharedPtr<ShaderCode> vertexShaderCode;
	static SharedPtr<ShaderCode> pixelShaderCode;
	static Vector<SharedPtr<Texture>> textures;

	if (!viewport)
		return;

	viewport->SetViewport();

	SetRenderTarget(viewport, false);

	if (!vertexBuffer)
	{
		vertexBuffer = new VertexBuffer();
	}

	if (!vertexShaderCode && !pixelShaderCode)
	{
		auto* cache = context_->GetVariable<ResourceCache>("ResourceCache");

		vertexShaderCode = cache->GetResource<ShaderCode>("Shader/UI.hlsl");
		vertexShader = vertexShaderCode->GetShader(VS, {});

		pixelShaderCode = cache->GetResource<ShaderCode>("Shader/UI.hlsl");
		pixelShader = pixelShaderCode->GetShader(PS, {});
	}

	for (const auto& batch : batches_)
	{
		vertexBuffer->SetSize(batch->GetVertexCount(), vertexElement);
		char* data = (char*)vertexBuffer->Lock(0, vertexBuffer->GetVertexCount());
		memcpy(data, batch->GetVertexs(), batch->GetVertexCount() * batch->GetVertexSize());
		vertexBuffer->Unlock();

		vertexBuffers.Clear();
		vertexBuffers.Push(vertexBuffer);

		SetVertexBuffers(vertexBuffers);
		SetVertexShader(vertexShader);
		SetPixelShader(pixelShader);

		textures.Clear();
		if (batch->GetTexture())
		{
			textures.Push(SharedPtr<Texture>(batch->GetTexture()));
			SetTextures(textures);
		}

		switch (batch->GetType())
		{
		case DRAW_LINE:
			SetPrimitiveType(PRIMITIVE_LINE);
			break;

		case DRAW_TRIANGLE:
			SetPrimitiveType(PRIMITIVE_TRIANGLE);
			break;
		}

		DrawCall(0, batch->GetVertexCount());
	}
}

}