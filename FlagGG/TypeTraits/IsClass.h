#pragma once

#include "Core/BaseTypes.h"

namespace FlagGG
{

/**
 * Determines if T is a struct/class type
 */
template <typename T>
struct TIsClass
{
private:
	template <typename U> static uint16 Func(int U::*);
	template <typename U> static uint8  Func(...);

public:
	enum { value = !__is_union(T) && sizeof(Func<T>(0)) - 1 };
};

}
