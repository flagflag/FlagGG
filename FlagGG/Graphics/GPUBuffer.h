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
			GPUBuffer();

			bool IsValid() override;

			bool SetSize(UInt32 byteCount);

			void SetDynamic(bool dynamic);

			void* Lock(UInt32 start, UInt32 count);

			void Unlock();

			IOFrame::Buffer::IOBuffer* LockStaticBuffer(UInt32 start, UInt32 count);

			void UnlockStaticBuffer();

			IOFrame::Buffer::IOBuffer* LockDynamicBuffer();

			void UnlockDynamicBuffer();

		protected:
			virtual void Create(const bgfx::Memory* mem, bool dynamic) = 0;

			virtual void UpdateBuffer(const bgfx::Memory* mem) = 0;

			void Initialize() override;

			bool dynamic_;

			Container::SharedPtr<IOFrame::Buffer::IOBuffer> buffer_;

			UInt32 gpuBufferSize_;
			char* srcBuffer_;
		};
	}
}
