//
// 索引缓冲区
//

#pragma once

#include "Graphics/GPUBuffer.h"

namespace FlagGG
{

class FlagGG_API IndexBuffer : public GPUBuffer
{
public:
	/**
	 * 设置ib的大小
	 * indexSize   - ib元素的个数
	 * indexCount  - ib元素的大小
	 * dynamic     - 是否是动态bufer
	 */
	bool SetSize(UInt32 indexSize, UInt32 indexCount, bool dynamic = false);

	// 获取ib元素大小
	UInt32 GetIndexSize() const;

	// 获取ib元素个数
	UInt32 GetIndexCount() const;

protected:

	UInt32 indexSize_{ 0 };
	UInt32 indexCount_{ 0 };
};

}

