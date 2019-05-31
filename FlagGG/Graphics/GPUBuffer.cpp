#include "GPUBuffer.h"
#include "Graphics/RenderEngine.h"
#include "Log.h"

namespace FlagGG
{
	namespace Graphics
	{
		bool GPUBuffer::IsValid()
		{
			return !!GetHandler();
		}

		void GPUBuffer::Initialize()
		{
		}

		bool GPUBuffer::SetSize(uint32_t byteCount)
		{
			D3D11_BUFFER_DESC bufferDesc;
			memset(&bufferDesc, 0, sizeof(bufferDesc));
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //D3D11_USAGE_DEFAULT
			bufferDesc.ByteWidth = byteCount;

			ID3D11Buffer* buffer;
			HRESULT hr = RenderEngine::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &buffer);
			if (FAILED(hr))
			{
				SAFE_RELEASE(buffer);
				FLAGGG_LOG_ERROR("Failed to create vertex buffer.");
				return false;
			}

			ResetHandler(buffer);
			
			return true;
		}

		void* GPUBuffer::Lock(uint32_t start, uint32_t count)
		{
			D3D11_MAPPED_SUBRESOURCE mappedData;
			memset(&mappedData, 0, sizeof mappedData);
			HRESULT hr = RenderEngine::GetDeviceContext()->Map(GetObject<ID3D11Buffer>(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
			if (FAILED(hr) || !mappedData.pData)
			{
				FLAGGG_LOG_ERROR("Failed to Map buffer data.");
				return nullptr;
			}
			return mappedData.pData;
		}

		void GPUBuffer::Unlock()
		{
			RenderEngine::GetDeviceContext()->Unmap(GetObject<ID3D11Buffer>(), 0);
		}
	}
}
