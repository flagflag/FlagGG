#ifndef _TORQUE_PLATFORM_PLATFORMINTRINSICS_VISUALC_H_
#define _TORQUE_PLATFORM_PLATFORMINTRINSICS_VISUALC_H_

#include "Core/BaseTypes.h"
#include <atomic>

inline void dFetchAndAdd( volatile UInt32& ref, UInt32 val )
{
	__sync_fetch_and_add((int* ) &ref, val);
}

inline void dFetchAndAdd( volatile Int32& ref, Int32 val )
{
	__sync_fetch_and_add((int* ) &ref, val);
}

// Compare-And-Swap

inline bool dCompareAndSwap( volatile UInt32& ref, UInt32 oldVal, UInt32 newVal )
{
	return __sync_bool_compare_and_swap(&ref, oldVal, newVal);
}

inline bool dCompareAndSwap( volatile UInt64& ref, UInt64 oldVal, UInt64 newVal )
{
	return __sync_bool_compare_and_swap(&ref, oldVal, newVal);
}

#if defined(__aarch64__)
inline bool dCompareAndSwap( volatile uintptr_t& ref, uintptr_t oldVal, uintptr_t newVal )
{
	return __sync_bool_compare_and_swap(&ref, oldVal, newVal);
}
#endif

/// Performs an atomic read operation.
inline uint32 dAtomicRead( volatile UInt32 &ref )
{
	return __sync_fetch_and_add((int* ) &ref, 0);
}

#endif // _TORQUE_PLATFORM_PLATFORMINTRINSICS_VISUALC_H_
