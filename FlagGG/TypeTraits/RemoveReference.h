#pragma once

#include "Core/BaseTypes.h"

namespace FlagGG
{

/**
 * TRemoveReference<type> will remove any references from a type.
 */
template <typename T> struct TRemoveReference      { typedef T type; };
template <typename T> struct TRemoveReference<T& > { typedef T type; };
template <typename T> struct TRemoveReference<T&&> { typedef T type; };

}
