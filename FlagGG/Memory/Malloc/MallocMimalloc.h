//
// mimalloc内存分配器
//

#pragma once

#include "Memory/Malloc.h"

namespace FlagGG
{

class MallocMimalloc : public IMalloc
{
public:
	// 分配内存
	void* Malloc(USize size, UInt32 alignment) override;

	// 重分配内存
	void* Realloc(void* originPtr, USize newSize, UInt32 alignment) override;

	// 释放内存
	void Free(void* ptr) override;

	// 获取指针的内存大小（如果不支持则返回false）
	bool GetAllocationSize(void* ptr, USize& outSize) override;

	// 释放内存缓存
	void Trim(bool trimThreadCache) override;
};

}
