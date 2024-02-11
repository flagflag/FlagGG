#pragma once

#include "Math/SphericalHarmonics.h"
#include "Math/Math.h"

namespace FlagGG
{

struct SphericalHarmonicsL2;

struct FlagGG_API SphericalHarmonicsL1
{
    enum
    {
        kCoeffCount = 4,
        kFloatCount = kCoeffCount * SphericalHarmonics::kColorChannelCount
    };

    // Notation:
    // http://www.ppsloan.org/publications/StupidSH36.pdf
    // http://graphics.stanford.edu/papers/envmap/envmap.pdf
    //
    //            [L00:  DC]
    // [L1-1:  y] [L10:   z] [L11:   x]
    //
    // 4 coefficients for R, G and B ordered:
    // {  L00,  L11, L1-1,  L10,  // red   channel
    //    L00,  L11, L1-1,  L10,  // blue  channel
    //    L00,  L11, L1-1,  L10 } // green channel
    //
    // Note: this order matches Enlighten and takes into consideration
    // that Enlighten has Y and Z axes swapped relatively to Unity
    // and to the papers mentioned above.
    //
    // Note2: this order is different than in L2.
    float           sh[kFloatCount];

    void            SetZero();
    void            AddWeighted(const SphericalHarmonicsL1& src, float weight);

    // Overwrites this L1 probe with the shrunk SphericalHarmonicsL2 probe (Tries to match L2 probe as close as it can)
    void            Shrink(const SphericalHarmonicsL2& src);

    float* GetPtr() { return sh; }
    const float* GetPtr() const { return sh; }

    float& GetCoefficient(SphericalHarmonics::ColorChannel colorChannel, int index) { return sh[kCoeffCount * colorChannel + index]; }
    const float& GetCoefficient(SphericalHarmonics::ColorChannel colorChannel, int index) const { return sh[kCoeffCount * colorChannel + index]; }
};

}
