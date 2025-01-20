#pragma once

#include "Core/BaseTypes.h"
#include "Core/BaseMacro.h"
#include "TypeTraits/IsIntegral.h"
#include "TypeTraits/IsPointer.h"

namespace FlagGG
{

/**
 * Aligns a value to the nearest higher multiple of 'Alignment', which must be a power of two.
 *
 * @param  Val        The value to align.
 * @param  Alignment  The alignment value, must be a power of two.
 *
 * @return The value aligned up to the specified alignment.
 */
template <typename T>
FORCEINLINE constexpr T Align(T value, UInt64 alignment)
{
	static_assert(TIsIntegral<T>::value || TIsPointer<T>::value, "Align expects an integer or pointer type");

	return (T)(((UInt64)value + alignment - 1) & ~(alignment - 1));
}

/**
 * Aligns a value to the nearest lower multiple of 'Alignment', which must be a power of two.
 *
 * @param  Val        The value to align.
 * @param  Alignment  The alignment value, must be a power of two.
 *
 * @return The value aligned down to the specified alignment.
 */
template <typename T>
FORCEINLINE constexpr T AlignDown(T value, UInt64 alignment)
{
	static_assert(TIsIntegral<T>::value || TIsPointer<T>::value, "AlignDown expects an integer or pointer type");

	return (T)(((UInt64)value) & ~(alignment - 1));
}

/**
 * Checks if a pointer is aligned to the specified alignment.
 *
 * @param  Val        The value to align.
 * @param  Alignment  The alignment value, must be a power of two.
 *
 * @return true if the pointer is aligned to the specified alignment, false otherwise.
 */
template <typename T>
FORCEINLINE constexpr bool IsAligned(T value, UInt64 alignment)
{
	static_assert(TIsIntegral<T>::value || TIsPointer<T>::value, "IsAligned expects an integer or pointer type");

	return !((UInt64)value & (alignment - 1));
}

/**
 * Aligns a value to the nearest higher multiple of 'Alignment'.
 *
 * @param  Val        The value to align.
 * @param  Alignment  The alignment value, can be any arbitrary value.
 *
 * @return The value aligned up to the specified alignment.
 */
template <typename T>
FORCEINLINE constexpr T AlignArbitrary(T value, UInt64 alignment)
{
	static_assert(TIsIntegral<T>::value || TIsPointer<T>::value, "AlignArbitrary expects an integer or pointer type");

	return (T)((((UInt64)value + alignment - 1) / alignment) * alignment);
}

}
