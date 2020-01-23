#pragma once
#include "Export.h"
#include "Graphics/GraphicsDef.h"
#include "Graphics/GPUObject.h"
#include "Container/RefCounted.h"
#include "IOFrame/Buffer/IOBuffer.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{
	namespace Graphics
	{
		class FlagGG_API GPUBuffer : public GPUObject, public Container::RefCounted
		{
		public:
			bool IsValid() override;

			bool SetSize(UInt32 byteCount);

			void* Lock(UInt32 start, UInt32 count);

			void Unlock();

			IOFrame::Buffer::IOBuffer* LockStaticBuffer(UInt32 start, UInt32 count);

			void UnlockStaticBuffer();

			IOFrame::Buffer::IOBuffer* LockDynamicBuffer();

			void UnlockDynamicBuffer();

			virtual UInt32 GetBindFlags() = 0;

		protected:
			void Initialize() override;

			Container::SharedPtr<IOFrame::Buffer::IOBuffer> buffer_;

			UInt32 gpuBufferSize_{ 0u };
		};
	}
}
