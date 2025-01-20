#pragma once

#include "Core/BaseTypes.h"

namespace FlagGG
{

/**
 * Traits class which tests if a type is arithmetic.
 */
template <typename T>
struct TIsArithmetic
{ 
	enum { value = false };
};

template <> struct TIsArithmetic<float>       { enum { value = true }; };
template <> struct TIsArithmetic<double>      { enum { value = true }; };
template <> struct TIsArithmetic<long double> { enum { value = true }; };
template <> struct TIsArithmetic<UInt8>       { enum { value = true }; };
template <> struct TIsArithmetic<UInt16>      { enum { value = true }; };
template <> struct TIsArithmetic<UInt32>      { enum { value = true }; };
template <> struct TIsArithmetic<UInt64>      { enum { value = true }; };
template <> struct TIsArithmetic<Int8>        { enum { value = true }; };
template <> struct TIsArithmetic<Int16>       { enum { value = true }; };
template <> struct TIsArithmetic<Int32>       { enum { value = true }; };
template <> struct TIsArithmetic<Int64>       { enum { value = true }; };
template <> struct TIsArithmetic<long>        { enum { value = true }; };
template <> struct TIsArithmetic<unsigned long> { enum { value = true }; };
template <> struct TIsArithmetic<bool>        { enum { value = true }; };
template <> struct TIsArithmetic<GenericPlatformTypes::widechar>    { enum { value = true }; };
template <> struct TIsArithmetic<GenericPlatformTypes::ansichar>    { enum { value = true }; };

template <typename T> struct TIsArithmetic<const          T> { enum { value = TIsArithmetic<T>::value }; };
template <typename T> struct TIsArithmetic<      volatile T> { enum { value = TIsArithmetic<T>::value }; };
template <typename T> struct TIsArithmetic<const volatile T> { enum { value = TIsArithmetic<T>::value }; };

}
