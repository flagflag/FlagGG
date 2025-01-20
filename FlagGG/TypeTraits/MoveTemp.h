#pragma once

#include "Core/BaseMacro.h"
#include "TypeTraits/RemoveReference.h"
#include "TypeTraits/AreTypesEqual.h"

namespace FlagGG
{

/**
 * TIsLValueReferenceType
 */
	template<typename T> struct TIsLValueReferenceType { enum { value = false }; };
	template<typename T> struct TIsLValueReferenceType<T&> { enum { value = true }; };

/**
 * MoveTemp will cast a reference to an rvalue reference.
 * This is UE's equivalent of std::move except that it will not compile when passed an rvalue or
 * const object, because we would prefer to be informed when MoveTemp will have no effect.
 */
template <typename T>
FORCEINLINE typename TRemoveReference<T>::type&& MoveTemp(T&& Obj)
{
	typedef typename TRemoveReference<T>::type CastType;

	// Validate that we're not being passed an rvalue or a const object - the former is redundant, the latter is almost certainly a mistake
	static_assert(TIsLValueReferenceType<T>::value, "MoveTemp called on an rvalue");
	static_assert(!TAreTypesEqual<CastType&, const CastType&>::value, "MoveTemp called on a const object");

	return (CastType&&)Obj;
}

/**
 * MoveTemp will cast a reference to an rvalue reference.
 * This is UE's equivalent of std::move.  It doesn't static assert like MoveTemp, because it is useful in
 * templates or macros where it's not obvious what the argument is, but you want to take advantage of move semantics
 * where you can but not stop compilation.
 */
template <typename T>
FORCEINLINE typename TRemoveReference<T>::type&& MoveTempIfPossible(T&& Obj)
{
	typedef typename TRemoveReference<T>::type CastType;
	return (CastType&&)Obj;
}

}
