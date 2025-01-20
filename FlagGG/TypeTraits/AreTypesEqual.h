#pragma once

namespace FlagGG
{

/** Tests whether two typenames refer to the same type. */
template<typename A,typename B>
struct TAreTypesEqual;

template<typename,typename>
struct TAreTypesEqual
{
	enum { value = false };
};

template<typename A>
struct TAreTypesEqual<A,A>
{
	enum { value = true };
};

#define ARE_TYPES_EQUAL(A,B) TAreTypesEqual<A,B>::value

}
