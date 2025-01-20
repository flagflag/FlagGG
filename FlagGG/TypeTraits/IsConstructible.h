#pragma once

namespace FlagGG
{

/**
 * Determines if T is constructible from a set of arguments.
 */
template <typename T, typename... Args>
struct TIsConstructible
{
	enum { value = __is_constructible(T, Args...) };
};

}
