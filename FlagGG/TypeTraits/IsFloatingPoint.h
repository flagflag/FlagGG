#pragma once

#include "Core/BaseTypes.h"

namespace FlagGG
{

/**
 * Traits class which tests if a type is floating point.
 */
template <typename T>
struct TIsFloatingPoint
{
	enum { value = false };
};

template <> struct TIsFloatingPoint<float>       { enum { value = true }; };
template <> struct TIsFloatingPoint<double>      { enum { value = true }; };
template <> struct TIsFloatingPoint<long double> { enum { value = true }; };

template <typename T> struct TIsFloatingPoint<const          T> { enum { value = TIsFloatingPoint<T>::value }; };
template <typename T> struct TIsFloatingPoint<      volatile T> { enum { value = TIsFloatingPoint<T>::value }; };
template <typename T> struct TIsFloatingPoint<const volatile T> { enum { value = TIsFloatingPoint<T>::value }; };

}
