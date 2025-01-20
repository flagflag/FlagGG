#pragma once

#include "Core/BaseTypes.h"

namespace FlagGG
{

/**
 * Traits class which tests if a type is a pointer to member (data member or member function).
 */
template <typename T>
struct TIsMemberPointer
{
	enum { value = false };
};

template <typename T, typename U> struct TIsMemberPointer<T U::*> { enum { value = true }; };

template <typename T> struct TIsMemberPointer<const          T> { enum { value = TIsPointer<T>::value }; };
template <typename T> struct TIsMemberPointer<      volatile T> { enum { value = TIsPointer<T>::value }; };
template <typename T> struct TIsMemberPointer<const volatile T> { enum { value = TIsPointer<T>::value }; };

}
