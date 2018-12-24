#include "Shader.h"
#include "RenderEngine.h"
#include "Allocator/SmartMemory.hpp"

#include <d3dcompiler.h>

#include <fstream>

namespace FlagGG
{
	namespace Graphics
	{
		Shader::Shader(const std::wstring& shaderPath, ShaderType shaderType) :
			shaderPath_(shaderPath),
			shaderType_(shaderType)
		{
		}

		Shader::~Shader()
		{
			SAFE_RELEASE(shaderCode_);
		}

		static ID3DBlob* GetShaderCode(const std::wstring& path, ShaderType type)
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
			else if(type == PS) // PS
			{
				entryPoint = "PS";
				profile = "ps_4_0";
				//flags |= D3DCOMPILE_PREFER_FLOW_CONTROL;
			}

			std::ifstream stream;
			stream.open(path, std::ios::in | std::ios::binary | std::ios::ate);
			if (!stream.is_open())
			{
				return nullptr;
			}

			std::streamoff sourceCodeSize = stream.tellg();
			Allocator::SmartMemory<char> sourceCode(sourceCodeSize);

			stream.seekg(0, std::ios::beg);
			stream.read(sourceCode.get(), sourceCodeSize);

			ID3DBlob* shaderCode = nullptr;
			ID3DBlob* errorMsgs = nullptr;

			HRESULT hr = D3DCompile(
				sourceCode.get(),
				sourceCodeSize,
				nullptr,
				nullptr,
				nullptr,
				entryPoint,
				profile,
				flags,
				0,
				&shaderCode,
				&errorMsgs
				);
			if (hr != 0)
			{
				puts("D3DCompile failed.");

				if (errorMsgs)
				{
					printf("%s\n", errorMsgs->GetBufferPointer());

					SAFE_RELEASE(errorMsgs);
				}

				return nullptr;
			}

			ID3DBlob* strippedCode = nullptr;
			hr = D3DStripShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(),
				D3DCOMPILER_STRIP_REFLECTION_DATA | D3DCOMPILER_STRIP_DEBUG_INFO | D3DCOMPILER_STRIP_TEST_BLOBS, &strippedCode);
			if (hr != 0)
			{
				puts("D3DStripShader failed.");

				return nullptr;
			}

			SAFE_RELEASE(shaderCode);
			SAFE_RELEASE(errorMsgs);

			return strippedCode;
		}

		void Shader::Initialize()
		{
			shaderCode_ = GetShaderCode(shaderPath_, shaderType_);
			if (shaderCode_)
			{
				if (shaderType_ == VS)
				{
					ID3D11VertexShader* vertexShader;

					HRESULT hr = RenderEngine::GetDevice()->CreateVertexShader(
						shaderCode_->GetBufferPointer(),
						shaderCode_->GetBufferSize(),
						nullptr,
						&vertexShader
						);
					if (hr != 0)
					{
						puts("CreateVertexShader failed.");

						SAFE_RELEASE(vertexShader);

						return;
					}

					ResetHandler(vertexShader);
				}
				else if (shaderType_ == PS)
				{
					ID3D11PixelShader* pixelShader;

					HRESULT hr = RenderEngine::GetDevice()->CreatePixelShader(
						shaderCode_->GetBufferPointer(),
						shaderCode_->GetBufferSize(),
						nullptr,
						&pixelShader
						);
					if (hr != 0)
					{
						puts("CreatePixelShader failed.");

						SAFE_RELEASE(pixelShader);

						return;
					}

					ResetHandler(pixelShader);
				}
			}
		}

		bool Shader::IsValid()
		{
			return GetHandler() != nullptr && shaderCode_ != nullptr;
		}

		ID3DBlob* Shader::GetByteCode()
		{
			return shaderCode_;
		}
	}
}
