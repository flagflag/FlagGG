#include "GfxDeviceD3D11.h"
#include "GfxD3D11Defines.h"
#include "GfxSwapChainD3D11.h"
#include "GfxTextureD3D11.h"
#include "GfxShaderResourceViewD3D11.h"
#include "GfxShaderD3D11.h"
#include "GfxRenderSurfaceD3D11.h"
#include "AmbientOcclusionRenderingD3D11.h"
#include "GfxDevice/GfxProgram.h"
#include "GfxDevice/GfxSampler.h"
#include "GfxDevice/VertexDescFactory.h"
#include "Graphics/ShaderParameter.h"
#include "Memory/Memory.h"
#include "Core/Profiler.h"

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

static const D3D11_BLEND d3d11SourceRgbBlendFactor[] =
{
	D3D11_BLEND_ONE,
	D3D11_BLEND_ONE,
	D3D11_BLEND_DEST_COLOR,
	D3D11_BLEND_SRC_ALPHA,
	D3D11_BLEND_SRC_ALPHA,
	D3D11_BLEND_ONE,
	D3D11_BLEND_INV_DEST_ALPHA,
	D3D11_BLEND_ONE,
	D3D11_BLEND_SRC_ALPHA,
};

static const D3D11_BLEND d3d11DestRgbBlendFactor[] =
{
	D3D11_BLEND_ZERO,
	D3D11_BLEND_ONE,
	D3D11_BLEND_ZERO,
	D3D11_BLEND_INV_SRC_ALPHA,
	D3D11_BLEND_ONE,
	D3D11_BLEND_INV_SRC_ALPHA,
	D3D11_BLEND_DEST_ALPHA,
	D3D11_BLEND_ONE,
	D3D11_BLEND_ONE,
};

static const D3D11_BLEND_OP d3d11RgbBlendOpt[] =
{
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_OP_REV_SUBTRACT,
	D3D11_BLEND_OP_REV_SUBTRACT,
};

static const D3D11_BLEND d3d11SourceAlphaBlendFactor[] =
{
	D3D11_BLEND_ONE,
	D3D11_BLEND_ONE,
	D3D11_BLEND_DEST_COLOR,
	D3D11_BLEND_SRC_ALPHA,
	D3D11_BLEND_SRC_ALPHA,
	D3D11_BLEND_ONE,
	D3D11_BLEND_INV_DEST_ALPHA,
	D3D11_BLEND_ONE,
	D3D11_BLEND_SRC_ALPHA,
};

static const D3D11_BLEND d3d11DestAlphaBlendFactor[] =
{
	D3D11_BLEND_ZERO,
	D3D11_BLEND_ONE,
	D3D11_BLEND_ZERO,
	D3D11_BLEND_INV_SRC_ALPHA,
	D3D11_BLEND_ONE,
	D3D11_BLEND_INV_SRC_ALPHA,
	D3D11_BLEND_DEST_ALPHA,
	D3D11_BLEND_ONE,
	D3D11_BLEND_ONE,
};

static const D3D11_BLEND_OP d3d11AlphaBlendOpt[] =
{
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_OP_REV_SUBTRACT,
	D3D11_BLEND_OP_REV_SUBTRACT,
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

static const D3D11_STENCIL_OP d3d11StencilOperation[] =
{
	D3D11_STENCIL_OP_KEEP,
	D3D11_STENCIL_OP_ZERO,
	D3D11_STENCIL_OP_REPLACE,
	D3D11_STENCIL_OP_INCR_SAT,
	D3D11_STENCIL_OP_DECR_SAT,
	D3D11_STENCIL_OP_INVERT,
	D3D11_STENCIL_OP_INCR,
	D3D11_STENCIL_OP_DECR,
};

GfxDeviceD3D11::GfxDeviceD3D11()
	: GfxDevice()
{
	UINT createDeviceFlags = 0;
#if _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
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
	for (auto& it : blendStates_)
	{
		D3D11_SAFE_RELEASE(it);
	}
}

void GfxDeviceD3D11::BeginFrame()
{
	GfxDevice::BeginFrame();

	deviceContext_->ClearState();
}

void GfxDeviceD3D11::EndFrame()
{
	GfxDevice::EndFrame();
}

void GfxDeviceD3D11::Clear(ClearTargetFlags flags, const Color& color/* = Color::TRANSPARENT_BLACK*/, float depth/* = 1.0f*/, unsigned stencil/* = 0*/)
{
	if (flags & CLEAR_COLOR)
	{
		for (UInt32 slotID = 0; slotID < MAX_RENDERTARGET_COUNT; ++slotID)
		{
			auto renderTargetD3D11 = RTTICast<GfxRenderSurfaceD3D11>(renderTargets_[slotID]);
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
		auto depthStencilD3D11 = RTTICast<GfxRenderSurfaceD3D11>(depthStencil_);
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
		ASSERT_MESSAGE(false, "vertex shader or pixel shader is null.");
		return;
	}

	PROFILE_AUTO(GfxDeviceD3D11::Draw);

	PrepareDraw();

	deviceContext_->Draw(vertexCount, vertexStart);
}

void GfxDeviceD3D11::DrawIndexed(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart)
{
	if (!vertexShader_ || !pixelShader_)
	{
		ASSERT_MESSAGE(false, "vertex shader or pixel shader is null.");
		return;
	}

	PROFILE_AUTO(GfxDeviceD3D11::DrawIndexed);

	PrepareDraw();

	deviceContext_->DrawIndexed(indexCount, indexStart, vertexStart);
}

void GfxDeviceD3D11::DrawIndexedInstanced(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart, UInt32 instanceStart, UInt32 instanceCount)
{
	if (!vertexShader_ || !pixelShader_)
	{
		ASSERT_MESSAGE(false, "vertex shader or pixel shader is null.");
		return;
	}

	PROFILE_AUTO(GfxDeviceD3D11::DrawIndexedInstanced);

	PrepareDraw();

	deviceContext_->DrawIndexedInstanced(indexCount, instanceCount, indexStart, vertexStart, instanceStart);
}

void GfxDeviceD3D11::Flush()
{
	deviceContext_->Flush();
}

void GfxDeviceD3D11::Dispatch(UInt32 threadGroupCountX, UInt32 threadGroupCountY, UInt32 threadGroupCountZ)
{
	PrepareDispatch();

	deviceContext_->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

bool GfxDeviceD3D11::IsInstanceSupported() const
{
	return true;
}

void GfxDeviceD3D11::PrepareDraw()
{
	PROFILE_AUTO(GfxDeviceD3D11::PrepareDraw);

	PrepareRasterizerState();

	PrepareDepthStencilState();

	auto vertexShaderD3D11 = RTTICast<GfxShaderD3D11>(vertexShader_);
	auto pixelShaderD3D11 = RTTICast<GfxShaderD3D11>(pixelShader_);

	if (computeBufferDirty_)
	{
		ID3D11UnorderedAccessView* d3dUAV[D3D11_PS_CS_UAV_REGISTER_COUNT] = {};
		deviceContext_->CSSetUnorderedAccessViews(0, D3D11_PS_CS_UAV_REGISTER_COUNT, d3dUAV, nullptr);
		computeBufferDirty_ = false;
	}

	switch (primitiveType_)
	{
	case PRIMITIVE_LINE:
		deviceContext_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		break;

	case PRIMITIVE_TRIANGLE:
		deviceContext_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	}

	if (vertexBufferDirty_ || instanceBufferDirty_)
	{
		static ID3D11Buffer* d3dVertexBuffers[MAX_VERTEX_BUFFER_COUNT] = { nullptr };
		static UInt32 d3dVertexBufferCount{ 0 };
		static UInt32 d3dVertexSize[MAX_VERTEX_BUFFER_COUNT] = { 0 };
		static UInt32 d3dVertexOffset[MAX_VERTEX_BUFFER_COUNT] = { 0 };

		d3dVertexBufferCount = Min<UInt32>(vertexBuffers_.Size(), MAX_VERTEX_BUFFER_COUNT);

		for (UInt32 i = 0; i < d3dVertexBufferCount; ++i)
		{
			auto* vertexBufferD3D11 = RTTICast<GfxBufferD3D11>(vertexBuffers_[i]);
			d3dVertexBuffers[i] = vertexBufferD3D11->GetD3D11Buffer();
			d3dVertexSize[i] = vertexDesc_->GetStrideSize();
			d3dVertexOffset[i] = 0;
		}

		if (instanceBuffer_)
		{
			auto* instanceBufferD3D11 = RTTICast<GfxBufferD3D11>(instanceBuffer_);
			d3dVertexBuffers[d3dVertexBufferCount] = instanceBufferD3D11->GetD3D11Buffer();
			d3dVertexSize[d3dVertexBufferCount] = instanceBufferD3D11->GetDesc().stride_;
			d3dVertexOffset[d3dVertexBufferCount] = 0;
			++d3dVertexBufferCount;
		}

		deviceContext_->IASetVertexBuffers(0, d3dVertexBufferCount, d3dVertexBuffers, d3dVertexSize, d3dVertexOffset);

		vertexBufferDirty_ = false;
	}

	if (indexBufferDirty_)
	{
		deviceContext_->IASetIndexBuffer(RTTICast<GfxBufferD3D11>(indexBuffer_)->GetD3D11Buffer(),
			indexBuffer_->GetDesc().stride_ == sizeof(UInt16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);

		indexBufferDirty_ = false;
	}

	if (shaderDirty_)
	{
		deviceContext_->VSSetShader(vertexShaderD3D11->GetD3D11VertexShader(), nullptr, 0);
		deviceContext_->PSSetShader(pixelShaderD3D11->GetD3D11PixelShader(), nullptr, 0);

		shaderDirty_ = false;
	}

	if (vertexDescDirty_ || instanceBufferDirty_)
	{
		ID3D11InputLayout* vertexFormat = GetD3D11InputLayout(vertexDesc_, instanceDesc_, vertexShaderD3D11);
		deviceContext_->IASetInputLayout(vertexFormat);

		vertexDescDirty_ = false;
	}

	instanceBufferDirty_ = false;

	{
		PROFILE_AUTO(CopyShaderParameter);

		CopyShaderParameterToBuffer(vertexShaderD3D11, vsConstantBuffer_);
		CopyShaderParameterToBuffer(pixelShaderD3D11, psConstantBuffer_);

		static ID3D11Buffer* d3dVSConstantBuffer[MAX_CONST_BUFFER] = { nullptr };
		static ID3D11Buffer* d3dPSConstantBuffer[MAX_CONST_BUFFER] = { nullptr };

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

		for (UInt32 slotID = 0; slotID < MAX_RENDERTARGET_COUNT; ++slotID)
		{
			auto renderTargetD3D11 = RTTICast<GfxRenderSurfaceD3D11>(renderTargets_[slotID]);
			d3dRenderTargetViews[slotID] = renderTargetD3D11 ? renderTargetD3D11->GetRenderTargetView() : nullptr;
		}

		auto depthStencilD3D11 = RTTICast<GfxRenderSurfaceD3D11>(depthStencil_);
		auto* d3dDepthStencilView = depthStencilD3D11 ? depthStencilD3D11->GetDepthStencilView() : nullptr;

		deviceContext_->OMSetRenderTargets(MAX_RENDERTARGET_COUNT, d3dRenderTargetViews, d3dDepthStencilView);

		renderTargetDirty_ = false;
		depthStencilDirty_ = false;
	}

	if (texturesDirty_ || samplerDirty_ || buffersDirty_)
	{
		static ID3D11ShaderResourceView* vertexShaderResourceView[MAX_GPU_UNITS_COUNT] = {};
		static ID3D11SamplerState* vertexSamplerState[MAX_GPU_UNITS_COUNT] = {};
		static ID3D11ShaderResourceView* pixelShaderResourceView[MAX_GPU_UNITS_COUNT] = {};
		static ID3D11SamplerState* pixelSamplerState[MAX_GPU_UNITS_COUNT] = {};

		for (UInt32 i = 0; i < MAX_GPU_UNITS_COUNT; ++i)
		{
			ID3D11ShaderResourceView* currentShaderResourceView = nullptr;

			if (auto* textureViewD3D11 = RTTICast<GfxShaderResourceViewD3D11>(textureViews_[i]))
			{
				currentShaderResourceView = textureViewD3D11->GetD3D11ShaderResourceView();
			}
			else if (auto* textureD3D11 = RTTICast<GfxTextureD3D11>(textures_[i]))
			{
				currentShaderResourceView = textureD3D11->GetD3D11ShaderResourceView();
			}
			else if (auto* bufferD3D11 = RTTICast<GfxBufferD3D11>(buffers_[i]))
			{
				currentShaderResourceView = bufferD3D11->GetShaderResourceView();
			}

			if (currentShaderResourceView)
			{
				if (vertexShaderD3D11->GetTextureDesc().Contains(i))
				{
					vertexShaderResourceView[i] = currentShaderResourceView;
					vertexSamplerState[i] = GetD3D11SamplerState(samplers_[i]);
				}
				else if (vertexShaderD3D11->GetStructBufferVariableDesc().Contains(i))
				{
					vertexShaderResourceView[i] = currentShaderResourceView;
					vertexSamplerState[i] = nullptr;
				}
				else
				{
					vertexShaderResourceView[i] = nullptr;
					vertexSamplerState[i] = nullptr;
				}

				if (pixelShaderD3D11->GetTextureDesc().Contains(i))
				{
					pixelShaderResourceView[i] = currentShaderResourceView;
					pixelSamplerState[i] = GetD3D11SamplerState(samplers_[i]);
				}
				else if (pixelShaderD3D11->GetStructBufferVariableDesc().Contains(i))
				{
					pixelShaderResourceView[i] = currentShaderResourceView;
					pixelSamplerState[i] = nullptr;
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

		deviceContext_->VSSetShaderResources(0, MAX_GPU_UNITS_COUNT, vertexShaderResourceView);
		deviceContext_->VSSetSamplers(0, MAX_GPU_UNITS_COUNT, vertexSamplerState);

		deviceContext_->PSSetShaderResources(0, MAX_GPU_UNITS_COUNT, pixelShaderResourceView);
		deviceContext_->PSSetSamplers(0, MAX_GPU_UNITS_COUNT, pixelSamplerState);

		texturesDirty_ = false;
		samplerDirty_ = false;
		buffersDirty_ = false;
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

	if (scissorRectDirty_)
	{
		if (rasterizerState_.scissorTest_)
		{
			D3D11_RECT d3d11Rect;
			d3d11Rect.left = scissorRect_.left_;
			d3d11Rect.top = scissorRect_.top_;
			d3d11Rect.right = scissorRect_.right_;
			d3d11Rect.bottom = scissorRect_.bottom_;

			deviceContext_->RSSetScissorRects(1, &d3d11Rect);
		}
		else
		{
			deviceContext_->RSSetScissorRects(0, nullptr);
		}

		scissorRectDirty_ = false;
	}
}

void GfxDeviceD3D11::PrepareDispatch()
{
	auto computeShaderD3D11 = RTTICast<GfxShaderD3D11>(computeShader_);
	ID3D11Buffer* d3dComputeConstantBuffer[MAX_CONST_BUFFER] = {};

	if (computeBufferDirty_)
	{
		ID3D11ShaderResourceView* d3dSRV[D3D11_PS_CS_UAV_REGISTER_COUNT] = {};
		ID3D11UnorderedAccessView* d3dUAV[D3D11_PS_CS_UAV_REGISTER_COUNT] = {};

		for (UInt32 i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; ++i)
		{
			auto* computeBufferD3D11 = RTTICast<GfxBufferD3D11>(computeBuffers_[i]);
			if (computeBufferD3D11)
			{
				if (computeBindFlags_[i] == COMPUTE_BIND_ACCESS_READ)
				{
					d3dSRV[i] = computeBufferD3D11->GetShaderResourceView();
				}
				else
				{
					d3dUAV[i] = computeBufferD3D11->GetUnorderedAccessViews();
				}
			}
		}

		deviceContext_->CSSetShaderResources(0, D3D11_PS_CS_UAV_REGISTER_COUNT, d3dSRV);
		deviceContext_->CSSetUnorderedAccessViews(0, D3D11_PS_CS_UAV_REGISTER_COUNT, d3dUAV, nullptr);

		computeBufferDirty_ = false;
	}

	if (shaderDirty_)
	{
		deviceContext_->CSSetShader(computeShaderD3D11->GetD3D11ComputeShader(), nullptr, 0);

		shaderDirty_ = false;
	}

	CopyShaderParameterToBuffer(computeShaderD3D11, csConstantBuffer_);

	for (UInt32 i = 0; i < MAX_CONST_BUFFER; ++i)
	{
		d3dComputeConstantBuffer[i] = csConstantBuffer_[i].GetD3D11Buffer();
	}

	deviceContext_->CSSetConstantBuffers(0, MAX_CONST_BUFFER, d3dComputeConstantBuffer);
}

void GfxDeviceD3D11::PrepareRasterizerState()
{
	if (rasterizerStateDirty_)
	{
		UInt32 stateHash = rasterizerState_.GetHash();
		if (!rasterizerStates_.Contains(stateHash))
		{
			int scaledDepthBias = (int)(rasterizerState_.depthBias_ * (1 << 24));

			D3D11_RASTERIZER_DESC stateDesc;
			Memory::Memzero(&stateDesc, sizeof(stateDesc));
			stateDesc.FillMode = d3d11FillMode[rasterizerState_.fillMode_];
			stateDesc.CullMode = d3d11CullMode[rasterizerState_.cullMode_];
			stateDesc.FrontCounterClockwise = FALSE;
			stateDesc.DepthBias = scaledDepthBias;
			stateDesc.DepthBiasClamp = F_INFINITY;

			stateDesc.SlopeScaledDepthBias = rasterizerState_.slopeScaledDepthBias_;
			stateDesc.DepthClipEnable = TRUE;
			stateDesc.ScissorEnable = rasterizerState_.scissorTest_ ? TRUE : FALSE;
			stateDesc.MultisampleEnable = FALSE;
			stateDesc.AntialiasedLineEnable = FALSE;

			ID3D11RasterizerState* newRasterizerState = nullptr;

			HRESULT hr = device_->CreateRasterizerState(&stateDesc, &newRasterizerState);
			if (FAILED(hr))
			{
				FLAGGG_LOG_ERROR("CreateRasterizerState failed.");
				D3D11_SAFE_RELEASE(newRasterizerState);
				return;
			}

			rasterizerStates_.Insert(MakePair(stateHash, newRasterizerState));
		}

		deviceContext_->RSSetState(rasterizerStates_[stateHash]);

		if (!blendStates_[rasterizerState_.blendMode_])
		{
			D3D11_BLEND_DESC blendDesc;
			Memory::Memzero(&blendDesc, sizeof(blendDesc));
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			blendDesc.RenderTarget[0].BlendEnable = rasterizerState_.blendMode_ == BLEND_REPLACE ? FALSE : TRUE;
			blendDesc.RenderTarget[0].SrcBlend = d3d11SourceRgbBlendFactor[rasterizerState_.blendMode_];
			blendDesc.RenderTarget[0].DestBlend = d3d11DestRgbBlendFactor[rasterizerState_.blendMode_];
			blendDesc.RenderTarget[0].BlendOp = d3d11RgbBlendOpt[rasterizerState_.blendMode_];
			blendDesc.RenderTarget[0].SrcBlendAlpha = d3d11SourceAlphaBlendFactor[rasterizerState_.blendMode_];
			blendDesc.RenderTarget[0].DestBlendAlpha = d3d11DestAlphaBlendFactor[rasterizerState_.blendMode_];
			blendDesc.RenderTarget[0].BlendOpAlpha = d3d11AlphaBlendOpt[rasterizerState_.blendMode_];
			blendDesc.RenderTarget[0].RenderTargetWriteMask = rasterizerState_.colorWrite_ ? D3D11_COLOR_WRITE_ENABLE_ALL : 0;

			HRESULT hr = device_->CreateBlendState(&blendDesc, &blendStates_[rasterizerState_.blendMode_]);
			if (FAILED(hr))
			{
				FLAGGG_LOG_ERROR("CreateBlendState failed.");
				D3D11_SAFE_RELEASE(blendStates_[rasterizerState_.blendMode_]);
				return;
			}
		}

		deviceContext_->OMSetBlendState(blendStates_[rasterizerState_.blendMode_], nullptr, F_MAX_UNSIGNED);

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
			Memory::Memzero(&d3d11DepthStencilDesc, sizeof(d3d11DepthStencilDesc));

			d3d11DepthStencilDesc.DepthEnable = true;
			d3d11DepthStencilDesc.DepthFunc = d3dComparisonFun[depthStencilState_.depthTestMode_];
			d3d11DepthStencilDesc.DepthWriteMask = depthStencilState_.depthWrite_ ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

			d3d11DepthStencilDesc.StencilEnable = depthStencilState_.stencilTest_;
			d3d11DepthStencilDesc.StencilReadMask = depthStencilState_.stencilReadMask_;
			d3d11DepthStencilDesc.StencilWriteMask = depthStencilState_.stencilWriteMask_;

			d3d11DepthStencilDesc.FrontFace.StencilFailOp = d3d11StencilOperation[depthStencilState_.stencilFailOp_];
			d3d11DepthStencilDesc.FrontFace.StencilDepthFailOp = d3d11StencilOperation[depthStencilState_.depthFailOp_];
			d3d11DepthStencilDesc.FrontFace.StencilPassOp = d3d11StencilOperation[depthStencilState_.depthStencilPassOp_];
			d3d11DepthStencilDesc.FrontFace.StencilFunc = d3dComparisonFun[depthStencilState_.stencilTestMode_];

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
				if (engineShaderParameters_)
					engineShaderParameters_->ReadParameter(variableDesc.name_, data + variableDesc.offset_, variableDesc.size_);

				if (materialShaderParameters_)
					materialShaderParameters_->ReadParameter(variableDesc.name_, data + variableDesc.offset_, variableDesc.size_);
			}
			constantBuffer.EndWrite(bufferDesc.size_);
		}
	}
}

ID3D11InputLayout* GfxDeviceD3D11::GetD3D11InputLayout(VertexDescription* vertxDesc, VertexDescription* instanceDesc, GfxShaderD3D11* vertexShader)
{
	auto key = MakePair(vertxDesc->GetUUID(), vertexShader);

	auto it = d3d11InputLayoutMap_.Find(key);
	if (it != d3d11InputLayoutMap_.End())
	{
		return it->second_;
	}
	
	PODVector<D3D11_INPUT_ELEMENT_DESC> d3d11InputElementDescs;

	const PODVector<VertexElement>& elements = vertxDesc->GetElements();

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

	if (instanceDesc)
	{
		const PODVector<VertexElement>& elements = instanceDesc->GetElements();

		for (const auto& element : elements)
		{
			const char* semName = VERTEX_ELEMENT_SEM_NAME[element.vertexElementSemantic_];

			D3D11_INPUT_ELEMENT_DESC desc;
			desc.SemanticName = semName;
			desc.SemanticIndex = element.index_;
			desc.Format = d3dElementFormats[element.vertexElementType_];
			desc.InputSlot = 1;
			desc.AlignedByteOffset = element.offset_;
			desc.InputSlotClass = element.perInstance_ ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = element.perInstance_ ? 1 : 0;
			d3d11InputElementDescs.Push(desc);
		}
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
	Memory::Memzero(&d3d11SamplerDesc, sizeof(d3d11SamplerDesc));
	d3d11SamplerDesc.Filter = d3d11Filter[desc.filterMode_];
	d3d11SamplerDesc.AddressU = d3dAddressMode[desc.addresMode_[TEXTURE_COORDINATE_U]];
	d3d11SamplerDesc.AddressV = d3dAddressMode[desc.addresMode_[TEXTURE_COORDINATE_V]];
	d3d11SamplerDesc.AddressW = d3dAddressMode[desc.addresMode_[TEXTURE_COORDINATE_W]];
	d3d11SamplerDesc.MaxAnisotropy = 1;
	d3d11SamplerDesc.MinLOD = 0;
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

AmbientOcclusionRendering* GfxDeviceD3D11::CreateAmbientOcclusionRendering()
{
	return new AmbientOcclusionRenderingD3D11();
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

