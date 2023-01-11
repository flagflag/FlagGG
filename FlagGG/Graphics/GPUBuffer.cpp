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
	UInt32 lockStart = start * gfxBuffer_->GetDesc().stride_;
	lockSize_ = count * gfxBuffer_->GetDesc().stride_;
	return gfxBuffer_->BeginWrite(lockStart, lockSize_);
}

void GPUBuffer::Unlock()
{
	gfxBuffer_->EndWrite(lockSize_);
	lockSize_ = 0;
}

IOFrame::Buffer::IOBuffer* GPUBuffer::LockStaticBuffer(UInt32 start, UInt32 count)
{
	void* data = Lock(start, count);
	buffer_ = new IOFrame::Buffer::StringBuffer(data, count * gfxBuffer_->GetDesc().stride_);
	return buffer_;
}

void GPUBuffer::UnlockStaticBuffer()
{
	buffer_.Reset();
	Unlock();
}

}
