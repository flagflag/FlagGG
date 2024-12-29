//
// Malloc stomp
//

#pragma once

#include "Memory/Malloc.h"

namespace FlagGG
{

class FlagGG_API MallocStomp : public IMalloc
{
public:
	explicit MallocStomp(bool bUseUnderrunMode = false);

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

	// 内部是否线程安全
	bool IsInternallyThreadSafe() const override
	{
		return false;
	}

private:
	void* TryMalloc(USize size, UInt32 alignment);

	void* TryRealloc(void* originPtr, USize newSize, UInt32 alignment);

#if PLATFORM_64BITS
	/** Expected value to be found in the sentinel. */
	static constexpr USize sentinelExpectedValue_ = 0xdeadbeefdeadbeef;
#else
	/** Expected value to be found in the sentinel. */
	static constexpr USize sentinelExpectedValue_ = 0xdeadbeef;
#endif

	// 系统虚拟内存页大小
	const USize pageSize_;

	struct AllocationData
	{
		/** Pointer to the full allocation. Needed so the OS knows what to free. */
		void* fullAllocationPointer_;
		/** Full size of the allocation including the extra page. */
		USize fullSize_;
		/** Size of the allocation requested. */
		USize size_;
		/** Sentinel used to check for underrun. */
		USize sentinel_;
	};

	// false => 可以检测内存后面区域越界
	// true  => 可以检测内存前面区域越界
	const bool bUseUnderrunMode_;

	UPtrInt virtualAddressCursor_ = 0;
	USize virtualAddressMax_ = 0;
	static constexpr USize virtualAddressBlockSize_ = 1 * 1024 * 1024 * 1024; // 1 GB blocks
};

}
