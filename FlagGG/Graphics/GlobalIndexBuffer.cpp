#include "GlobalIndexBuffer.h"

namespace FlagGG
{

GlobalDynamicIndexBuffer::GlobalDynamicIndexBuffer()
	: indexBuffer_(new IndexBuffer())
	, indexCount_(0)
{

}

GlobalDynamicIndexBuffer::~GlobalDynamicIndexBuffer()
{

}

GlobalDynamicIndexBuffer::Allocation GlobalDynamicIndexBuffer::Allocate(UInt32 count)
{
	Allocation allocation;
	allocation.indexBuffer_ = indexBuffer_;
	allocation.indexData_ = static_cast<UInt32*>(alloctor_.Malloc(count * sizeof(UInt32)));
	indexCount_ += count;
	allocation.offsetInCount_ = indexCount_;
	return allocation;
}

void GlobalDynamicIndexBuffer::CommitToGPU()
{
	// buffer不够大，或者小于buffer的一半，重新重新分配顶点buffer
	if (indexCount_ > indexBuffer_->GetIndexCount() || indexCount_ * 2 < indexBuffer_->GetIndexCount())
	{
		indexBuffer_->SetSize(sizeof(UInt32), indexCount_, true);
	}

	UInt32* vertexData = (UInt32*)indexBuffer_->Lock(0, indexCount_);

	for (auto& allocation : allocations_)
	{
		memcpy(vertexData + allocation.offsetInCount_, allocation.indexData_, allocation.sizeInCount_ * sizeof(UInt32));
	}

	indexBuffer_->Unlock();
}

void GlobalDynamicIndexBuffer::Clear()
{
	for (auto& allocation : allocations_)
	{
		alloctor_.Free(allocation.indexData_);
	}

	allocations_.Clear();

	indexCount_ = 0;
}

}
