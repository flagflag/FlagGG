#include "GPUBuffer.h"
#include "Graphics/RenderEngine.h"
#include "IOFrame/Buffer/StringBuffer.h"
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
			uint32_t byteLeft = byteCount % 16u;
			if (byteLeft != 0u)
				byteCount += (16u - byteLeft);

			if (GetHandler() && gpuBufferSize_ == byteCount)
				return true;

			gpuBufferSize_ = byteCount;

			if (byteCount == 0)
			{
				ResetHandler(nullptr);
				return false;
			}

			D3D11_BUFFER_DESC bufferDesc;
			memset(&bufferDesc, 0, sizeof(bufferDesc));
			bufferDesc.BindFlags = GetBindFlags();
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //D3D11_USAGE_DEFAULT
			bufferDesc.ByteWidth = byteCount;

			ID3D11Buffer* buffer;
			HRESULT hr = RenderEngine::Instance()->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &buffer);
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
			if (gpuBufferSize_ == 0)
				return nullptr;

			D3D11_MAPPED_SUBRESOURCE mappedData;
			memset(&mappedData, 0, sizeof mappedData);
			HRESULT hr = RenderEngine::Instance()->GetDeviceContext()->Map(GetObject<ID3D11Buffer>(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
			if (FAILED(hr) || !mappedData.pData)
			{
				FLAGGG_LOG_ERROR("Failed to Map buffer data.");
				return nullptr;
			}
			return (char*)mappedData.pData + start;
		}

		void GPUBuffer::Unlock()
		{
			if (gpuBufferSize_ == 0)
				return;

			RenderEngine::Instance()->GetDeviceContext()->Unmap(GetObject<ID3D11Buffer>(), 0);
		}

		IOFrame::Buffer::IOBuffer* GPUBuffer::LockStaticBuffer(uint32_t start, uint32_t count)
		{
			void* data = Lock(start, count);
			buffer_ = new IOFrame::Buffer::StringBuffer(data, count);
			return buffer_;
		}

		void GPUBuffer::UnlockStaticBuffer()
		{
			buffer_.Reset();
			Unlock();
		}

		IOFrame::Buffer::IOBuffer* GPUBuffer::LockDynamicBuffer()
		{
			buffer_ = new IOFrame::Buffer::StringBuffer();
			return buffer_;
		}

		void GPUBuffer::UnlockDynamicBuffer()
		{
			SetSize(buffer_->GetSize());
			void* data = Lock(0, buffer_->GetSize());
			buffer_->ClearIndex();
			buffer_->ReadStream(data, buffer_->GetSize());
			Unlock();
			buffer_.Reset();
		}
	}
}
