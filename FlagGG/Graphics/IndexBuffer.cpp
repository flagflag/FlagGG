#include "Graphics/IndexBuffer.h"
#include "Graphics/RenderEngine.h"
#include "Log.h"

namespace FlagGG
{
	namespace Graphics
	{
		uint32_t IndexBuffer::GetBindFlags()
		{
			return D3D11_BIND_INDEX_BUFFER;
		}

		bool IndexBuffer::SetSize(uint32_t indexSize, uint32_t indexCount)
		{
			indexSize_ = indexSize;
			indexCount_ = indexCount;

			return GPUBuffer::SetSize(indexSize_ * indexCount_);
		}

		uint32_t IndexBuffer::GetIndexSize() const
		{
			return indexSize_;
		}

		uint32_t IndexBuffer::GetIndexCount() const
		{
			return indexCount_;
		}
	}
}
