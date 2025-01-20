#pragma once

#include "Core/BaseTypes.h"

namespace FlagGG
{

/**
 * Type trait which yields a signed integer type of a given number of bytes.
 * If there is no such type, the Type member type will be absent, allowing it to be used in SFINAE contexts.
 */
template <int NumBytes>
struct TSignedIntType
{
};

template <> struct TSignedIntType<1> { using type = Int8; };
template <> struct TSignedIntType<2> { using type = Int16; };
template <> struct TSignedIntType<4> { using type = Int32; };
template <> struct TSignedIntType<8> { using type = Int64; };

/**
 * Helper for TSignedIntType which expands out to the nested Type.
 */
template <int NumBytes>
using TSignedIntType_T = typename TSignedIntType<NumBytes>::type;


/**
 * Type trait which yields an unsigned integer type of a given number of bytes.
 * If there is no such type, the Type member type will be absent, allowing it to be used in SFINAE contexts.
 */
template <int NumBytes>
struct TUnsignedIntType
{
};

template <> struct TUnsignedIntType<1> { using type = UInt8; };
template <> struct TUnsignedIntType<2> { using type = UInt16; };
template <> struct TUnsignedIntType<4> { using type = UInt32; };
template <> struct TUnsignedIntType<8> { using type = UInt64; };

/**
 * Helper for TUnsignedIntType which expands out to the nested Type.
 */
template <int NumBytes>
using TUnsignedIntType_T = typename TUnsignedIntType<NumBytes>::type;

}
