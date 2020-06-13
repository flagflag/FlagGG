#ifndef _TORQUE_PLATFORM_PLATFORMINTRINSICS_VISUALC_H_
#define _TORQUE_PLATFORM_PLATFORMINTRINSICS_VISUALC_H_

#include <intrin.h>


inline void dFetchAndAdd( volatile UInt32& ref, UInt32 val )
{  
   _InterlockedExchangeAdd( ( volatile long* ) &ref, val );
}
inline void dFetchAndAdd( volatile Int32& ref, Int32 val )
{
   _InterlockedExchangeAdd( ( volatile long* ) &ref, val );
}

inline bool dCompareAndSwap( volatile UInt32& ref, UInt32 oldVal, UInt32 newVal )
{
   return ( _InterlockedCompareExchange( ( volatile long* ) &ref, newVal, oldVal ) == oldVal );
}
inline bool dCompareAndSwap( volatile UInt64& ref, UInt64 oldVal, UInt64 newVal )
{
   return ( _InterlockedCompareExchange64( ( volatile __int64* ) &ref, newVal, oldVal ) == oldVal );
}

inline UInt32 dAtomicRead( volatile UInt32 &ref )
{
   return _InterlockedExchangeAdd( ( volatile long* )&ref, 0 );
}

#endif // _TORQUE_PLATFORM_PLATFORMINTRINSICS_VISUALC_H_
