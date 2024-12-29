#include "MallocStomp.h"
#include "Memory/Memory.h"
#include "Memory/MemoryConstants.h"
#include "Math/Math.h"

#if PLATFORM_WINDOWS
#include <windows.h>
#elif PLATFORM_UNIX || PLATFORM_APPLE
#include <sys/mman.h>
#endif

#if PLATFORM_64BITS
// 64-bit ABIs on x86_64 expect a 16-byte alignment
#define STOMPALIGNMENT 16U
#else
#define STOMPALIGNMENT 0U
#endif

namespace FlagGG
{

MallocStomp::MallocStomp(bool bUseUnderrunMode)
	: pageSize_(GetMemoryConstants().pageSize_)
	, bUseUnderrunMode_(bUseUnderrunMode)
{

}

void* MallocStomp::TryMalloc(USize size, UInt32 alignment)
{
	if (size == 0U)
	{
		size = 1U;
	}

#if PLATFORM_64BITS
	// 64-bit ABIs on x86_64 expect a 16-byte alignment
	alignment = Max<UInt32>(alignment, STOMPALIGNMENT);
#endif

	const USize alignedSize = (alignment > 0U) ? ((size + alignment - 1U) & -static_cast<Int32>(alignment)) : size;
	const USize allocFullPageSize = alignedSize + sizeof(AllocationData) + (pageSize_ - 1) & ~(pageSize_ - 1U);
	const USize totalAllocationSize = allocFullPageSize + pageSize_;

#if PLATFORM_UNIX || PLATFORM_APPLE
	void* fullAllocationPointer = mmap(nullptr, totalAllocationSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
#elif PLATFORM_WINDOWS
#if MALLOC_STOMP_KEEP_VIRTUAL_MEMORY
	// Allocate virtual address space from current block using linear allocation strategy.
	// If there is not enough space, try to allocate new block from OS. Report OOM if block allocation fails.
	void* fullAllocationPointer = nullptr;
	if (virtualAddressCursor_ + totalAllocationSize <= virtualAddressMax_)
	{
		fullAllocationPointer = (void*)(virtualAddressCursor_);
	}
	else
	{
		const USize reserveSize = Max(virtualAddressBlockSize_, totalAllocationSize);

		// Reserve a new block of virtual address space that will be linearly sub-allocated
		// We intentionally don't keep track of reserved blocks, as we never need to explicitly release them.
		fullAllocationPointer = VirtualAlloc(nullptr, reserveSize, MEM_RESERVE, PAGE_NOACCESS);

		virtualAddressCursor_ = UPtrInt(fullAllocationPointer);
		virtualAddressMax_ = virtualAddressCursor_ + reserveSize;
	}

	// No atomics or locks required here, as Malloc is externally synchronized (as indicated by FMallocStomp::IsInternallyThreadSafe()).
	virtualAddressCursor_ += totalAllocationSize;
#else
	void* fullAllocationPointer = VirtualAlloc(nullptr, totalAllocationSize, MEM_RESERVE, PAGE_NOACCESS);
#endif // MALLOC_STOMP_KEEP_VIRTUAL_MEMORY
#else
#error
#endif // PLATFORM_UNIX || PLATFORM_APPLE

	if (!fullAllocationPointer)
	{
		return nullptr;
	}

	void* returnedPointer = nullptr;
	static const USize allocationDataSize = sizeof(AllocationData);

	const AllocationData allocData = { fullAllocationPointer, totalAllocationSize, alignedSize, sentinelExpectedValue_ };

	if (bUseUnderrunMode_)
	{
		const USize alignedAllocationData = (alignment > 0U) ? ((allocationDataSize + alignment - 1U) & -static_cast<Int32>(alignment)) : allocationDataSize;
		returnedPointer = reinterpret_cast<void*>(reinterpret_cast<UInt8*>(fullAllocationPointer) + pageSize_ + alignedAllocationData);
		void* allocDataPointerStart = reinterpret_cast<AllocationData*>(reinterpret_cast<UInt8*>(fullAllocationPointer) + pageSize_);

		// Page protect the first page, this will cause the exception in case the is an underrun.
#if PLATFORM_UNIX || PLATFORM_APPLE
		mprotect(fullAllocationPointer, pageSize_, PROT_NONE);
#elif PLATFORM_WINDOWS
		// Commit physical pages to the used range, leaving the first page unmapped.
		void* committedMemory = VirtualAlloc(allocDataPointerStart, allocFullPageSize, MEM_COMMIT, PAGE_READWRITE);
		if (!committedMemory)
		{
			// Failed to allocate and commit physical memory pages. 
			return nullptr;
		}
		CRY_ASSERT(committedMemory == allocDataPointerStart);
#else
#error
#endif // PLATFORM_UNIX || PLATFORM_APPLE
	} //-V773
	else
	{
		returnedPointer = reinterpret_cast<void*>(reinterpret_cast<UInt8*>(fullAllocationPointer) + allocFullPageSize - alignedSize);

		// Page protect the last page, this will cause the exception in case the is an overrun.
#if PLATFORM_UNIX || PLATFORM_APPLE
		mprotect(reinterpret_cast<void*>(reinterpret_cast<UInt8*>(fullAllocationPointer) + allocFullPageSize), pageSize_, PROT_NONE);
#elif PLATFORM_WINDOWS
		// Commit physical pages to the used range, leaving the last page unmapped.
		void* committedMemory = VirtualAlloc(fullAllocationPointer, allocFullPageSize, MEM_COMMIT, PAGE_READWRITE);
		if (!committedMemory)
		{
			// Failed to allocate and commit physical memory pages
			return nullptr;
		}
		CRY_ASSERT(committedMemory == fullAllocationPointer);
#else
#error
#endif // PLATFORM_UNIX || PLATFORM_APPLE
	} //-V773

	AllocationData* allocDataPointer = reinterpret_cast<AllocationData*>(reinterpret_cast<UInt8*>(returnedPointer) - allocationDataSize);
	*allocDataPointer = allocData;

	return returnedPointer;
}

void* MallocStomp::Malloc(USize size, UInt32 alignment)
{
	void* result = TryMalloc(size, alignment);

	if (result == nullptr)
	{
		throw "Out of memory.";
	}

	return result;
}

void* MallocStomp::TryRealloc(void* originPtr, USize newSize, UInt32 alignment)
{
	if (newSize == 0U)
	{
		Free(originPtr);
		return nullptr;
	}

	void* returnPtr = nullptr;

	if (originPtr != nullptr)
	{
		returnPtr = TryMalloc(newSize, alignment);

		if (returnPtr != nullptr)
		{
			AllocationData* allocDataPtr = reinterpret_cast<AllocationData*>(reinterpret_cast<UInt8*>(originPtr) - sizeof(AllocationData));
			Memory::Memcpy(returnPtr, originPtr, Min(allocDataPtr->size_, newSize));
			Free(originPtr);
		}
	}
	else
	{
		returnPtr = TryMalloc(newSize, alignment);
	}

	return returnPtr;
}

void* MallocStomp::Realloc(void* originPtr, USize newSize, UInt32 alignment)
{
	void* result = TryRealloc(originPtr, newSize, alignment);

	if (result == nullptr && newSize)
	{
		throw "Out of memory.";
	}

	return result;
}

void MallocStomp::Free(void* ptr)
{
	if (ptr == nullptr)
	{
		return;
	}

	AllocationData* allocDataPtr = reinterpret_cast<AllocationData*>(ptr);
	allocDataPtr--;

	// Check that our sentinel is intact.
	if (allocDataPtr->sentinel_ != sentinelExpectedValue_)
	{
		// There was a memory underrun related to this allocation.
		DEBUG_BREAK();
	}

#if PLATFORM_UNIX || PLATFORM_APPLE
	munmap(allocDataPtr->fullAllocationPointer_, allocDataPtr->fullSize_);
#elif PLATFORM_WINDOWS
#if MALLOC_STOMP_KEEP_VIRTUAL_MEMORY
	// Unmap physical memory, but keep virtual address range reserved to catch use-after-free errors.
	VirtualFree(allocDataPtr->fullAllocationPointer_, allocDataPtr->fullSize_, MEM_DECOMMIT);
#else
	VirtualFree(allocDataPtr->fullAllocationPointer_, 0, MEM_RELEASE);
#endif
#else
#error
#endif // PLATFORM_UNIX || PLATFORM_APPLE
}

bool MallocStomp::GetAllocationSize(void* ptr, USize& outSize)
{
	return false;
}

void MallocStomp::Trim(bool trimThreadCache)
{

}
}
