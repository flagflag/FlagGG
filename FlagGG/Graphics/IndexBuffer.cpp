#include "Graphics/IndexBuffer.h"
#include "Graphics/RenderEngine.h"
#include "Log.h"

namespace FlagGG
{

bool IndexBuffer::SetSize(UInt32 indexSize, UInt32 indexCount, bool dynamic)
{
	indexSize_ = indexSize;
	indexCount_ = indexCount;

	gfxBuffer_->SetStride(indexSize_);
	gfxBuffer_->SetSize(indexSize_ * indexCount);
	gfxBuffer_->SetBind(BUFFER_BIND_INDEX);
	gfxBuffer_->SetAccess(dynamic ? BUFFER_ACCESS_WRITE : BUFFER_ACCESS_NONE);
	gfxBuffer_->SetUsage(dynamic ? BUFFER_USAGE_DYNAMIC : BUFFER_USAGE_STATIC);
	gfxBuffer_->Apply(nullptr);

	return true;
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
