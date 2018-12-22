#include "RenderEngine.h"
#include "Allocator/SmartMemory.hpp"

#include <d3dcompiler.h>

#include <string>
#include <fstream>

namespace FlagGG
{
	namespace Graphics
	{
		ID3D11Device* RenderEngine::device_ = nullptr;

		ID3D11DeviceContext* RenderEngine::deviceContext_ = nullptr;

		ID3D11VertexShader* RenderEngine::vertexShader_ = nullptr;

		ID3D11PixelShader* RenderEngine::pixelShader_ = nullptr;

		ID3D11InputLayout* RenderEngine::inputLayout_ = nullptr;

		void RenderEngine::CreateDevice()
		{
			HRESULT hr = D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				0,
				nullptr,
				0,
				D3D11_SDK_VERSION,
				&device_,
				nullptr,
				&deviceContext_
				);

			if (hr != 0)
			{
				puts("D3D11CreateDevice failed.");

				SAFE_RELEASE(device_);
				SAFE_RELEASE(deviceContext_);

				return;
			}
		}

		static ID3DBlob* GetShaderCode(const std::wstring& path, int type)
		{
			char* entryPoint = nullptr;
			char* profile = nullptr;
			//unsigned flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
			unsigned flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
			flags |= D3DCOMPILE_DEBUG;
#endif

			if (type == 0) // VS
			{
				entryPoint = "VS";
				profile = "vs_4_0";
			}
			else // PS
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

		void RenderEngine::CreateShader()
		{
			ID3DBlob* VSCode = GetShaderCode(L"E:\\Shader\\test.hlsl", 0);
			if (VSCode)
			{
				HRESULT hr = device_->CreateVertexShader(
					VSCode->GetBufferPointer(),
					VSCode->GetBufferSize(),
					nullptr,
					(ID3D11VertexShader**)&vertexShader_
					);
				if (hr != 0)
				{
					puts("CreateVertexShader failed.");

					SAFE_RELEASE(vertexShader_);
					SAFE_RELEASE(VSCode);

					return;
				}
			}

			D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
			{
				{
					"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,

					0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
				}
			};

			unsigned int totalLayoutElements = ARRAYSIZE(solidColorLayout);

			HRESULT hr = device_->CreateInputLayout(
				solidColorLayout,
				totalLayoutElements,
				VSCode->GetBufferPointer(),
				VSCode->GetBufferSize(),
				&inputLayout_
				);
			if (hr != 0)
			{
				puts("CreateInputLayout failed.");

				return;
			}

			SAFE_RELEASE(VSCode);

			ID3DBlob* PSCode = GetShaderCode(L"E:\\Shader\\test.hlsl", 1);
			if (PSCode)
			{
				HRESULT hr = device_->CreatePixelShader(
					PSCode->GetBufferPointer(),
					PSCode->GetBufferSize(),
					nullptr,
					(ID3D11PixelShader**)&pixelShader_
					);
				if (hr != 0)
				{
					puts("CreatePixelShader failed.");

					SAFE_RELEASE(pixelShader_);
					SAFE_RELEASE(PSCode);

					return;
				}
			}
			SAFE_RELEASE(PSCode);
		}

		void RenderEngine::Initialize()
		{
			CreateDevice();

			if (!device_ || !deviceContext_) return;

			CreateShader();
		}

		void RenderEngine::Uninitialize()
		{
			SAFE_RELEASE(device_);
			SAFE_RELEASE(deviceContext_);
			SAFE_RELEASE(vertexShader_);
			SAFE_RELEASE(pixelShader_);
			SAFE_RELEASE(inputLayout_);
		}

		ID3D11Device* RenderEngine::GetDevice()
		{
			return device_;
		}

		ID3D11DeviceContext* RenderEngine::GetDeviceContext()
		{
			return deviceContext_;
		}

		void RenderEngine::Update()
		{
			deviceContext_->IASetInputLayout(inputLayout_);
			deviceContext_->VSSetShader(vertexShader_, nullptr, 0);
			deviceContext_->PSSetShader(pixelShader_, nullptr, 0);
		}
	}
}