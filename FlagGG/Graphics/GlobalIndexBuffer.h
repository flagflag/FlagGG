//
// 动态索引buffer
// 可分配若干子区域作为不同Mesh的索引
// 通常用于优化过多的索引Buffer导致同步GPU数据耗时过大的问题
//

#pragma once

#include "Graphics/IndexBuffer.h"
#include "Allocator/AllocatorLJ.h"

namespace FlagGG
{

class GlobalDynamicIndexBuffer
{
public:
	struct Allocation
	{
		UInt32* indexData_;
		IndexBuffer* indexBuffer_;
		UInt32 offsetInCount_;
		UInt32 sizeInCount_;
	};

	GlobalDynamicIndexBuffer();

	~GlobalDynamicIndexBuffer();

	// 分配多少个索引
	Allocation Allocate(UInt32 count);

	// 将缓冲区数据提交到GPU
	void CommitToGPU();

	// 清理所有分配的Allocation
	void Clear();

private:
	// 索引buffer，所有Alloction的合并
	SharedPtr<IndexBuffer> indexBuffer_;

	UInt32 indexCount_;

	AllocatorLJInstance alloctor_;

	PODVector<Allocation> allocations_;
};

}
