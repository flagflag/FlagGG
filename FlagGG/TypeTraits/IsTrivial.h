#pragma once

#include "TypeTraits/AndOrNot.h"

namespace FlagGG
{

namespace IsTriviallyDestructible_Private
{
	// We have this specialization for enums to avoid the need to have a full definition of
	// the type.
	template <typename T, bool bIsTriviallyTriviallyDestructible = __is_enum(T)>
	struct TImpl
	{
		enum { value = true };
	};

	template <typename T>
	struct TImpl<T, false>
	{
		enum { value = __has_trivial_destructor(T) };
	};
}

/**
 * Traits class which tests if a type has a trivial copy constructor.
 */
template <typename T>
struct TIsTriviallyCopyConstructible
{
	enum { value = __has_trivial_copy(T) };
};

/**
 * Traits class which tests if a type has a trivial copy assignment operator.
 */
template <typename T>
struct TIsTriviallyCopyAssignable
{
	enum { value = __has_trivial_assign(T) };
};


/**
 * Traits class which tests if a type has a trivial destructor.
 */
template <typename T>
struct TIsTriviallyDestructible
{
	enum { value = IsTriviallyDestructible_Private::TImpl<T>::value };
};

/**
 * Traits class which tests if a type is trivial.
 */
template <typename T>
struct TIsTrivial
{
	enum { value = TAnd<TIsTriviallyDestructible<T>, TIsTriviallyCopyConstructible<T>, TIsTriviallyCopyAssignable<T>>::value };
};

}
