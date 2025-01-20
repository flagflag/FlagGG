#pragma once

#include "Core/BaseTypes.h"

namespace FlagGG
{

/**
 * Traits class which tests if a type is integral.
 */
template <typename T>
struct TIsIntegral
{
	enum { value = false };
};

template <> struct TIsIntegral<         bool>      { enum { value = true }; };
template <> struct TIsIntegral<         char>      { enum { value = true }; };
template <> struct TIsIntegral<signed   char>      { enum { value = true }; };
template <> struct TIsIntegral<unsigned char>      { enum { value = true }; };
template <> struct TIsIntegral<         char16_t>  { enum { value = true }; };
template <> struct TIsIntegral<         char32_t>  { enum { value = true }; };
template <> struct TIsIntegral<         wchar_t>   { enum { value = true }; };
template <> struct TIsIntegral<         short>     { enum { value = true }; };
template <> struct TIsIntegral<unsigned short>     { enum { value = true }; };
template <> struct TIsIntegral<         int>       { enum { value = true }; };
template <> struct TIsIntegral<unsigned int>       { enum { value = true }; };
template <> struct TIsIntegral<         long>      { enum { value = true }; };
template <> struct TIsIntegral<unsigned long>      { enum { value = true }; };
template <> struct TIsIntegral<         long long> { enum { value = true }; };
template <> struct TIsIntegral<unsigned long long> { enum { value = true }; };

template <typename T> struct TIsIntegral<const          T> { enum { value = TIsIntegral<T>::value }; };
template <typename T> struct TIsIntegral<      volatile T> { enum { value = TIsIntegral<T>::value }; };
template <typename T> struct TIsIntegral<const volatile T> { enum { value = TIsIntegral<T>::value }; };

}
