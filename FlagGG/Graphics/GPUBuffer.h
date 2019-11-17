#pragma once
#include "Export.h"
#include "Graphics/GraphicsDef.h"
#include "Graphics/GPUObject.h"
#include "Container/RefCounted.h"
#include "IOFrame/Buffer/IOBuffer.h"

namespace FlagGG
{
	namespace Graphics
	{
		class FlagGG_API GPUBuffer : public GPUObject, public Container::RefCounted
		{
		public:
			bool IsValid() override;

			bool SetSize(uint32_t byteCount);

			void* Lock(uint32_t start, uint32_t count);

			void Unlock();

			IOFrame::Buffer::IOBuffer* LockStaticBuffer(uint32_t start, uint32_t count);

			void UnlockStaticBuffer();

			IOFrame::Buffer::IOBuffer* LockDynamicBuffer();

			void UnlockDynamicBuffer();

			virtual uint32_t GetBindFlags() = 0;

		protected:
			void Initialize() override;

			Container::SharedPtr<IOFrame::Buffer::IOBuffer> buffer_;

			uint32_t gpuBufferSize_{ 0u };
		};
	}
}
