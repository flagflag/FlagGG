#include "GPUBuffer.h"
#include "Graphics/RenderEngine.h"
#include "IOFrame/Buffer/StringBuffer.h"
#include "AsyncFrame/LockFree/Intrinsics.h"
#include "Log.h"

namespace FlagGG
{
	namespace Graphics
	{
		GPUBuffer::GPUBuffer() :
			gpuBufferSize_(0u),
			srcBuffer_(nullptr),
			dynamic_(false)
		{}

		bool GPUBuffer::IsValid()
		{
			return !!GetHandler();
		}

		void GPUBuffer::Initialize()
		{
		}

		bool GPUBuffer::SetSize(UInt32 byteCount)
		{
			UInt32 byteLeft = byteCount % 16u;
			if (byteLeft != 0u)
				byteCount += (16u - byteLeft);

			if (GetHandler() && gpuBufferSize_ == byteCount)
				return true;

			gpuBufferSize_ = byteCount;

			if (byteCount == 0)
			{
				ResetHandler(GPUHandler::INVALID);
				return false;
			}

			if (srcBuffer_)
			{
				delete[] srcBuffer_;
			}

			srcBuffer_ = new char[gpuBufferSize_];

			const bgfx::Memory* mem = bgfx::makeRef(srcBuffer_, gpuBufferSize_);

			Create(mem, dynamic_);
			
			return true;
		}

		void GPUBuffer::SetDynamic(bool dynamic)
		{
			dynamic_ = dynamic;
		}

		bool GPUBuffer::IsDynamic() const
		{
			return dynamic_;
		}

		void* GPUBuffer::Lock(UInt32 start, UInt32 count)
		{
			if (!dynamic_ || gpuBufferSize_ == 0)
				return nullptr;

			return srcBuffer_;
		}

		void GPUBuffer::Unlock()
		{
			if (!dynamic_ || gpuBufferSize_ == 0)
				return;

			const bgfx::Memory* mem = bgfx::makeRef(srcBuffer_, gpuBufferSize_);
			UpdateBuffer(mem);
		}

		IOFrame::Buffer::IOBuffer* GPUBuffer::LockStaticBuffer(UInt32 start, UInt32 count)
		{
			if (!dynamic_ || gpuBufferSize_ == 0)
				return nullptr;

			void* data = Lock(start, count);
			buffer_ = new IOFrame::Buffer::StringBuffer(data, count);
			return buffer_;
		}

		void GPUBuffer::UnlockStaticBuffer()
		{
			if (!dynamic_ || gpuBufferSize_ == 0)
				return;

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
