//
// 内存分配器
//

#pragma once

#include "Core/BaseTypes.h"
#include "Core/BaseMacro.h"

namespace FlagGG
{

enum
{
	// 默认内存字节对其
	DEFAULT_ALIGNMENT = 16,

	// 最小内存字节对其
	MIN_ALIGNMENT = 8,
};

class FlagGG_API UseSystemMalloc
{
public:
	// 重写new，使用系统内存分配
	void* operator new(size_t size);

	// 重写delete，使用系统内存释放
	void operator delete(void* ptr);

	// 重写new[]，使用系统内存分配
	void* operator new[](size_t size);

	// 重写delete[]，使用系统内存释放
	void operator delete[](void* ptr);
};

class FlagGG_API IMalloc : public UseSystemMalloc
{
public:
	// 分配内存
	virtual void* Malloc(USize size, UInt32 alignment = DEFAULT_ALIGNMENT) = 0;

	// 重分配内存
	virtual void* Realloc(void* originPtr, USize newSize, UInt32 alignment = DEFAULT_ALIGNMENT) = 0;

	// 释放内存
	virtual void Free(void* ptr) = 0;

	// 获取指针的内存大小（如果不支持则返回false）
	virtual bool GetAllocationSize(void* ptr, USize& outSize) = 0;

	// 释放内存缓存
	virtual void Trim(bool trimThreadCache) = 0;
};

}
