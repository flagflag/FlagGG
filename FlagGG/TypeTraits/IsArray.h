#pragma once

#include "Core/BaseTypes.h"
#include "TypeTraits/EnableIf.h"

namespace FlagGG
{

/**
 * Traits class which tests if a type is a C++ array.
 */
template <typename T>           struct TIsArray       { enum { Value = false }; };
template <typename T>           struct TIsArray<T[]>  { enum { Value = true  }; };
template <typename T, UInt32 N> struct TIsArray<T[N]> { enum { Value = true  }; };

/**
 * Traits class which tests if a type is a bounded C++ array.
 */
template <typename T>           struct TIsBoundedArray       { enum { Value = false }; };
template <typename T, UInt32 N> struct TIsBoundedArray<T[N]> { enum { Value = true  }; };

/**
 * Traits class which tests if a type is an unbounded C++ array.
 */
template <typename T> struct TIsUnboundedArray      { enum { Value = false }; };
template <typename T> struct TIsUnboundedArray<T[]> { enum { Value = true  }; };


#ifdef __clang__
template <typename T>
auto ArrayCountHelper(T& t) -> typename TEnableIf<__is_array(T), char(&)[sizeof(t) / sizeof(t[0]) + 1]>::Type;
#else
template <typename T, UInt32 N>
char(&ArrayCountHelper(const T(&)[N]))[N + 1];
#endif

}

// Number of elements in an array.
#define ARRAY_COUNT( array ) (sizeof(FlagGG::ArrayCountHelper(array)) - 1)
