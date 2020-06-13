#ifndef _PLATFORMINTRINSICS_H_
#define _PLATFORMINTRINSICS_H_

#include "Core/BaseTypes.h"

#if defined( WIN32 )
#  include "Intrinsics.Visualc.h"
#elif defined ( __ANDROID__ )
#  include "Intrinsics.Android.h"
#else
#  include "Intrinsics.gcc.h"
#endif

template< typename T >
inline bool dCompareAndSwap( T* volatile& refPtr, T* oldPtr, T* newPtr )
{
	return dCompareAndSwap( *reinterpret_cast< volatile uintptr_t* >( &refPtr ), ( uintptr_t ) oldPtr, ( uintptr_t ) newPtr );
}

inline bool dTestAndSet( volatile UInt32& ref )
{
	return dCompareAndSwap( ref, 0, 1 );
}

inline bool dTestAndSet( volatile UInt64& ref )
{
	return dCompareAndSwap( ref, 0, 1 );
}

#endif // _PLATFORMINTRINSICS_H_
