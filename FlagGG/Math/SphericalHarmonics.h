#pragma once

// Adds ability to use increment operators with enum type T.
// Enum must have consecutive values!
#define ENUM_INCREMENT(T) \
    inline T& operator++(T& flags) { flags = static_cast<T>(static_cast<int>(flags) + 1); return flags; } \
    inline T operator++(T& flags, int) { T result = flags; ++flags; return result; }

namespace FlagGG
{

namespace SphericalHarmonics
{

enum ColorChannel
{
	kColorChannelRed = 0,
	kColorChannelGreen = 1,
	kColorChannelBlue = 2,
	kColorChannelCount
};

ENUM_INCREMENT(ColorChannel);

const float kEpsilon = 0.00001f;

}

}
