#include "Graphics/Shader.h"
#include "Graphics/RenderEngine.h"
#include "Allocator/SmartMemory.hpp"
#include "Container/ArrayPtr.h"
#include "Log.h"

#include <d3dcompiler.h>

#include <fstream>

namespace FlagGG
{
	namespace Graphics
	{
		Shader::Shader(Core::Context* context) :
			GPUObject(),
			Resource(context)
		{ }

		Shader::~Shader()
		{
			SAFE_RELEASE(shaderCode_);
		}

		static ID3DBlob* CompileShader(const char* buffer, size_t bufferSize, ShaderType type)
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

			ID3DBlob* shaderCode = nullptr;
			ID3DBlob* errorMsgs = nullptr;

			HRESULT hr = D3DCompile(
				buffer,
				bufferSize,
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
				FLAGGG_LOG_ERROR("D3DCompile failed.");

				if (errorMsgs)
				{
					FLAGGG_LOG_ERROR("Error code: %s", errorMsgs->GetBufferPointer());

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
			if (shaderType_ == None)
			{
				FLAGGG_LOG_WARN("Shader type is None, so initialize failed.");

				return;
			}

			shaderCode_ = CompileShader(buffer_.Get(), bufferSize_, shaderType_);

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

		bool Shader::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
		{
			char* buffer = nullptr;
			bufferSize_ = 0;
			stream->ToString(buffer, bufferSize_);
			buffer_ = buffer;

			return true;
		}

		bool Shader::EndLoad()
		{
			return true;
		}

		void Shader::SetType(ShaderType type)
		{
			shaderType_ = type;
		}

		ShaderType Shader::GetType()
		{
			return shaderType_;
		}

		ID3DBlob* Shader::GetByteCode()
		{
			return shaderCode_;
		}
	}
}
