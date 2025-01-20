#pragma once

#include "Core/BaseTypes.h"

namespace FlagGG
{

/**
 * Traits class which tests if a type is a pointer.
 */
template <typename T>
struct TIsPointer
{
	enum { value = false };
};

template <typename T> struct TIsPointer<T*> { enum { value = true }; };

template <typename T> struct TIsPointer<const          T> { enum { value = TIsPointer<T>::value }; };
template <typename T> struct TIsPointer<      volatile T> { enum { value = TIsPointer<T>::value }; };
template <typename T> struct TIsPointer<const volatile T> { enum { value = TIsPointer<T>::value }; };

}
