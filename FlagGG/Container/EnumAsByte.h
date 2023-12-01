// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <type_traits>

#include "Core/BaseMacro.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{

template <bool> struct EnumAsByte_EnumClass;
template <> struct EnumAsByte_EnumClass<true> {};
template <> struct EnumAsByte_EnumClass<false> {};

/**
 * Template to store enumeration values as bytes in a type-safe way.
 */
template<class TEnum>
class EnumAsByte
{
	typedef EnumAsByte_EnumClass<std::is_enum<TEnum>::value> Check;

public:
	typedef TEnum EnumType;

	EnumAsByte() = default;
	EnumAsByte(const EnumAsByte&) = default;
	EnumAsByte& operator=(const EnumAsByte&) = default;

	/**
	 * Constructor, initialize to the enum value.
	 *
	 * @param InValue value to construct with.
	 */
	FORCEINLINE EnumAsByte(TEnum inValue)
		: value_(static_cast<UInt8>(inValue))
	{ }

	/**
	 * Constructor, initialize to the Int32 value.
	 *
	 * @param InValue value to construct with.
	 */
	explicit FORCEINLINE EnumAsByte(Int32 inValue)
		: value_(static_cast<UInt8>(inValue))
	{ }

	/**
	 * Constructor, initialize to the Int32 value.
	 *
	 * @param InValue value to construct with.
	 */
	explicit FORCEINLINE EnumAsByte(UInt8 inValue)
		: value_(InValue)
	{ }

public:
	/**
	 * Compares two enumeration values for equality.
	 *
	 * @param InValue The value to compare with.
	 * @return true if the two values are equal, false otherwise.
	 */
	bool operator==(TEnum inValue) const
	{
		return static_cast<TEnum>(value_) == inValue;
	}

	/**
	 * Compares two enumeration values for equality.
	 *
	 * @param InValue The value to compare with.
	 * @return true if the two values are equal, false otherwise.
	 */
	bool operator==(EnumAsByte inValue) const
	{
		return value_ == inValue.value_;
	}

	/** Implicit conversion to TEnum. */
	operator TEnum() const
	{
		return (TEnum)value_;
	}

public:

	/**
	 * Gets the enumeration value.
	 *
	 * @return The enumeration value.
	 */
	TEnum GetValue() const
	{
		return (TEnum)value_;
	}

private:

	/** Holds the value as a byte. **/
	UInt8 value_;
};

}
