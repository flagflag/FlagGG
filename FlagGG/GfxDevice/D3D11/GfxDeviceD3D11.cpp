#include "GfxDeviceD3D11.h"
#include "GfxD3D11Defines.h"
#include "GfxTextureD3D11.h"
#include "GfxBufferD3D11.h"
#include "GfxShaderD3D11.h"
#include "GfxDevice/GfxProgram.h"
#include "GfxDevice/VertexDescFactory.h"

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
	for (auto it = rasterizerStates_.Begin(); it != rasterizerStates_.End(); ++it)
	{
		D3D11_SAFE_RELEASE(it->second_);
	}
}

void GfxDeviceD3D11::Draw(UInt32 vertexStart, UInt32 vertexCount)
{
	PrepareDraw();

	deviceContext_->Draw(vertexCount, vertexStart);
}

void GfxDeviceD3D11::DrawIndexed(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart)
{
	PrepareDraw();

	deviceContext_->DrawIndexed(indexCount, indexStart, vertexStart);
}

bool GfxDeviceD3D11::PrepareDraw()
{
	static GfxBufferD3D11* vertexBuffers[MAX_VERTEX_BUFFER_COUNT + 1] = { 0 };

	PrepareRasterizerState();

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

	if (texturesDirty_)
	{
		static ID3D11ShaderResourceView* vertexShaderResourceView[MAX_TEXTURE_CLASS] = { nullptr };
		static ID3D11SamplerState* vertexSamplerState[MAX_TEXTURE_CLASS] = { nullptr };
		static ID3D11ShaderResourceView* pixelShaderResourceView[MAX_TEXTURE_CLASS] = { nullptr };
		static ID3D11SamplerState* pixelSamplerState[MAX_TEXTURE_CLASS] = { nullptr };

		for (UInt32 i = 0; i < MAX_TEXTURE_CLASS; ++i)
		{
			auto currentTexture = DynamicCast<GfxTextureD3D11>(textures_[i]);
			if (currentTexture)
			{
				if (vertexShaderD3D11->GetTextureDesc().Contains(i))
				{
					vertexShaderResourceView[i] = currentTexture->GetD3D11ShaderResourceView();
					vertexSamplerState[i] = currentTexture->sampler_;
				}

				if (pixelShaderD3D11->GetTextureDesc().Contains(i))
				{
					pixelShaderResourceView[i] = currentTexture->GetD3D11ShaderResourceView();
					pixelSamplerState[i] = currentTexture->sampler_;
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
			constantBuffer.SetSize(bufferDesc.size_);
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
					else if (StringHash(variableDesc.name_) == SP_SKIN_MATRICES)
					{
						memcpy(data + variableDesc.offset_, skinMatrix_,
							Min(variableDesc.size_, numSkinMatrix_ * (UInt32)sizeof(Matrix3x4)));
					}
				};

				if (engineShaderParameters_)
					CopyParam(*engineShaderParameters_);

				if (materialShaderParameters_)
					CopyParam(*materialShaderParameters_);
			}
			constantBuffer.EndWrite(0);
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
		desc.InputSlot = 0; // �Ȳ�֧�ֶ�VertexBuffer�����в�ͬ�Ĳ��֣����ֳ��Ͽ��ܻ��õ���֮���ڸİɣ�
		desc.AlignedByteOffset = element.offset_;
		desc.InputSlotClass = element.perInstance_ ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = element.perInstance_ ? 1 : 0;
		d3d11InputElementDescs.Push(desc);
	}

	ID3DBlob* shaderBlob = vertexShader->GetByteCode();
	if (!shaderBlob)
	{
		FLAGGG_LOG_ERROR("Failed to create vertext layout, shader code is nullptr.");
		return;
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
