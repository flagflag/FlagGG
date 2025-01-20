#pragma once

#include "Core/BaseTypes.h"

namespace FlagGG
{

/**
 * Type trait which returns true if the type T is an array or a reference to an array of ArrType.
 */
template <typename T, typename ArrType>
struct TIsArrayOrRefOfType
{
	enum { value = false };
};

template <typename ArrType> struct TIsArrayOrRefOfType<               ArrType[], ArrType> { enum { value = true }; };
template <typename ArrType> struct TIsArrayOrRefOfType<const          ArrType[], ArrType> { enum { value = true }; };
template <typename ArrType> struct TIsArrayOrRefOfType<      volatile ArrType[], ArrType> { enum { value = true }; };
template <typename ArrType> struct TIsArrayOrRefOfType<const volatile ArrType[], ArrType> { enum { value = true }; };

template <typename ArrType, unsigned int N> struct TIsArrayOrRefOfType<               ArrType[N], ArrType> { enum { value = true }; };
template <typename ArrType, unsigned int N> struct TIsArrayOrRefOfType<const          ArrType[N], ArrType> { enum { value = true }; };
template <typename ArrType, unsigned int N> struct TIsArrayOrRefOfType<      volatile ArrType[N], ArrType> { enum { value = true }; };
template <typename ArrType, unsigned int N> struct TIsArrayOrRefOfType<const volatile ArrType[N], ArrType> { enum { value = true }; };

template <typename ArrType, unsigned int N> struct TIsArrayOrRefOfType<               ArrType(&)[N], ArrType> { enum { value = true }; };
template <typename ArrType, unsigned int N> struct TIsArrayOrRefOfType<const          ArrType(&)[N], ArrType> { enum { value = true }; };
template <typename ArrType, unsigned int N> struct TIsArrayOrRefOfType<      volatile ArrType(&)[N], ArrType> { enum { value = true }; };
template <typename ArrType, unsigned int N> struct TIsArrayOrRefOfType<const volatile ArrType(&)[N], ArrType> { enum { value = true }; };

}
