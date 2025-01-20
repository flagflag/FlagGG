#pragma once

#include "Core/BaseTypes.h"
#include "TypeTraits/AndOrNot.h"

namespace FlagGG
{

namespace IsEnumClass_Private
{
	template <typename T>
	struct TIsEnumConvertibleToInt
	{
		static char (&Resolve(int))[2];
		static char Resolve(...);

		enum { value = sizeof(Resolve(T())) - 1 };
	};
}

/**
 * Traits class which tests if a type is arithmetic.
 */
template <typename T>
struct TIsEnumClass
{ 
	enum { value = TAndValue<__is_enum(T), TNot<IsEnumClass_Private::TIsEnumConvertibleToInt<T>>>::value };
};

}
