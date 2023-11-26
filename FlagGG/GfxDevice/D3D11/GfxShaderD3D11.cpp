#include "GfxShaderD3D11.h"
#include "GfxDeviceD3D11.h"
#include "GfxD3D11Defines.h"
#include "Core/CryAssert.h"
#include "Log.h"

#include <d3dcompiler.h>

namespace FlagGG
{

static bool CompileShader(const char* buffer, Size bufferSize, ShaderType type, const Vector<String>& defines, ID3DBlob*& outCompileCode, ID3DBlob*& outStrippedCode)
{
	char* entryPoint = nullptr;
	char* profile = nullptr;
	//unsigned flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
	unsigned flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	if (type == VS) // VS
	{
		entryPoint = "VS";
		profile = "vs_4_0";
	}
	else if (type == PS) // PS
	{
		entryPoint = "PS";
		profile = "ps_4_0";
		//flags |= D3DCOMPILE_PREFER_FLOW_CONTROL;
	}

	ID3DBlob* shaderCode = nullptr;
	ID3DBlob* errorMsgs = nullptr;

	PODVector<D3D_SHADER_MACRO> macros;

	for (UInt32 i = 0; i < defines.Size(); ++i)
	{
		D3D_SHADER_MACRO macro;
		macro.Name = defines[i].CString();
		macro.Definition = "1";
		macros.Push(macro);
	}

	D3D_SHADER_MACRO emptyMacro;
	emptyMacro.Name = nullptr;
	emptyMacro.Definition = nullptr;
	macros.Push(emptyMacro);

	HRESULT hr = D3DCompile(
		buffer,
		bufferSize,
		nullptr,
		&macros[0],
		nullptr,
		entryPoint,
		profile,
		flags,
		0,
		&shaderCode,
		&errorMsgs
	);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("D3DCompile failed.");

		if (errorMsgs)
		{
			FLAGGG_LOG_ERROR("Error code: {}", (const char*)errorMsgs->GetBufferPointer());

			ASSERT_MESSAGE(false, (const char*)errorMsgs->GetBufferPointer());

			D3D11_SAFE_RELEASE(errorMsgs);
		}

		return false;
	}

	ID3DBlob* strippedCode = nullptr;
	hr = D3DStripShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(),
		D3DCOMPILER_STRIP_REFLECTION_DATA | D3DCOMPILER_STRIP_DEBUG_INFO | D3DCOMPILER_STRIP_TEST_BLOBS, &strippedCode);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("D3DStripShader failed.");

		return false;
	}

	D3D11_SAFE_RELEASE(errorMsgs);

	outCompileCode = shaderCode;
	outStrippedCode = strippedCode;

	return true;
}

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
	HRESULT hr = D3DReflect(compileCode->GetBufferPointer(), compileCode->GetBufferSize(),
		IID_ID3D11ShaderReflection, (void**)&reflector);

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

	HashMap<StringHash, UInt32> bindMap;
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
			bindMap[bindDesc.Name] = bindDesc.BindPoint;
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
		}
	}

	for (UInt32 i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* rConstantBuffer = reflector->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC d3dBufferDesc;
		rConstantBuffer->GetDesc(&d3dBufferDesc);

		UInt32 index = bindMap[d3dBufferDesc.Name];
		auto& bufferDesc = constantBufferDescs_[index];
		bufferDesc.name_ = d3dBufferDesc.Name;
		bufferDesc.size_ = d3dBufferDesc.Size;

		for (UInt32 j = 0; j < d3dBufferDesc.Variables; ++j)
		{
			ID3D11ShaderReflectionVariable* variable = rConstantBuffer->GetVariableByIndex(j);
			D3D11_SHADER_VARIABLE_DESC d3dVariableDesc;
			variable->GetDesc(&d3dVariableDesc);

			ConstantBufferVariableDesc variableDesc;
			variableDesc.name_ = d3dVariableDesc.Name;
			variableDesc.offset_ = d3dVariableDesc.StartOffset;
			variableDesc.size_ = d3dVariableDesc.Size;
			bufferDesc.variableDescs_.Push(variableDesc);
		}
	}

	D3D11_SAFE_RELEASE(reflector);
}

void GfxShaderD3D11::Compile()
{
	if (shaderType_ == None)
	{
		FLAGGG_LOG_WARN("Shader type is None, so initialize failed.");
		return;
	}

	ID3DBlob* compileCode = nullptr;
	if (!CompileShader(shaderSource_.Get(), shaderSourceSize_, shaderType_, defines_, compileCode, shaderCode_))
	{
		FLAGGG_LOG_ERROR("Failed to compile shader.");
		return;
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
				return;
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
				return;
			}
		}
	}
}

}
