//
// Clang平台原子操作接口
//

#pragma once

#include "Core/BaseMacro.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{

struct FlagGG_API ClangPlatformAtomics
{
	static FORCEINLINE Int8 InterlockedIncrement(volatile Int8* value)
	{
		return __atomic_fetch_add(value, 1, __ATOMIC_SEQ_CST) + 1;
	}

	static FORCEINLINE Int16 InterlockedIncrement(volatile Int16* value)
	{
		return __atomic_fetch_add(value, 1, __ATOMIC_SEQ_CST) + 1;
	}

	static FORCEINLINE Int32 InterlockedIncrement(volatile Int32* value)
	{
		return __atomic_fetch_add(value, 1, __ATOMIC_SEQ_CST) + 1;
	}

	static FORCEINLINE Int64 InterlockedIncrement(volatile Int64* value)
	{
		return __atomic_fetch_add(value, 1, __ATOMIC_SEQ_CST) + 1;
	}

	static FORCEINLINE Int8 InterlockedDecrement(volatile Int8* value)
	{
		return __atomic_fetch_sub(value, 1, __ATOMIC_SEQ_CST) - 1;
	}

	static FORCEINLINE Int16 InterlockedDecrement(volatile Int16* value)
	{
		return __atomic_fetch_sub(value, 1, __ATOMIC_SEQ_CST) - 1;
	}

	static FORCEINLINE Int32 InterlockedDecrement(volatile Int32* value)
	{
		return __atomic_fetch_sub(value, 1, __ATOMIC_SEQ_CST) - 1;
	}

	static FORCEINLINE Int64 InterlockedDecrement(volatile Int64* value)
	{
		return __atomic_fetch_sub(value, 1, __ATOMIC_SEQ_CST) - 1;
	}

	static FORCEINLINE Int8 InterlockedAdd(volatile Int8* value, Int8 amount)
	{
		return __atomic_fetch_add(value, amount, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int16 InterlockedAdd(volatile Int16* value, Int16 amount)
	{
		return __atomic_fetch_add(value, amount, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int32 InterlockedAdd(volatile Int32* value, Int32 amount)
	{
		return __atomic_fetch_add(value, amount, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int64 InterlockedAdd(volatile Int64* value, Int64 amount)
	{
		return __atomic_fetch_add(value, amount, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int8 InterlockedExchange(volatile Int8* value, Int8 exchange)
	{
		return __atomic_exchange_n(value, exchange, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int16 InterlockedExchange(volatile Int16* value, Int16 exchange)
	{
		return __atomic_exchange_n(value, exchange, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int32 InterlockedExchange(volatile Int32* value, Int32 exchange)
	{
		return __atomic_exchange_n(value, exchange, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int64 InterlockedExchange(volatile Int64* value, Int64 exchange)
	{
		return __atomic_exchange_n(value, exchange, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE void* InterlockedExchangePtr(void* volatile* dest, void* exchange)
	{
		return __atomic_exchange_n(dest, exchange, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int8 InterlockedCompareExchange(volatile Int8* dest, Int8 exchange, Int8 comparand)
	{
		__atomic_compare_exchange_n(dest, &comparand, exchange, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
		return comparand;
	}

	static FORCEINLINE Int16 InterlockedCompareExchange(volatile Int16* dest, Int16 exchange, Int16 comparand)
	{
		__atomic_compare_exchange_n(dest, &comparand, exchange, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
		return comparand;
	}

	static FORCEINLINE Int32 InterlockedCompareExchange(volatile Int32* dest, Int32 exchange, Int32 comparand)
	{
		__atomic_compare_exchange_n(dest, &comparand, exchange, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
		return comparand;
	}

	static FORCEINLINE Int64 InterlockedCompareExchange(volatile Int64* dest, Int64 exchange, Int64 comparand)
	{
		__atomic_compare_exchange_n(dest, &comparand, exchange, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
		return comparand;
	}

	static FORCEINLINE Int8 InterlockedAnd(volatile Int8* value, const Int8 andValue)
	{
		return __atomic_fetch_and(value, andValue, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int16 InterlockedAnd(volatile Int16* value, const Int16 andValue)
	{
		return __atomic_fetch_and(value, andValue, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int32 InterlockedAnd(volatile Int32* value, const Int32 andValue)
	{
		return __atomic_fetch_and(value, andValue, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int64 InterlockedAnd(volatile Int64* value, const Int64 andValue)
	{
		return __atomic_fetch_and(value, andValue, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int8 InterlockedOr(volatile Int8* value, const Int8 orValue)
	{
		return __atomic_fetch_or(value, orValue, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int16 InterlockedOr(volatile Int16* value, const Int16 orValue)
	{
		return __atomic_fetch_or(value, orValue, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int32 InterlockedOr(volatile Int32* value, const Int32 orValue)
	{
		return __atomic_fetch_or(value, orValue, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int64 InterlockedOr(volatile Int64* value, const Int64 orValue)
	{
		return __atomic_fetch_or(value, orValue, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int8 InterlockedXor(volatile Int8* value, const Int8 xorValue)
	{
		return __atomic_fetch_xor(value, xorValue, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int16 InterlockedXor(volatile Int16* value, const Int16 xorValue)
	{
		return __atomic_fetch_xor(value, xorValue, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int32 InterlockedXor(volatile Int32* value, const Int32 xorValue)
	{
		return __atomic_fetch_xor(value, xorValue, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int64 InterlockedXor(volatile Int64* value, const Int64 xorValue)
	{
		return __atomic_fetch_xor(value, xorValue, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int8 AtomicRead(volatile const Int8* src)
	{
		return __atomic_load_n(src, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int16 AtomicRead(volatile const Int16* src)
	{
		return __atomic_load_n(src, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int32 AtomicRead(volatile const Int32* src)
	{
		return __atomic_load_n(src, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int64 AtomicRead(volatile const Int64* src)
	{
		return __atomic_load_n(src, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE Int8 AtomicRead_Relaxed(volatile const Int8* src)
	{
		return __atomic_load_n(src, __ATOMIC_RELAXED);
	}

	static FORCEINLINE Int16 AtomicRead_Relaxed(volatile const Int16* src)
	{
		return __atomic_load_n(src, __ATOMIC_RELAXED);
	}

	static FORCEINLINE Int32 AtomicRead_Relaxed(volatile const Int32* src)
	{
		return __atomic_load_n(src, __ATOMIC_RELAXED);
	}

	static FORCEINLINE Int64 AtomicRead_Relaxed(volatile const Int64* src)
	{
		return __atomic_load_n(src, __ATOMIC_RELAXED);
	}

	static FORCEINLINE void AtomicStore(volatile Int8* src, Int8 val)
	{
		__atomic_store_n((volatile Int8*)src, val, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE void AtomicStore(volatile Int16* src, Int16 val)
	{
		__atomic_store_n((volatile Int16*)src, val, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE void AtomicStore(volatile Int32* src, Int32 val)
	{
		__atomic_store_n((volatile Int32*)src, val, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE void AtomicStore(volatile Int64* src, Int64 val)
	{
		__atomic_store_n((volatile Int64*)src, val, __ATOMIC_SEQ_CST);
	}

	static FORCEINLINE void AtomicStore_Relaxed(volatile Int8* src, Int8 val)
	{
		__atomic_store_n((volatile Int8*)src, val, __ATOMIC_RELAXED);
	}

	static FORCEINLINE void AtomicStore_Relaxed(volatile Int16* src, Int16 val)
	{
		__atomic_store_n((volatile Int16*)src, val, __ATOMIC_RELAXED);
	}

	static FORCEINLINE void AtomicStore_Relaxed(volatile Int32* src, Int32 val)
	{
		__atomic_store_n((volatile Int32*)src, val, __ATOMIC_RELAXED);
	}

	static FORCEINLINE void AtomicStore_Relaxed(volatile Int64* src, Int64 val)
	{
		__atomic_store_n((volatile Int64*)src, val, __ATOMIC_RELAXED);
	}

	static FORCEINLINE void* InterlockedCompareExchangePointer(void* volatile* dest, void* exchange, void* comparand)
	{
		__atomic_compare_exchange_n(dest, &comparand, exchange, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
		return comparand;
	}
};

typedef ClangPlatformAtomics PlatformAtomics;

}
