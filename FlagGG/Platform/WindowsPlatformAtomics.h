//
// Windows平台原子操作接口
//

#pragma once

#include "Core/BaseMacro.h"
#include "Core/BaseTypes.h"
#include <intrin.h>

namespace FlagGG
{

struct FlagGG_API WindowPlatformAtomics
{
	static_assert(sizeof(Int8) == sizeof(char) && alignof(Int8) == alignof(char), "Int8 must be compatible with char");
	static_assert(sizeof(Int16) == sizeof(short) && alignof(Int16) == alignof(short), "Int16 must be compatible with short");
	static_assert(sizeof(Int32) == sizeof(long) && alignof(Int32) == alignof(long), "Int32 must be compatible with long");
	static_assert(sizeof(Int64) == sizeof(long long) && alignof(Int64) == alignof(long long), "Int64 must be compatible with long long");

	static FORCEINLINE Int8 InterlockedIncrement(volatile Int8* value)
	{
		return (Int8)_InterlockedExchangeAdd8((char*)value, 1) + 1;
	}

	static FORCEINLINE Int16 InterlockedIncrement(volatile Int16* value)
	{
		return (Int16)_InterlockedIncrement16((short*)value);
	}

	static FORCEINLINE Int32 InterlockedIncrement(volatile Int32* value)
	{
		return (Int32)::_InterlockedIncrement((long*)value);
	}

	static FORCEINLINE Int64 InterlockedIncrement(volatile Int64* value)
	{
		return (Int64)::_InterlockedIncrement64((long long*)value);
	}

	static FORCEINLINE Int8 InterlockedDecrement(volatile Int8* value)
	{
		return (Int8)::_InterlockedExchangeAdd8((char*)value, -1) - 1;
	}

	static FORCEINLINE Int16 InterlockedDecrement(volatile Int16* value)
	{
		return (Int16)::_InterlockedDecrement16((short*)value);
	}

	static FORCEINLINE Int32 InterlockedDecrement(volatile Int32* value)
	{
		return (Int32)::_InterlockedDecrement((long*)value);
	}

	static FORCEINLINE Int64 InterlockedDecrement(volatile Int64* value)
	{
		return (Int64)::_InterlockedDecrement64((long long*)value);
	}

	static FORCEINLINE Int8 InterlockedAdd(volatile Int8* value, Int8 amount)
	{
		return (Int8)::_InterlockedExchangeAdd8((char*)value, (char)amount);
	}

	static FORCEINLINE Int16 InterlockedAdd(volatile Int16* value, Int16 amount)
	{
		return (Int16)::_InterlockedExchangeAdd16((short*)value, (short)amount);
	}

	static FORCEINLINE Int32 InterlockedAdd(volatile Int32* value, Int32 amount)
	{
		return (Int32)::_InterlockedExchangeAdd((long*)value, (long)amount);
	}

	static FORCEINLINE Int64 InterlockedAdd(volatile Int64* value, Int64 amount)
	{
		return (Int64)::_InterlockedExchangeAdd64((Int64*)value, (Int64)amount);
	}

	static FORCEINLINE Int8 InterlockedExchange(volatile Int8* value, Int8 exchange)
	{
		return (Int8)::_InterlockedExchange8((char*)value, (char)exchange);
	}

	static FORCEINLINE Int16 InterlockedExchange(volatile Int16* value, Int16 exchange)
	{
		return (Int16)::_InterlockedExchange16((short*)value, (short)exchange);
	}

	static FORCEINLINE Int32 InterlockedExchange(volatile Int32* value, Int32 exchange)
	{
		return (Int32)::_InterlockedExchange((long*)value, (long)exchange);
	}

	static FORCEINLINE Int64 InterlockedExchange(volatile Int64* value, Int64 exchange)
	{
		return (Int64)::_InterlockedExchange64((long long*)value, (long long)exchange);
	}

	static FORCEINLINE void* InterlockedExchangePtr(void* volatile* dest, void* exchange)
	{
		return ::_InterlockedExchangePointer(dest, exchange);
	}

	static FORCEINLINE Int8 InterlockedCompareExchange(volatile Int8* dest, Int8 exchange, Int8 comparand)
	{
		return (Int8)::_InterlockedCompareExchange8((char*)dest, (char)exchange, (char)comparand);
	}

	static FORCEINLINE Int16 InterlockedCompareExchange(volatile Int16* dest, Int16 exchange, Int16 comparand)
	{
		return (Int16)::_InterlockedCompareExchange16((short*)dest, (short)exchange, (short)comparand);
	}

	static FORCEINLINE Int32 InterlockedCompareExchange(volatile Int32* dest, Int32 exchange, Int32 comparand)
	{
		return (Int32)::_InterlockedCompareExchange((long*)dest, (long)exchange, (long)comparand);
	}

	static FORCEINLINE Int64 InterlockedCompareExchange(volatile Int64* dest, Int64 exchange, Int64 comparand)
	{
		return (Int64)::_InterlockedCompareExchange64(dest, exchange, comparand);
	}

	static FORCEINLINE Int8 InterlockedAnd(volatile Int8* value, const Int8 andValue)
	{
		return (Int8)::_InterlockedAnd8((volatile char*)value, (char)andValue);
	}

	static FORCEINLINE Int16 InterlockedAnd(volatile Int16* value, const Int16 andValue)
	{
		return (Int16)::_InterlockedAnd16((volatile short*)value, (short)andValue);
	}

	static FORCEINLINE Int32 InterlockedAnd(volatile Int32* value, const Int32 andValue)
	{
		return (Int32)::_InterlockedAnd((volatile long*)value, (long)andValue);
	}

	static FORCEINLINE Int64 InterlockedAnd(volatile Int64* value, const Int64 andValue)
	{
		return (Int64)::_InterlockedAnd64((volatile long long*)value, (long long)andValue);
	}

	static FORCEINLINE Int8 InterlockedOr(volatile Int8* value, const Int8 orValue)
	{
		return (Int8)::_InterlockedOr8((volatile char*)value, (char)orValue);
	}

	static FORCEINLINE Int16 InterlockedOr(volatile Int16* value, const Int16 orValue)
	{
		return (Int16)::_InterlockedOr16((volatile short*)value, (short)orValue);
	}

	static FORCEINLINE Int32 InterlockedOr(volatile Int32* value, const Int32 orValue)
	{
		return (Int32)::_InterlockedOr((volatile long*)value, (long)orValue);
	}

	static FORCEINLINE Int64 InterlockedOr(volatile Int64* value, const Int64 orValue)
	{
		return (Int64)::_InterlockedOr64((volatile long long*)value, (long long)orValue);
	}

	static FORCEINLINE Int8 InterlockedXor(volatile Int8* value, const Int8 xorValue)
	{
		return (Int8)::_InterlockedXor8((volatile char*)value, (char)xorValue);
	}

	static FORCEINLINE Int16 InterlockedXor(volatile Int16* value, const Int16 xorValue)
	{
		return (Int16)::_InterlockedXor16((volatile short*)value, (short)xorValue);
	}

	static FORCEINLINE Int32 InterlockedXor(volatile Int32* value, const Int32 xorValue)
	{
		return (Int32)::_InterlockedXor((volatile long*)value, (Int32)xorValue);
	}

	static FORCEINLINE Int64 InterlockedXor(volatile Int64* value, const Int64 xorValue)
	{
		return (Int64)::_InterlockedXor64((volatile long long*)value, (long long)xorValue);
	}

	static FORCEINLINE Int8 AtomicRead(volatile const Int8* src)
	{
		return InterlockedCompareExchange((Int8*)src, 0, 0);
	}

	static FORCEINLINE Int16 AtomicRead(volatile const Int16* src)
	{
		return InterlockedCompareExchange((Int16*)src, 0, 0);
	}

	static FORCEINLINE Int32 AtomicRead(volatile const Int32* src)
	{
		return InterlockedCompareExchange((Int32*)src, 0, 0);
	}

	static FORCEINLINE Int64 AtomicRead(volatile const Int64* src)
	{
		return InterlockedCompareExchange((Int64*)src, 0, 0);
	}

	static FORCEINLINE Int8 AtomicRead_Relaxed(volatile const Int8* src)
	{
		return *src;
	}

	static FORCEINLINE Int16 AtomicRead_Relaxed(volatile const Int16* src)
	{
		return *src;
	}

	static FORCEINLINE Int32 AtomicRead_Relaxed(volatile const Int32* src)
	{
		return *src;
	}

	static FORCEINLINE Int64 AtomicRead_Relaxed(volatile const Int64* src)
	{
		return *src;
	}

	static FORCEINLINE void AtomicStore(volatile Int8* src, Int8 val)
	{
		InterlockedExchange(src, val);
	}

	static FORCEINLINE void AtomicStore(volatile Int16* src, Int16 val)
	{
		InterlockedExchange(src, val);
	}

	static FORCEINLINE void AtomicStore(volatile Int32* src, Int32 val)
	{
		InterlockedExchange(src, val);
	}

	static FORCEINLINE void AtomicStore(volatile Int64* src, Int64 val)
	{
		InterlockedExchange(src, val);
	}

	static FORCEINLINE void AtomicStore_Relaxed(volatile Int8* src, Int8 val)
	{
		*src = val;
	}

	static FORCEINLINE void AtomicStore_Relaxed(volatile Int16* src, Int16 val)
	{
		*src = val;
	}

	static FORCEINLINE void AtomicStore_Relaxed(volatile Int32* src, Int32 val)
	{
		*src = val;
	}

	static FORCEINLINE void AtomicStore_Relaxed(volatile Int64* src, Int64 val)
	{
		*src = val;
	}

	// AtomicRead64 has been deprecated, please use AtomicRead's overload instead
	static FORCEINLINE Int64 AtomicRead64(volatile const Int64* src)
	{
		return AtomicRead(src);
	}

	static FORCEINLINE void* InterlockedCompareExchangePointer(void* volatile* dest, void* exchange, void* comparand)
	{
		return ::_InterlockedCompareExchangePointer(dest, exchange, comparand);
	}
};

typedef WindowPlatformAtomics PlatformAtomics;

}
