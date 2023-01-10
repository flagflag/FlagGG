#include "GfxBuffer.h"

namespace FlagGG
{

GfxBuffer::GfxBuffer()
	: GfxObject()
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
	gfxBufferDesc_.bindFlags_ = bindFlags;
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

}
