#include "GfxBufferMetal.h"
#include "GfxDeviceMetal.h"

namespace FlagGG
{

GfxBufferMetal::GfxBufferMetal()
	: lastWriteBeginOffset_(0)
{

}

GfxBufferMetal::~GfxBufferMetal()
{

}

void GfxBufferMetal::Apply(const void* initialDataPtr)
{
	UInt32 options = (UInt32)mtlpp::ResourceOptions::StorageModeShared;
	
	// 只写不读
	// https://developer.apple.com/documentation/metal/mtlcpucachemode
	// A write-combined CPU cache mode that is optimized for resources that the CPU writes into, but never reads.
	if ((gfxBufferDesc_.accessFlags_ & BUFFER_ACCESS_WRITE) && !(gfxBufferDesc_.accessFlags_ & BUFFER_ACCESS_READ))
		options |= (UInt32)mtlpp::ResourceOptions::CpuCacheModeWriteCombined;
	else
		options |= (UInt32)mtlpp::ResourceOptions::CpuCacheModeDefaultCache;

	if (initialDataPtr)
		mtlBuffer_ = GetSubsystem<GfxDeviceMetal>()->GetMetalDevice().NewBuffer(initialDataPtr, gfxBufferDesc_.size_, (mtlpp::ResourceOptions)options);
	else
		mtlBuffer_ = GetSubsystem<GfxDeviceMetal>()->GetMetalDevice().NewBuffer(gfxBufferDesc_.size_, (mtlpp::ResourceOptions)options);
}

void GfxBufferMetal::UpdateBuffer(const void* dataPtr)
{
	memcpy(mtlBuffer_.GetContents(), dataPtr, gfxBufferDesc_.size_);
	mtlBuffer_.DidModify(ns::Range(0, gfxBufferDesc_.size_));
}

void GfxBufferMetal::UpdateBufferRange(const void* dataPtr, UInt32 offset, UInt32 size)
{
	memcpy((char*)mtlBuffer_.GetContents() + offset, dataPtr, size);
	mtlBuffer_.DidModify(ns::Range(offset, size));
}

void* GfxBufferMetal::BeginWrite(UInt32 offset, UInt32 size)
{
	lastWriteBeginOffset_ = offset;
	return (char*)mtlBuffer_.GetContents() + offset;
}

void GfxBufferMetal::EndWrite(UInt32 bytesWritten)
{
	mtlBuffer_.DidModify(ns::Range(lastWriteBeginOffset_, bytesWritten));
}

}
