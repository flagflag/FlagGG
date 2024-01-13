#include "GlobalVertexBuffer.h"

namespace FlagGG
{

/*-----------------------------------------------------------------------------
	GlobalDynamicVertexBuffer implementation.
-----------------------------------------------------------------------------*/

GlobalDynamicVertexBuffer::GlobalDynamicVertexBuffer()
	: vertexBuffer_(new VertexBuffer())
	, offsetInBytes_(0)
{
}

GlobalDynamicVertexBuffer::~GlobalDynamicVertexBuffer()
{

}

GlobalDynamicVertexBuffer::Allocation GlobalDynamicVertexBuffer::Allocate(UInt32 sizeInBytes)
{
	Allocation allocation;
	allocation.vertexBuffer_ = vertexBuffer_;
	allocation.vertexData_ = static_cast<UInt8*>(alloctor_.Malloc(sizeInBytes));
	offsetInBytes_ += sizeInBytes;
	allocation.offsetInBytes_ = offsetInBytes_;
	return allocation;
}

void GlobalDynamicVertexBuffer::CommitToGPU()
{
	UInt32 vertexCount = (offsetInBytes_ + 5) / 4;
	// buffer不够大，或者小于buffer的一半，重新重新分配顶点buffer
	if (vertexCount > vertexBuffer_->GetVertexCount() || vertexCount * 2 < vertexBuffer_->GetVertexCount())
	{
		PODVector<VertexElement> elements;
		elements.Push(VertexElement(VE_INT, SEM_POSITION));
		vertexBuffer_->SetSize(vertexCount, elements, true);
	}

	UInt8* vertexData = (UInt8*)vertexBuffer_->Lock(0, vertexCount);
	
	for (auto& allocation : allocations_)
	{
		memcpy(vertexData + allocation.offsetInBytes_, allocation.vertexData_, allocation.sizeInBytes_);
	}

	vertexBuffer_->Unlock();
}

void GlobalDynamicVertexBuffer::Clear()
{
	for (auto& allocation : allocations_)
	{
		alloctor_.Free(allocation.vertexData_);
	}

	allocations_.Clear();

	offsetInBytes_ = 0;
}

}
