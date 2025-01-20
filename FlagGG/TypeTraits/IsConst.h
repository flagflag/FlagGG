#pragma once

namespace FlagGG
{

/**
 * Traits class which tests if a type is const.
 */
template <typename T>
struct TIsConst
{
	static constexpr bool value = false;
};

template <typename T>
struct TIsConst<const T>
{
	static constexpr bool value = true;
};

}
