#include "RenderEngine.h"
#include "Camera.h"

#include <assert.h>

namespace FlagGG
{
	namespace Graphics
	{
		ID3D11Device* RenderEngine::device_ = nullptr;

		ID3D11DeviceContext* RenderEngine::deviceContext_ = nullptr;

		ID3D11RasterizerState* RenderEngine::rasterizerState_ = nullptr;

		ID3D11Buffer* RenderEngine::matrixData_ = nullptr;

		MaterialQuality RenderEngine::textureQuality_ = QUALITY_HIGH;

		struct MatrixData
		{
			Math::Matrix4 world;
			Math::Matrix4 view;
			Math::Matrix4 projection;
		};

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
				puts("D3D11CreateDevice failed.");

				SAFE_RELEASE(device_);
				SAFE_RELEASE(deviceContext_);

				return;
			}
		}

		void RenderEngine::CreateRasterizerState()
		{
			D3D11_RASTERIZER_DESC stateDesc;
			memset(&stateDesc, 0, sizeof(stateDesc));
			stateDesc.FillMode = D3D11_FILL_SOLID;
			stateDesc.CullMode = D3D11_CULL_BACK;
			stateDesc.FrontCounterClockwise = false;
			stateDesc.DepthBias = 0;
			stateDesc.DepthBiasClamp = 0.0f;

			stateDesc.SlopeScaledDepthBias = 0;
			stateDesc.DepthClipEnable = false;
			stateDesc.ScissorEnable = false;
			stateDesc.MultisampleEnable = false;
			stateDesc.AntialiasedLineEnable = false;

			HRESULT hr = device_->CreateRasterizerState(&stateDesc, &rasterizerState_);
			if (hr != 0)
			{
				puts("CreateRasterizerState failed.");

				SAFE_RELEASE(rasterizerState_);

				return;
			}

			deviceContext_->RSSetState(rasterizerState_);
		}

		void RenderEngine::CreateMatrixData()
		{
			D3D11_BUFFER_DESC bufferDesc;
			memset(&bufferDesc, 0, sizeof(bufferDesc));
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth = sizeof(MatrixData);
			HRESULT hr = RenderEngine::GetDevice()->CreateBuffer(&bufferDesc, NULL, &matrixData_);
			if (hr != 0)
			{
				puts("MatrixData CreateBuffer failed.");

				SAFE_RELEASE(matrixData_);

				return;
			}
		}

		void RenderEngine::Initialize()
		{
			CreateDevice();

			CreateRasterizerState();

			CreateMatrixData();
		}

		void RenderEngine::Uninitialize()
		{
			SAFE_RELEASE(device_);
			SAFE_RELEASE(deviceContext_);
			SAFE_RELEASE(rasterizerState_);
			SAFE_RELEASE(matrixData_);
		}

		ID3D11Device* RenderEngine::GetDevice()
		{
			return device_;
		}

		ID3D11DeviceContext* RenderEngine::GetDeviceContext()
		{
			return deviceContext_;
		}

		bool RenderEngine::CheckMultiSampleSupport(DXGI_FORMAT format, uint32_t sampleCount)
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

		uint32_t RenderEngine::GetMultiSampleQuality(DXGI_FORMAT format, uint32_t sampleCount)
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

		uint32_t RenderEngine::GetAlphaFormat()
		{
			return DXGI_FORMAT_A8_UNORM;
		}

		uint32_t RenderEngine::GetRGBAFormat()
		{
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		}

		uint32_t RenderEngine::GetFormat(Resource::CompressedFormat format)
		{
			switch (format)
			{
			case Resource::CF_RGBA:
				return DXGI_FORMAT_R8G8B8A8_UNORM;

			case Resource::CF_DXT1:
				return DXGI_FORMAT_BC1_UNORM;

			case Resource::CF_DXT3:
				return DXGI_FORMAT_BC2_UNORM;

			case Resource::CF_DXT5:
				return DXGI_FORMAT_BC3_UNORM;

			default:
				return 0;
			}
		}

		void RenderEngine::UpdateMatrix(Camera* camera)
		{
			assert(camera);

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			unsigned int bufferNumber;

			RenderEngine::GetDeviceContext()->Map(matrixData_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			MatrixData* dataPtr = static_cast<MatrixData*>(mappedResource.pData);
			dataPtr->world		= Math::Matrix4::IDENTITY.Transpose();
			dataPtr->view		= camera->GetViewMatrix().Transpose();
			dataPtr->projection = camera->GetProjectionMatrix().Transpose();

			RenderEngine::GetDeviceContext()->Unmap(matrixData_, 0);

			RenderEngine::GetDeviceContext()->VSSetConstantBuffers(0, 1, &matrixData_);
		}
	}
}