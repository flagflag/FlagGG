#include "Graphics/IndexBuffer.h"
#include "Graphics/RenderEngine.h"
#include "Log.h"

namespace FlagGG
{

UInt32 IndexBuffer::GetBindFlags()
{
	return D3D11_BIND_INDEX_BUFFER;
}

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

}
