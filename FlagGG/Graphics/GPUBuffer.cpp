#include "GPUBuffer.h"
#include "Graphics/RenderEngine.h"
#include "IOFrame/Buffer/StringBuffer.h"
#include "GfxDevice/GfxDevice.h"
#include "Log.h"

namespace FlagGG
{

GPUBuffer::GPUBuffer()
{
	gfxBuffer_ = GfxDevice::GetDevice()->CreateBuffer();
}

GPUBuffer::~GPUBuffer()
{

}

void* GPUBuffer::Lock(UInt32 start, UInt32 count)
{
	gfxBuffer_->BeginWrite(start, count);
}

void GPUBuffer::Unlock()
{
	gfxBuffer_->EndWrite(0);
}

IOFrame::Buffer::IOBuffer* GPUBuffer::LockStaticBuffer(UInt32 start, UInt32 count)
{
	void* data = Lock(start, count);
	buffer_ = new IOFrame::Buffer::StringBuffer(data, count);
	return buffer_;
}

void GPUBuffer::UnlockStaticBuffer()
{
	buffer_.Reset();
	Unlock();
}

}
