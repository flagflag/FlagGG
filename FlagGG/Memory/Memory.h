//
// 内存系统
// 抽象成一个系统实例，如果有多实例的需求可以自行创建一个Memory实例
//

#pragma once

#include "Core/Subsystem.h"
#include "Memory/Malloc.h"

#include <malloc.h>

namespace FlagGG
{

class FlagGG_API Memory : public Subsystem<Memory>, public IMalloc
{
public:
	Memory();

	~Memory();

	// 系统内存分配
	static FORCEINLINE void* SystemMalloc(USize size)
	{
		return ::malloc(size);
	}

	// 系统内存重分配
	static FORCEINLINE void* SystemRealloc(void* originPtr, USize newSize)
	{
		return ::realloc(originPtr, newSize);
	}

	// 系统内存释放
	static FORCEINLINE void SystemFree(void* ptr)
	{
		::free(ptr);
	}

	// 分配内存
	void* Malloc(USize size, UInt32 alignment = DEFAULT_ALIGNMENT) override;

	// 重分配内存
	void* Realloc(void* originPtr, USize newSize, UInt32 alignment = DEFAULT_ALIGNMENT) override;

	// 释放内存
	void Free(void* ptr) override;

	// 获取指针的内存大小（如果不支持则返回false）
	bool GetAllocationSize(void* ptr, USize& outSize) override;

	// 释放内存缓存
	void Trim(bool trimThreadCache) override;

	// 获取内存分配器实例
	IMalloc* GetMalloc() { return malloc_; }
	
private:
	IMalloc* malloc_;
};

}

