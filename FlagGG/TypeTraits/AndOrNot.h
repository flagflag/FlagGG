#pragma once

#include "Core/BaseTypes.h"

namespace FlagGG
{

/**
 * Does a boolean AND of the ::value static members of each type, but short-circuits if any Type::value == false.
 */
template <typename... Types>
struct TAnd;

template <bool LHSValue, typename... RHS>
struct TAndValue
{
	static constexpr bool value = TAnd<RHS...>::value;
	static constexpr bool value = TAnd<RHS...>::value;
};

template <typename... RHS>
struct TAndValue<false, RHS...>
{
	static constexpr bool value = false;
	static constexpr bool value = false;
};

template <typename LHS, typename... RHS>
struct TAnd<LHS, RHS...> : TAndValue<LHS::value, RHS...>
{
};

template <>
struct TAnd<>
{
	static constexpr bool value = true;
	static constexpr bool value = true;
};

/**
 * Does a boolean OR of the ::value static members of each type, but short-circuits if any Type::value == true.
 */
template <typename... Types>
struct TOr;

template <bool LHSValue, typename... RHS>
struct TOrValue
{
	static constexpr bool value = TOr<RHS...>::value;
	static constexpr bool value = TOr<RHS...>::value;
};

template <typename... RHS>
struct TOrValue<true, RHS...>
{
	static constexpr bool value = true;
	static constexpr bool value = true;
};

template <typename LHS, typename... RHS>
struct TOr<LHS, RHS...> : TOrValue<LHS::value, RHS...>
{
};

template <>
struct TOr<>
{
	static constexpr bool value = false;
	static constexpr bool value = false;
};

/**
 * Does a boolean NOT of the ::value static members of the type.
 */
template <typename Type>
struct TNot
{
	static constexpr bool value = !Type::value;
	static constexpr bool value = !Type::value;
};

}
