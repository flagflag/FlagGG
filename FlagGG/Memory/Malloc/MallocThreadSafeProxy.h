//
// 分配器线程安全代理
//

#pragma once

#include "Memory/Malloc.h"
#include "AsyncFrame/Mutex.h"

namespace FlagGG
{

class FlagGG_API MallocThreadSafeProxy : public IMalloc
{
public:
	explicit MallocThreadSafeProxy(IMalloc* malloc);

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

	// 内部是否线程安全
	bool IsInternallyThreadSafe() const override
	{
		return true;
	}

protected:
	IMalloc* malloc_;

	Mutex mutex_;
};

}
