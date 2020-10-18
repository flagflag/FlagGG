#include "Graphics/IndexBuffer.h"
#include "Graphics/RenderEngine.h"
#include "Log.h"

namespace FlagGG
{
	namespace Graphics
	{
		bool IndexBuffer::SetSize(UInt32 indexSize, UInt32 indexCount)
		{
			indexSize_ = indexSize;
			indexCount_ = indexCount;

			return GPUBuffer::SetSize(indexSize_ * indexCount_);
		}

		UInt32 IndexBuffer::GetIndexSize() const
		{
			return indexSize_;
		}

		UInt32 IndexBuffer::GetIndexCount() const
		{
			return indexCount_;
		}

		void IndexBuffer::Create(const bgfx::Memory* mem, bool dynamic)
		{
			if (dynamic)
			{
				bgfx::DynamicIndexBufferHandle handle = bgfx::createDynamicIndexBuffer(mem);
				ResetHandler(handle);
			}
			else
			{
				bgfx::IndexBufferHandle handle = bgfx::createIndexBuffer(mem);
				ResetHandler(handle);
			}
		}

		void IndexBuffer::UpdateBuffer(const bgfx::Memory* mem)
		{
			if (dynamic_)
				bgfx::update(GetSrcHandler<bgfx::DynamicIndexBufferHandle>(), 0u, mem);
		}
	}
}
