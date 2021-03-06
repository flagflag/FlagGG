#include "Graphics/Shader.h"
#include "Graphics/RenderEngine.h"
#include "Allocator/SmartMemory.hpp"
#include "Container/ArrayPtr.h"
#include "Container/Sort.h"
#include "IOFrame/Stream/FileStream.h"
#include "Resource/ResourceCache.h"
#include "Core/Context.h"
#include "Log.h"

#include <d3dcompiler.h>

#include <fstream>

namespace FlagGG
{
	namespace Graphics
	{
		static Container::String HashVectorString(Container::Vector<Container::String> vecStr)
		{
			Container::Sort(vecStr.Begin(), vecStr.End());
			Container::String hash;
			for (const auto& str : vecStr)
			{
				hash += str;
			}
			return hash;
		}

		ShaderCode::ShaderCode(Core::Context* context) :
			Resource(context)
		{ }

		Shader* ShaderCode::GetShader(ShaderType type, const Container::Vector<Container::String>& defines)
		{
			Container::Vector<Container::String> newDefines = defines;
			newDefines.Push(type == VS ? "VERTEX" : "PIXEL");
			Container::String definesStr = HashVectorString(newDefines);
			for (const auto& shader : shaders_)
			{
				if (shader->GetType() == type && shader->GetDefinesString() == definesStr)
				{
					return shader;
				}
			}
			
			Container::SharedPtr<Shader> shader(new Shader(buffer_, bufferSize_));
			shader->SetType(type);
			shader->SetDefines(newDefines);
			shader->Initialize();
			shaders_.Push(shader);

			return shader;
		}

		bool ShaderCode::PreCompileShaderCode(const char* head, const char* tail, Container::String& out)
		{
			while (head != tail)
			{
				while (head != tail && (*head == '\n' || *head == '\r' || *head == ' ' || *head == '\t')) ++head;
				if (head + 10 < tail &&
					head[0] == '#' &&
					head[1] == 'i' &&
					head[2] == 'n' &&
					head[3] == 'c' &&
					head[4] == 'l' &&
					head[5] == 'u' &&
					head[6] == 'd' &&
					head[7] == 'e')
				{
					head += 8;
					while (head != tail && (*head == ' ' || *head == '\t')) ++head;
					if (*head == '\"')
					{
						++head;
						const char* temp = head;
						while (temp != tail && *temp != '\"') ++temp;

						Container::String relativePath(head, temp - head);

						if (*temp != '\"')
						{
							FLAGGG_LOG_ERROR("Pre compile shader error: missing '\"' behind #include\"{}.", relativePath.CString());
							return false;
						}

						head = temp + 1;

						while (head != tail && (*head == ' ' || *head == '\t')) ++head;

						// 这个判断不严谨，懒得写了，将就用着。
						if (head != tail && *head != '\r' && *head != '\n')
						{
							FLAGGG_LOG_ERROR("Pre compile shader error: missing wrap behind #include\"{}\".", relativePath.CString());
							return false;
						}

						auto file = context_->GetVariable<FlagGG::Resource::ResourceCache>("ResourceCache")->GetFile(relativePath);
						if (!file)
						{
							FLAGGG_LOG_ERROR("Pre compile shader error: can not open file[{}].", relativePath.CString());
							return false;
						}

						UInt32 bufferSize = 0u;
						Container::SharedArrayPtr<char> buffer;
						file->ToBuffer(buffer, bufferSize);
						if (!PreCompileShaderCode(buffer.Get(), buffer.Get() + bufferSize, out))
							return false;
					}
					else
					{
						FLAGGG_LOG_ERROR("Pre compile shader error: missing filename behind #include.");
						return false;
					}
				}
				else
				{
					break;
				}
			}

			out.Append(head, tail - head);
		}

		bool ShaderCode::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
		{
			Container::String buffer;
			stream->ToString(buffer);

			Container::String out;
			PreCompileShaderCode(buffer.CString(), buffer.CString() + buffer.Length(), out);

			bufferSize_ = out.Length();
			buffer_ = new char[bufferSize_];
			memcpy(buffer_.Get(), out.CString(), bufferSize_);

			return true;
		}

		bool ShaderCode::EndLoad()
		{
			return true;
		}

// --------------------------------------------------------------------------------------------------------------

		Shader::Shader(Container::SharedArrayPtr<char> buffer, UInt32 bufferSize) :
			GPUObject(),
			buffer_(buffer),
			bufferSize_(bufferSize)
		{ }

		Shader::~Shader()
		{
			SAFE_RELEASE(shaderCode_);
		}

		static bool CompileShader(const char* buffer, Size bufferSize, ShaderType type, const Container::Vector<Container::String>& defines, ID3DBlob*& outCompileCode, ID3DBlob*& outStrippedCode)
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

			Container::PODVector<D3D_SHADER_MACRO> macros;

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

					SAFE_RELEASE(errorMsgs);
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

			SAFE_RELEASE(errorMsgs);

			outCompileCode = shaderCode;
			outStrippedCode = strippedCode;

			return true;
		}

		void Shader::Initialize()
		{
			if (shaderType_ == None)
			{
				FLAGGG_LOG_WARN("Shader type is None, so initialize failed.");

				return;
			}

			ID3DBlob* compileCode = nullptr;
			if (!CompileShader(buffer_.Get(), bufferSize_, shaderType_, defines_, compileCode, shaderCode_))
			{
				FLAGGG_LOG_ERROR("Failed to compile shader.");
				return;
			}

			AnalysisReflection(compileCode);
			SAFE_RELEASE(compileCode);

			if (shaderCode_)
			{
				if (shaderType_ == VS)
				{
					ID3D11VertexShader* vertexShader;

					HRESULT hr = RenderEngine::Instance()->GetDevice()->CreateVertexShader(
						shaderCode_->GetBufferPointer(),
						shaderCode_->GetBufferSize(),
						nullptr,
						&vertexShader
						);
					if (hr != 0)
					{
						FLAGGG_LOG_ERROR("CreateVertexShader failed.");

						SAFE_RELEASE(vertexShader);

						return;
					}

					ResetHandler(vertexShader);
				}
				else if (shaderType_ == PS)
				{
					ID3D11PixelShader* pixelShader;

					HRESULT hr = RenderEngine::Instance()->GetDevice()->CreatePixelShader(
						shaderCode_->GetBufferPointer(),
						shaderCode_->GetBufferSize(),
						nullptr,
						&pixelShader
						);
					if (hr != 0)
					{
						FLAGGG_LOG_ERROR("CreatePixelShader failed.");

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

		void Shader::SetType(ShaderType type)
		{
			shaderType_ = type;
		}

		void Shader::SetDefines(const Container::Vector<Container::String> &defines)
		{
			defines_ = defines;

			definesString_ = HashVectorString(defines_);
		}

		Container::String Shader::GetDefinesString() const
		{
			return definesString_;
		}

		ShaderType Shader::GetType()
		{
			return shaderType_;
		}

		ID3DBlob* Shader::GetByteCode()
		{
			return shaderCode_;
		}

		const Container::HashMap<UInt32, ConstantBufferDesc>& Shader::GetContantBufferVariableDesc() const
		{
			return constantBufferDescs_;
		}

		const Container::HashMap<UInt32, TextureDesc>& Shader::GetTextureDesc() const
		{
			return textureDescs_;
		}

		void Shader::AnalysisReflection(ID3DBlob* compileCode)
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

			Container::HashMap<Container::StringHash, UInt32> bindMap;
			for (UInt32 i = 0; i < shaderDesc.BoundResources; ++i)
			{
				D3D11_SHADER_INPUT_BIND_DESC bindDesc;
				hr = reflector->GetResourceBindingDesc(i, &bindDesc);
				if (FAILED(hr))
				{
					FLAGGG_LOG_ERROR("Failed to get resource bind desc.");
					return;
				}

				switch(bindDesc.Type)
				{
				case D3D_SIT_CBUFFER:
					bindMap[bindDesc.Name] = bindDesc.BindPoint;
				break;

				case D3D_SIT_TEXTURE:
				{
					TextureDesc& desc = textureDescs_[bindDesc.BindPoint];
					desc.textureName_ = bindDesc.Name;
				}
				break;

				case D3D_SIT_SAMPLER:
				{
					TextureDesc& desc = textureDescs_[bindDesc.BindPoint];
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

			SAFE_RELEASE(reflector);
		}
	}
}
