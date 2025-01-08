#include "GfxShaderD3D11.h"
#include "GfxDeviceD3D11.h"
#include "GfxD3D11Defines.h"
#include "GfxDevice/Shader/HlslShaderCompile.h"
#include "FileSystem/FileHandle/LocalFileHandle.h"
#include "Utility/SystemHelper.h"
#include "Core/CryAssert.h"
#include "Log.h"

namespace FlagGG
{

GfxShaderD3D11::GfxShaderD3D11()
	: GfxShader()
{

}

GfxShaderD3D11::~GfxShaderD3D11()
{
	D3D11_SAFE_RELEASE(shaderCode_);
	D3D11_SAFE_RELEASE(vertexShader_);
	D3D11_SAFE_RELEASE(pixelShader_);
}

void GfxShaderD3D11::AnalysisReflection(ID3DBlob* compileCode)
{
	ID3D11ShaderReflection* reflector = nullptr;
	HRESULT hr = GetSubsystem<D3DCompiler>()->D3DReflect(compileCode->GetBufferPointer(), compileCode->GetBufferSize(),
		GetSubsystem<D3DCompiler>()->IID_ID3D11ShaderReflection, (void**)&reflector);

	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("Failed to reflect shader.");
		return;
	}

	D3D11_SHADER_DESC shaderDesc;
	hr = reflector->GetDesc(&shaderDesc);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("Failed to get shader desc.");
		return;
	}

	HashMap<StringHash, UInt32> cbufferBindMap;
	for (UInt32 i = 0; i < shaderDesc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		hr = reflector->GetResourceBindingDesc(i, &bindDesc);
		if (FAILED(hr))
		{
			FLAGGG_LOG_ERROR("Failed to get resource bind desc.");
			return;
		}

		switch (bindDesc.Type)
		{
		case D3D_SIT_CBUFFER:
			cbufferBindMap[bindDesc.Name] = bindDesc.BindPoint;
			break;

		case D3D_SIT_TEXTURE:
		{
			D3D11ShaderTextureDesc& desc = textureDescs_[bindDesc.BindPoint];
			desc.textureName_ = bindDesc.Name;
		}
		break;

		case D3D_SIT_SAMPLER:
		{
			D3D11ShaderTextureDesc& desc = textureDescs_[bindDesc.BindPoint];
			desc.samplerName_ = bindDesc.Name;
		}
		break;

		case D3D_SIT_STRUCTURED:
		{
			D3D11StructBufferDesc& dest = structBufferDescs_[bindDesc.BindPoint];
			dest.name_ = bindDesc.Name;
			dest.readonly_ = true;
		}
		break;

		case D3D_SIT_UAV_RWSTRUCTURED:
		{
			D3D11StructBufferDesc& dest = structBufferDescs_[bindDesc.BindPoint];
			dest.name_ = bindDesc.Name;
			dest.readonly_ = false;
		}
		break;
		}
	}

	for (UInt32 i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* rConstantBuffer = reflector->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC d3dBufferDesc;
		rConstantBuffer->GetDesc(&d3dBufferDesc);

		if (auto* index = cbufferBindMap.TryGetValue(d3dBufferDesc.Name))
		{
			auto& bufferDesc = constantBufferDescs_[*index];
			bufferDesc.name_ = d3dBufferDesc.Name;
			bufferDesc.size_ = d3dBufferDesc.Size;

			for (UInt32 j = 0; j < d3dBufferDesc.Variables; ++j)
			{
				ID3D11ShaderReflectionVariable* variable = rConstantBuffer->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC d3dVariableDesc;
				variable->GetDesc(&d3dVariableDesc);

				D3D11ConstantBufferVariableDesc variableDesc;
				variableDesc.name_ = d3dVariableDesc.Name;
				variableDesc.offset_ = d3dVariableDesc.StartOffset;
				variableDesc.size_ = d3dVariableDesc.Size;
				bufferDesc.variableDescs_.Push(variableDesc);
			}
		}
	}

	D3D11_SAFE_RELEASE(reflector);
}

bool GfxShaderD3D11::Compile()
{
	ID3DBlob* compileCode = nullptr;
	if (!CompileShader(shaderSource_.Get(), shaderSourceSize_, shaderType_, defines_, compileCode, shaderCode_))
	{
		FLAGGG_LOG_ERROR("Failed to compile shader.");
		return false;
	}

	AnalysisReflection(compileCode);
	D3D11_SAFE_RELEASE(compileCode);

	if (shaderCode_)
	{
		if (shaderType_ == VS)
		{
			HRESULT hr = GetSubsystem<GfxDeviceD3D11>()->GetD3D11Device()->CreateVertexShader(
				shaderCode_->GetBufferPointer(),
				shaderCode_->GetBufferSize(),
				nullptr,
				&vertexShader_
			);
			if (hr != 0)
			{
				FLAGGG_LOG_ERROR("CreateVertexShader failed.");
				D3D11_SAFE_RELEASE(vertexShader_);
				return false;
			}
		}
		else if (shaderType_ == PS)
		{
			HRESULT hr = GetSubsystem<GfxDeviceD3D11>()->GetD3D11Device()->CreatePixelShader(
				shaderCode_->GetBufferPointer(),
				shaderCode_->GetBufferSize(),
				nullptr,
				&pixelShader_
			);
			if (hr != 0)
			{
				FLAGGG_LOG_ERROR("CreatePixelShader failed.");
				D3D11_SAFE_RELEASE(pixelShader_);
				return false;
			}
		}
		else if (shaderType_ == CS)
		{
			HRESULT hr = GetSubsystem<GfxDeviceD3D11>()->GetD3D11Device()->CreateComputeShader(
				shaderCode_->GetBufferPointer(),
				shaderCode_->GetBufferSize(),
				nullptr,
				&computeShader_
			);
			if (hr != 0)
			{
				FLAGGG_LOG_ERROR("CreateComputeShader failed.");
				D3D11_SAFE_RELEASE(pixelShader_);
				return false;
			}
		}
	}

	return true;
}

}
