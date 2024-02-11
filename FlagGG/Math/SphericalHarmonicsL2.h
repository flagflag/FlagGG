#pragma once

#include "Math/SphericalHarmonics.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Container/fixed_array.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{

class Color;
class Vector3;
struct SphericalHarmonicsL1;

static FORCEINLINE void vstore1f(float* p, const float& v)
{
    p[0] = v;
}

static FORCEINLINE void vstore4f(float* p, const Vector4& v)
{
	p[0] = v.x_;
	p[1] = v.y_;
	p[2] = v.z_;
	p[3] = v.w_;
}

struct FlagGG_API SphericalHarmonicsL2
{
    enum
    {
        kCoeffCount = 9,
        kCoeffCountSIMD = 3,
        kFloatCount = kCoeffCount * SphericalHarmonics::kColorChannelCount,
        kVec4Count = 7  // 4 * float * 7 = 28. SHL2 needs 27
    };

    alignas(16) static const float kNormalizationConstants[kCoeffCount];
    typedef fixed_array<Vector4, kVec4Count> Vec4Coeffs;
    static const Vec4Coeffs s_BlackCoeffs;
    static const SphericalHarmonicsL2 BLACK;

    // Notation:
    // http://www.ppsloan.org/publications/StupidSH36.pdf
    // http://graphics.stanford.edu/papers/envmap/envmap.pdf
    //
    //                       [L00:  DC]
    //            [L1-1:  y] [L10:   z] [L11:   x]
    // [L2-2: xy] [L2-1: yz] [L20:  zz] [L21:  xz]  [L22:  xx - yy]
    //
    // 9 coefficients for R, G and B ordered:
    // {  L00, L1-1,  L10,  L11, L2-2, L2-1,  L20,  L21,  L22,  // red   channel
    //    L00, L1-1,  L10,  L11, L2-2, L2-1,  L20,  L21,  L22,  // blue  channel
    //    L00, L1-1,  L10,  L11, L2-2, L2-1,  L20,  L21,  L22 } // green channel
    float               sh[kFloatCount];

    SphericalHarmonicsL2() noexcept :
        sh{}
    {
    }
        
    bool                operator==(SphericalHarmonicsL2 const& other) const { return memcmp(this, &other, sizeof(SphericalHarmonicsL2)) == 0; }
    bool                operator!=(SphericalHarmonicsL2 const& other) const { return memcmp(this, &other, sizeof(SphericalHarmonicsL2)) != 0; }

    SphericalHarmonicsL2& operator+=(const SphericalHarmonicsL2& other);

    void                SetZero();
    void                AddWeighted(const SphericalHarmonicsL2& src, float weight);
    void                AddWeighted(const SphericalHarmonicsL1& src, float weight);

    // Overwrites this L2 probe with the expanded SphericalHarmonicsL1 probe (Tries to match L1 probe as close as it can)
    void                Expand(const SphericalHarmonicsL1& src);

    float* GetPtr() { return sh; }
    const float* GetPtr() const { return sh; }
    float* GetPtr(SphericalHarmonics::ColorChannel colorChannel);

    inline float& GetCoefficient(SphericalHarmonics::ColorChannel colorChannel, int index) { return sh[kCoeffCount * colorChannel + index]; }
    inline const float& GetCoefficient(SphericalHarmonics::ColorChannel colorChannel, int index) const { return sh[kCoeffCount * colorChannel + index]; }

    inline void         AddToCoefficients(const Vector4* evaluatedDirection, const float* rgb)
    {
        for (size_t i = 0; i < SphericalHarmonics::kColorChannelCount; i++)
        {
            float* ptr = &GetCoefficient((SphericalHarmonics::ColorChannel)i, 0);
            vstore4f(ptr, Vector4(ptr) + (evaluatedDirection[0] * rgb[i]));
            vstore4f(ptr + 4, Vector4(ptr + 4) + (evaluatedDirection[1] * rgb[i]));
            vstore1f(ptr + 8, float(*(ptr + 8)) + (evaluatedDirection[2].x_ * rgb[i]));
        }
    }

    void                AddAmbientLight(const Color& ambient);

    static void         GetShaderConstantsFromNormalizedSH(const SphericalHarmonicsL2& probe, Vec4Coeffs& outVecs);
    static void         GetShaderConstantsFromNormalizedSH(const SphericalHarmonicsL2& probe, Vector4(&outCoefficients)[kVec4Count]);
};

// Returns true if the distance between f0 and f1 is smaller than epsilon
inline bool CompareApproximately(float f0, float f1, float epsilon = 0.000001F)
{
	float dist = (f0 - f1);
	dist = Abs(dist);
	return dist <= epsilon;
}

inline bool CompareApproximately(const SphericalHarmonicsL2& inV0, const SphericalHarmonicsL2& inV1, const float inMaxDist = SphericalHarmonics::kEpsilon)
{
    for (int i = 0; i < SphericalHarmonicsL2::kFloatCount; i++)
    {
        if (!CompareApproximately(inV0.sh[i], inV1.sh[i], inMaxDist))
            return false;
    }
    return true;
}

}
