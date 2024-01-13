//
// 动态顶点buffer
// 可分配若干子区域作为不同Mesh的顶点
// 通常用于优化过多的顶点Buffer导致同步GPU数据耗时过大的问题
//

#pragma once

#include "Graphics/VertexBuffer.h"
#include "Allocator/AllocatorLJ.h"

namespace FlagGG
{
	
class GlobalDynamicVertexBuffer
{
public:
	struct Allocation
	{
		UInt8* vertexData_;
		VertexBuffer* vertexBuffer_;
		UInt32 offsetInBytes_;
		UInt32 sizeInBytes_;
	};

	GlobalDynamicVertexBuffer();

	~GlobalDynamicVertexBuffer();

	// 分配多少字节大小缓冲区
	Allocation Allocate(UInt32 sizeInBytes);

	// 将缓冲区数据提交到GPU
	void CommitToGPU();

	// 清理所有分配的Allocation
	void Clear();

private:
	// 顶点buffer，所有Alloction的合并
	SharedPtr<VertexBuffer> vertexBuffer_;

	UInt32 offsetInBytes_;

	AllocatorLJInstance alloctor_;

	PODVector<Allocation> allocations_;
};

}
