#include "GfxDeviceD3D11.h"
#include "GfxD3D11Defines.h"
#include "GfxSwapChainD3D11.h"
#include "GfxTextureD3D11.h"
#include "GfxShaderD3D11.h"
#include "GfxRenderSurfaceD3D11.h"
#include "GfxDevice/GfxProgram.h"
#include "GfxDevice/GfxSampler.h"
#include "GfxDevice/VertexDescFactory.h"
#include "Graphics/ShaderParameter.h"

#include "Log.h"

namespace FlagGG
{

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

static const DXGI_FORMAT d3dElementFormats[] =
{
	DXGI_FORMAT_R32_SINT,
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R8G8B8A8_UINT,
	DXGI_FORMAT_R8G8B8A8_UNORM
};

static const D3D11_FILTER d3d11Filter[] =
{
	D3D11_FILTER_MIN_MAG_MIP_POINT,
	D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
	D3D11_FILTER_MIN_MAG_MIP_LINEAR,
	D3D11_FILTER_ANISOTROPIC,
	D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
	D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT
};

static const D3D11_TEXTURE_ADDRESS_MODE d3dAddressMode[] =
{
	D3D11_TEXTURE_ADDRESS_WRAP,
	D3D11_TEXTURE_ADDRESS_MIRROR,
	D3D11_TEXTURE_ADDRESS_CLAMP,
	D3D11_TEXTURE_ADDRESS_BORDER
};

static const D3D11_COMPARISON_FUNC d3dComparisonFun[] =
{
	D3D11_COMPARISON_NEVER,
	D3D11_COMPARISON_LESS,
	D3D11_COMPARISON_EQUAL,
	D3D11_COMPARISON_LESS_EQUAL,
	D3D11_COMPARISON_GREATER,
	D3D11_COMPARISON_NOT_EQUAL,
	D3D11_COMPARISON_GREATER_EQUAL,
	D3D11_COMPARISON_ALWAYS
};

GfxDevice* GfxDevice::CreateDevice()
{
	return GfxDeviceD3D11::CreateInstance();
}

void GfxDevice::DestroyDevice()
{
	GfxDeviceD3D11::DestroyInstance();
}

GfxDevice* GfxDevice::GetDevice()
{
	return GfxDeviceD3D11::Instance();
}

GfxDeviceD3D11::GfxDeviceD3D11()
	: GfxDevice()
{
	UINT createDeviceFlags = 0;

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

		D3D11_SAFE_RELEASE(device_);
		D3D11_SAFE_RELEASE(deviceContext_);

		return;
	}
}

GfxDeviceD3D11::~GfxDeviceD3D11()
{
	D3D11_SAFE_RELEASE(device_);
	D3D11_SAFE_RELEASE(deviceContext_);
	for (auto& it : rasterizerStates_)
	{
		D3D11_SAFE_RELEASE(it.second_);
	}
	for (auto& it : d3d11InputLayoutMap_)
	{
		D3D11_SAFE_RELEASE(it.second_);
	}
	for (auto& it : d3d11SamplerStateMap_)
	{
		D3D11_SAFE_RELEASE(it.second_);
	}
}

void GfxDeviceD3D11::Clear(ClearTargetFlags flags, const Color& color/* = Color::TRANSPARENT_BLACK*/, float depth/* = 1.0f*/, unsigned stencil/* = 0*/)
{
	if (flags & CLEAR_COLOR)
	{
		for (UInt32 slotID = 0; slotID < MAX_RENDERTARGET_COUNT; ++slotID)
		{
			auto renderTargetD3D11 = DynamicCast<GfxRenderSurfaceD3D11>(renderTargets_[slotID]);
			if (renderTargetD3D11)
			{
				auto* renderTargetView = renderTargetD3D11->GetRenderTargetView();
				if (renderTargetView)
				{
					deviceContext_->ClearRenderTargetView(renderTargetView, color.Data());
				}
			}
		}
	}

	UINT clearDepthStencilFlags = 0;
	if (flags & CLEAR_DEPTH)
		clearDepthStencilFlags |= D3D11_CLEAR_DEPTH;
	if (flags & CLEAR_STENCIL)
		clearDepthStencilFlags |= D3D11_CLEAR_STENCIL;

	if (clearDepthStencilFlags)
	{
		auto depthStencilD3D11 = DynamicCast<GfxRenderSurfaceD3D11>(depthStencil_);
		if (depthStencilD3D11)
		{
			auto* depthStencilView = depthStencilD3D11->GetDepthStencilView();
			if (depthStencilView)
			{
				deviceContext_->ClearDepthStencilView(depthStencilView, clearDepthStencilFlags, depth, stencil);
			}
		}
	}
}

void GfxDeviceD3D11::Draw(UInt32 vertexStart, UInt32 vertexCount)
{
	if (!vertexShader_ || !pixelShader_)
	{
		CRY_ASSERT(false, "vertex shader or pixel shader is null.");
		return;
	}

	PrepareDraw();

	deviceContext_->Draw(vertexCount, vertexStart);
}

void GfxDeviceD3D11::DrawIndexed(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart)
{
	if (!vertexShader_ || !pixelShader_)
	{
		CRY_ASSERT(false, "vertex shader or pixel shader is null.");
		return;
	}

	PrepareDraw();

	deviceContext_->DrawIndexed(indexCount, indexStart, vertexStart);
}

void GfxDeviceD3D11::Flush()
{
	deviceContext_->Flush();
}

void GfxDeviceD3D11::PrepareDraw()
{
	static GfxBufferD3D11* vertexBuffers[MAX_VERTEX_BUFFER_COUNT + 1] = { 0 };

	PrepareRasterizerState();

	PrepareDepthStencilState();

	auto vertexShaderD3D11 = DynamicCast<GfxShaderD3D11>(vertexShader_);
	auto pixelShaderD3D11 = DynamicCast<GfxShaderD3D11>(pixelShader_);

	switch (primitiveType_)
	{
	case PRIMITIVE_LINE:
		deviceContext_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		break;

	case PRIMITIVE_TRIANGLE:
		deviceContext_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	}

	if (vertexBufferDirty_)
	{
		static ID3D11Buffer* d3dVertexBuffers[MAX_VERTEX_BUFFER_COUNT] = { nullptr };
		static UInt32 d3dVertexBufferCount{ 0 };
		static UInt32 d3dVertexSize[MAX_VERTEX_BUFFER_COUNT] = { 0 };
		static UInt32 d3dVertexOffset[MAX_VERTEX_BUFFER_COUNT] = { 0 };

		d3dVertexBufferCount = Min<UInt32>(vertexBuffers_.Size(), MAX_VERTEX_BUFFER_COUNT);

		for (UInt32 i = 0; i < d3dVertexBufferCount; ++i)
		{
			const SharedPtr<GfxBuffer>& vertexBuffer = vertexBuffers_[i];
			vertexBuffers[i] = DynamicCast<GfxBufferD3D11>(vertexBuffer);
			d3dVertexBuffers[i] = vertexBuffers[i]->GetD3D11Buffer();
			d3dVertexSize[i] = vertexBuffer->GetDesc().stride_;
			d3dVertexOffset[i] = 0;
		}

		deviceContext_->IASetVertexBuffers(0, d3dVertexBufferCount, d3dVertexBuffers, d3dVertexSize, d3dVertexOffset);

		vertexBufferDirty_ = false;
	}

	if (indexBufferDirty_)
	{
		deviceContext_->IASetIndexBuffer(DynamicCast<GfxBufferD3D11>(indexBuffer_)->GetD3D11Buffer(),
			indexBuffer_->GetDesc().stride_ == sizeof(UInt16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);

		indexBufferDirty_ = false;
	}

	if (shaderDirty_)
	{
		deviceContext_->VSSetShader(vertexShaderD3D11->GetD3D11VertexShader(), nullptr, 0);
		deviceContext_->PSSetShader(pixelShaderD3D11->GetD3D11PixelShader(), nullptr, 0);

		shaderDirty_ = false;
	}

	if (vertexDescDirty_)
	{
		ID3D11InputLayout* vertexFormat = GetD3D11InputLayout(vertexDesc_, vertexShaderD3D11);
		deviceContext_->IASetInputLayout(vertexFormat);

		vertexDescDirty_ = false;
	}

	{
		CopyShaderParameterToBuffer(vertexShaderD3D11, vsConstantBuffer_);
		CopyShaderParameterToBuffer(pixelShaderD3D11, psConstantBuffer_);

		static ID3D11Buffer* d3dVSConstantBuffer[MAX_CONST_BUFFER_COUNT] = { nullptr };
		static ID3D11Buffer* d3dPSConstantBuffer[MAX_CONST_BUFFER_COUNT] = { nullptr };
		for (UInt32 i = 0; i < MAX_CONST_BUFFER; ++i)
		{
			d3dVSConstantBuffer[i] = vsConstantBuffer_[i].GetD3D11Buffer();
			d3dPSConstantBuffer[i] = psConstantBuffer_[i].GetD3D11Buffer();
		}

		deviceContext_->VSSetConstantBuffers(0, MAX_CONST_BUFFER, d3dVSConstantBuffer);
		deviceContext_->PSSetConstantBuffers(0, MAX_CONST_BUFFER, d3dPSConstantBuffer);
	}
	
	// 放在设置纹理之前设置RT，原因：如果某个RT下一次Draw时作为纹理调用PSSetShaderResources时会失效（因为没有解绑的RT不允许设置为ShaderResource）
	if (renderTargetDirty_ || depthStencilDirty_)
	{
		static ID3D11RenderTargetView* d3dRenderTargetViews[MAX_RENDERTARGET_COUNT] = {};

		UInt32 renderTargetCount = 0u;
		for (UInt32 slotID = 0; slotID < MAX_RENDERTARGET_COUNT; ++slotID)
		{
			auto renderTargetD3D11 = DynamicCast<GfxRenderSurfaceD3D11>(renderTargets_[slotID]);
			d3dRenderTargetViews[slotID] = renderTargetD3D11 ? renderTargetD3D11->GetRenderTargetView() : nullptr;
			if (d3dRenderTargetViews[slotID])
				renderTargetCount = slotID + 1;
		}

		auto depthStencilD3D11 = DynamicCast<GfxRenderSurfaceD3D11>(depthStencil_);
		auto* d3dDepthStencilView = depthStencilD3D11 ? depthStencilD3D11->GetDepthStencilView() : nullptr;

		if (depthStencilState_.depthWrite_)
		{
			deviceContext_->OMSetRenderTargets(renderTargetCount, d3dRenderTargetViews, d3dDepthStencilView);
		}
		else
		{
			deviceContext_->OMSetRenderTargets(renderTargetCount, d3dRenderTargetViews, nullptr);
		}

		renderTargetDirty_ = false;
		depthStencilDirty_ = false;
	}

	if (texturesDirty_ || samplerDirty_)
	{
		static ID3D11ShaderResourceView* vertexShaderResourceView[MAX_TEXTURE_CLASS] = {};
		static ID3D11SamplerState* vertexSamplerState[MAX_TEXTURE_CLASS] = {};
		static ID3D11ShaderResourceView* pixelShaderResourceView[MAX_TEXTURE_CLASS] = {};
		static ID3D11SamplerState* pixelSamplerState[MAX_TEXTURE_CLASS] = {};

		for (UInt32 i = 0; i < MAX_TEXTURE_CLASS; ++i)
		{
			auto currentTexture = DynamicCast<GfxTextureD3D11>(textures_[i]);
			if (currentTexture)
			{
				if (vertexShaderD3D11->GetTextureDesc().Contains(i))
				{
					vertexShaderResourceView[i] = currentTexture->GetD3D11ShaderResourceView();
					vertexSamplerState[i] = GetD3D11SamplerState(samplers_[i]);
				}
				else
				{
					vertexShaderResourceView[i] = nullptr;
					vertexSamplerState[i] = nullptr;
				}

				if (pixelShaderD3D11->GetTextureDesc().Contains(i))
				{
					pixelShaderResourceView[i] = currentTexture->GetD3D11ShaderResourceView();
					pixelSamplerState[i] = GetD3D11SamplerState(samplers_[i]);
				}
				else
				{
					pixelShaderResourceView[i] = nullptr;
					pixelSamplerState[i] = nullptr;
				}
			}
			else
			{
				vertexShaderResourceView[i] = nullptr;
				vertexSamplerState[i] = nullptr;
				pixelShaderResourceView[i] = nullptr;
				pixelSamplerState[i] = nullptr;
			}
		}

		deviceContext_->VSSetShaderResources(0, MAX_TEXTURE_CLASS, vertexShaderResourceView);
		deviceContext_->VSSetSamplers(0, MAX_TEXTURE_CLASS, vertexSamplerState);

		deviceContext_->PSSetShaderResources(0, MAX_TEXTURE_CLASS, pixelShaderResourceView);
		deviceContext_->PSSetSamplers(0, MAX_TEXTURE_CLASS, pixelSamplerState);

		texturesDirty_ = false;
		samplerDirty_ = false;
	}

	if (viewportDirty_)
	{
		D3D11_VIEWPORT d3d11Viewport;
		d3d11Viewport.TopLeftX = viewport_.Left();
		d3d11Viewport.TopLeftY = viewport_.Top();
		d3d11Viewport.Width = viewport_.Width();
		d3d11Viewport.Height = viewport_.Height();
		d3d11Viewport.MinDepth = 0.0f;
		d3d11Viewport.MaxDepth = 1.0f;

		deviceContext_->RSSetViewports(1, &d3d11Viewport);

		viewportDirty_ = false;
	}
}

void GfxDeviceD3D11::PrepareRasterizerState()
{
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
				D3D11_SAFE_RELEASE(newRasterizerState);
				return;
			}

			rasterizerStates_.Insert(MakePair(stateHash, newRasterizerState));
		}

		deviceContext_->RSSetState(rasterizerStates_[stateHash]);
		rasterizerStateDirty_ = false;
	}
}

void GfxDeviceD3D11::PrepareDepthStencilState()
{
	if (depthStencilStateDirty_)
	{
		UInt32 stateHash = depthStencilState_.GetHash();
		if (!depthStencilStates_.Contains(stateHash))
		{
			D3D11_DEPTH_STENCIL_DESC d3d11DepthStencilDesc;
			memset(&d3d11DepthStencilDesc, 0, sizeof(d3d11DepthStencilDesc));

			d3d11DepthStencilDesc.DepthEnable = true;
			d3d11DepthStencilDesc.DepthFunc = d3dComparisonFun[depthStencilState_.depthTestMode_];
			d3d11DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

			d3d11DepthStencilDesc.StencilEnable = depthStencilState_.stencilTest_;
			d3d11DepthStencilDesc.StencilReadMask = depthStencilState_.stencilReadMask_;
			d3d11DepthStencilDesc.StencilWriteMask = depthStencilState_.stencilWriteMask_;

			ID3D11DepthStencilState* d3d11DepthStencilState = nullptr;
			HRESULT hr = device_->CreateDepthStencilState(&d3d11DepthStencilDesc, &d3d11DepthStencilState);
			if (FAILED(hr))
			{
				FLAGGG_LOG_ERROR("CreateRasterizerState failed.");
				D3D11_SAFE_RELEASE(d3d11DepthStencilState);
				return;
			}

			depthStencilStates_.Insert(MakePair(stateHash, d3d11DepthStencilState));
		}

		deviceContext_->OMSetDepthStencilState(depthStencilStates_[stateHash], depthStencilState_.stencilRef_);
		depthStencilStateDirty_ = false;
	}
}

void GfxDeviceD3D11::CopyShaderParameterToBuffer(GfxShaderD3D11* shader, GfxBufferD3D11* buffer)
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
			if (constantBuffer.GetDesc().size_ < bufferDesc.size_)
			{
				constantBuffer.SetStride(sizeof(float));
				constantBuffer.SetSize(bufferDesc.size_);
				constantBuffer.SetBind(BUFFER_BIND_UNIFORM);
				constantBuffer.SetAccess(BUFFER_ACCESS_WRITE);
				constantBuffer.SetUsage(BUFFER_USAGE_DYNAMIC);
				constantBuffer.Apply(nullptr);
			}
			char* data = static_cast<char*>(constantBuffer.BeginWrite(0, bufferDesc.size_));
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
				};

				if (engineShaderParameters_)
					CopyParam(*engineShaderParameters_);

				if (materialShaderParameters_)
					CopyParam(*materialShaderParameters_);
			}
			constantBuffer.EndWrite(bufferDesc.size_);
		}
	}
}

ID3D11InputLayout* GfxDeviceD3D11::GetD3D11InputLayout(VertexDescription* verteDesc, GfxShaderD3D11* vertexShader)
{
	auto key = MakePair(verteDesc->GetUUID(), vertexShader);

	auto it = d3d11InputLayoutMap_.Find(key);
	if (it != d3d11InputLayoutMap_.End())
	{
		return it->second_;
	}
	
	PODVector<D3D11_INPUT_ELEMENT_DESC> d3d11InputElementDescs;

	const PODVector<VertexElement>& elements = verteDesc->GetElements();

	for (const auto& element : elements)
	{
		const char* semName = VERTEX_ELEMENT_SEM_NAME[element.vertexElementSemantic_];

		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName = semName;
		desc.SemanticIndex = element.index_;
		desc.Format = d3dElementFormats[element.vertexElementType_];
		desc.InputSlot = 0; // 先不支持多VertexBuffer可以有不同的布局（部分场合可能会用到，之后在改吧）
		desc.AlignedByteOffset = element.offset_;
		desc.InputSlotClass = element.perInstance_ ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = element.perInstance_ ? 1 : 0;
		d3d11InputElementDescs.Push(desc);
	}

	ID3DBlob* shaderBlob = vertexShader->GetByteCode();
	if (!shaderBlob)
	{
		FLAGGG_LOG_ERROR("Failed to create vertext layout, shader code is nullptr.");
		return nullptr;
	}

	ID3D11InputLayout* inputLayout = nullptr;
	HRESULT hr = device_->CreateInputLayout(&d3d11InputElementDescs[0], d3d11InputElementDescs.Size(),
		shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &inputLayout);
	if (FAILED(hr))
	{
		D3D11_SAFE_RELEASE(inputLayout);
		FLAGGG_LOG_ERROR("Failed to create vertex layout.");
		return nullptr;
	}

	d3d11InputLayoutMap_[key] = inputLayout;

	return inputLayout;
}

ID3D11SamplerState* GfxDeviceD3D11::GetD3D11SamplerState(GfxSampler* gfxSampler)
{
	if (!gfxSampler)
		return nullptr;

	auto it = d3d11SamplerStateMap_.Find(gfxSampler->GetHash());
	if (it != d3d11SamplerStateMap_.End())
		return it->second_;

	const SamplerDesc& desc = gfxSampler->GetDesc();

	D3D11_SAMPLER_DESC d3d11SamplerDesc;
	memset(&d3d11SamplerDesc, 0, sizeof(d3d11SamplerDesc));
	d3d11SamplerDesc.Filter = d3d11Filter[desc.filterMode_];
	d3d11SamplerDesc.AddressU = d3dAddressMode[desc.addresMode_[TEXTURE_COORDINATE_U]];
	d3d11SamplerDesc.AddressV = d3dAddressMode[desc.addresMode_[TEXTURE_COORDINATE_V]];
	d3d11SamplerDesc.AddressW = d3dAddressMode[desc.addresMode_[TEXTURE_COORDINATE_W]];
	d3d11SamplerDesc.MaxAnisotropy = 4;
	d3d11SamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	d3d11SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	d3d11SamplerDesc.ComparisonFunc = d3dComparisonFun[desc.comparisonFunc_];

	ID3D11SamplerState* d3d11SamplerState = nullptr;
	HRESULT hr = device_->CreateSamplerState(&d3d11SamplerDesc, &d3d11SamplerState);
	if (hr != 0)
	{
		FLAGGG_LOG_ERROR("CreateSamplerState failed.");
		D3D11_SAFE_RELEASE(d3d11SamplerState);
		return nullptr;
	}

	it = d3d11SamplerStateMap_.Insert(MakePair(gfxSampler->GetHash(), d3d11SamplerState));

	return it->second_;
}

GfxSwapChain* GfxDeviceD3D11::CreateSwapChain(Window* window)
{
	return new GfxSwapChainD3D11(window);
}

GfxTexture* GfxDeviceD3D11::CreateTexture()
{
	return new GfxTextureD3D11();
}

GfxBuffer* GfxDeviceD3D11::CreateBuffer()
{
	return new GfxBufferD3D11();
}

GfxShader* GfxDeviceD3D11::CreateShader()
{
	return new GfxShaderD3D11();
}

GfxProgram* GfxDeviceD3D11::CreateProgram()
{
	return new GfxProgram();
}

bool GfxDeviceD3D11::CheckMultiSampleSupport(DXGI_FORMAT format, UInt32 sampleCount)
{
	if (sampleCount < 2)
	{
		return true;
	}

	UINT numLevels = 0;
	if (FAILED(GetD3D11Device()->CheckMultisampleQualityLevels(format, sampleCount, &numLevels)))
	{
		return false;
	}

	return numLevels > 0;
}

UInt32 GfxDeviceD3D11::GetMultiSampleQuality(DXGI_FORMAT format, UInt32 sampleCount)
{
	if (sampleCount < 2)
	{
		return 0;
	}

	if (GetD3D11Device()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_1)
	{
		return 0xffffffff;
	}

	UINT numLevels = 0;
	if (FAILED(GetD3D11Device()->CheckMultisampleQualityLevels(format, sampleCount, &numLevels)) || !numLevels)
	{
		return 0;
	}

	return numLevels - 1;
}

}

