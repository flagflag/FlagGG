#include "GfxBuffer.h"

namespace FlagGG
{

GfxBufferReadbackDataStream::GfxBufferReadbackDataStream()
{

}

GfxBufferReadbackDataStream::~GfxBufferReadbackDataStream()
{

}

GfxBuffer::GfxBuffer()
	: GfxObject()
	, gfxBufferDesc_{}
{
}

GfxBuffer::~GfxBuffer()
{

}

void GfxBuffer::SetStride(UInt16 stride)
{
	gfxBufferDesc_.stride_ = stride;
}

void GfxBuffer::SetSize(UInt32 size)
{
	gfxBufferDesc_.size_ = size;
}

void GfxBuffer::SetBind(BufferBindFlags bindFlags)
{
	gfxBufferDesc_.bindFlags_ = bindFlags;
}

void GfxBuffer::SetAccess(BufferAccessFlags bindFlags)
{
	gfxBufferDesc_.accessFlags_ = bindFlags;
}

void GfxBuffer::SetUsage(BufferUsage usage)
{
	gfxBufferDesc_.usage_ = usage;
}

void GfxBuffer::Apply(const void* initialDataPtr)
{
	
}

void GfxBuffer::UpdateBuffer(const void* dataPtr)
{

}

void GfxBuffer::UpdateBufferRange(const void* dataPtr, UInt32 offset, UInt32 size)
{

}

void* GfxBuffer::BeginWrite(UInt32 offset, UInt32 size)
{
	return nullptr;
}

void GfxBuffer::EndWrite(UInt32 bytesWritten)
{

}

void GfxBuffer::CopyData(GfxBuffer* srcBuffer, UInt32 srcOffset, UInt32 destOffset, UInt32 copySize)
{

}

bool GfxBuffer::ReadBack(void* dataPtr)
{
	return false;
}

bool GfxBuffer::ReadBackSubResigon(void* dataPtr, UInt32 offset, UInt32 size)
{
	return false;
}

SharedPtr<GfxBufferReadbackDataStream> GfxBuffer::ReadBackToStream()
{
	return nullptr;
}

SharedPtr<GfxBufferReadbackDataStream> GfxBuffer::ReadbackToStream(UInt32 offset, UInt32 size)
{
	return nullptr;
}

const UInt8* GfxBuffer::GetShadowData() const
{
	return nullptr;
}

}
