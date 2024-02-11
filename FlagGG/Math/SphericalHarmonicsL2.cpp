#include "Math/SphericalHarmonicsL2.h"
#include "Math/Color.h"
#include "Math/SphericalHarmonicsL1.h"
#include "Math/Vector3.h"
#include "Core/CryAssert.h"

namespace FlagGG
{
    // Constants from SetSHEMapConstants function in the Stupid Spherical Harmonics Tricks paper:
    // http://www.ppsloan.org/publications/StupidSH36.pdf
#define sqrtPI (Sqrt(PI))
#define fC0 (1.0f / (2.0f * sqrtPI))
#define fC1 (Sqrt ( 3.0f) / ( 3.0f * sqrtPI))
#define fC2 (Sqrt (15.0f) / ( 8.0f * sqrtPI))
#define fC3 (Sqrt ( 5.0f) / (16.0f * sqrtPI))
#define fC4 (0.5f * fC2)

alignas(16) const float SphericalHarmonicsL2::kNormalizationConstants[] = { fC0, -fC1, fC1, -fC1, fC2, -fC2, fC3, -fC2, fC4 };

const SphericalHarmonicsL2::Vec4Coeffs SphericalHarmonicsL2::s_BlackCoeffs =
{
    { Vector4::ZERO, Vector4::ZERO, Vector4::ZERO, Vector4::ZERO, Vector4::ZERO, Vector4::ZERO, Vector4(0.f, 0.f, 0.f, 1.f) }
};

const SphericalHarmonicsL2 SphericalHarmonicsL2::BLACK;

SphericalHarmonicsL2& SphericalHarmonicsL2::operator+=(const SphericalHarmonicsL2& other)
{
    for (int i = 0; i < SphericalHarmonicsL2::kFloatCount; i++)
    {
        sh[i] += other.sh[i];
    }

    return *this;
}

void SphericalHarmonicsL2::SetZero()
{
    memset(sh, 0, sizeof(sh));
}

void SphericalHarmonicsL2::AddWeighted(const SphericalHarmonicsL2& src, float weight)
{
    for (int i = 0; i < SphericalHarmonicsL2::kFloatCount; i++)
    {
        sh[i] += src.sh[i] * weight;
    }
}

void SphericalHarmonicsL2::AddWeighted(const SphericalHarmonicsL1& src, float weight)
{
    SphericalHarmonicsL2 expandedSrc;
    expandedSrc.Expand(src);
    AddWeighted(expandedSrc, weight);
}

void SphericalHarmonicsL2::Expand(const SphericalHarmonicsL1& src)
{
    // Copy DC component.
    for (int c = 0; c < SphericalHarmonics::kColorChannelCount; c++)
        GetCoefficient((SphericalHarmonics::ColorChannel)c, 0) = src.GetCoefficient((SphericalHarmonics::ColorChannel)c, 0);

    // Swizzle Directional components and scale by two. Enlighten coefficients are generated expecting A + 2*dot(B,normal)
    // but since we do not perform this multiply by 2 in the shader (in SHEvalLinearL0L1()) we do it here instead.
    for (int c = 0; c < SphericalHarmonics::kColorChannelCount; c++)
        GetCoefficient((SphericalHarmonics::ColorChannel)c, 1) = 2 * src.GetCoefficient((SphericalHarmonics::ColorChannel)c, 2);

    for (int c = 0; c < SphericalHarmonics::kColorChannelCount; c++)
        GetCoefficient((SphericalHarmonics::ColorChannel)c, 2) = 2 * src.GetCoefficient((SphericalHarmonics::ColorChannel)c, 3);

    for (int c = 0; c < SphericalHarmonics::kColorChannelCount; c++)
        GetCoefficient((SphericalHarmonics::ColorChannel)c, 3) = 2 * src.GetCoefficient((SphericalHarmonics::ColorChannel)c, 1);

    // Pad with zeros.
    const size_t size = (SphericalHarmonicsL2::kCoeffCount - SphericalHarmonicsL1::kCoeffCount) * sizeof(float);
    memset(GetPtr(SphericalHarmonics::kColorChannelRed) + SphericalHarmonicsL1::kCoeffCount, 0, size);
    memset(GetPtr(SphericalHarmonics::kColorChannelGreen) + SphericalHarmonicsL1::kCoeffCount, 0, size);
    memset(GetPtr(SphericalHarmonics::kColorChannelBlue) + SphericalHarmonicsL1::kCoeffCount, 0, size);
}

float* SphericalHarmonicsL2::GetPtr(SphericalHarmonics::ColorChannel colorChannel)
{
    return &sh[kCoeffCount * colorChannel];
}

void SphericalHarmonicsL2::AddAmbientLight(const Color& ambient)
{
    const float kNormalization = 2 * Sqrt(PI) * kNormalizationConstants[0];
    CRY_ASSERT(kNormalization == 1.0);     // The values below would be multiplied by this, but since this is a const 1.f, we don't!
    GetCoefficient(SphericalHarmonics::kColorChannelRed, 0) += ambient.r_;
    GetCoefficient(SphericalHarmonics::kColorChannelGreen, 0) += ambient.g_;
    GetCoefficient(SphericalHarmonics::kColorChannelBlue, 0) += ambient.b_;
}

// The shader and, by extension, SetSHConstants expect the values to be normalized.
// The data in the probe passed here is expected to already be normalized
// with kNormalizationConstants.
void SphericalHarmonicsL2::GetShaderConstantsFromNormalizedSH(const SphericalHarmonicsL2& probe, SphericalHarmonicsL2::Vec4Coeffs& outCoefficients)
{
    // Constant + Linear
    SphericalHarmonics::ColorChannel iC;
    for (iC = SphericalHarmonics::kColorChannelRed; iC < SphericalHarmonics::kColorChannelCount; iC++)
    {
        // In the shader we multiply the normal is not swizzled, so it's normal.xyz.
        // Swizzle the coefficients to be in { x, y, z, DC } order.
        outCoefficients[iC].x_ = probe.GetCoefficient(iC, 3);
        outCoefficients[iC].y_ = probe.GetCoefficient(iC, 1);
        outCoefficients[iC].z_ = probe.GetCoefficient(iC, 2);
        outCoefficients[iC].w_ = probe.GetCoefficient(iC, 0) - probe.GetCoefficient(iC, 6);
    }

    // Quadratic polynomials
    for (iC = SphericalHarmonics::kColorChannelRed; iC < SphericalHarmonics::kColorChannelCount; iC++)
    {
        outCoefficients[iC + 3].x_ = probe.GetCoefficient(iC, 4);
        outCoefficients[iC + 3].y_ = probe.GetCoefficient(iC, 5);
        outCoefficients[iC + 3].z_ = probe.GetCoefficient(iC, 6) * 3.0f;
        outCoefficients[iC + 3].w_ = probe.GetCoefficient(iC, 7);
    }

    // Final quadratic polynomial
    outCoefficients[6].x_ = probe.GetCoefficient(SphericalHarmonics::kColorChannelRed, 8);
    outCoefficients[6].y_ = probe.GetCoefficient(SphericalHarmonics::kColorChannelGreen, 8);
    outCoefficients[6].z_ = probe.GetCoefficient(SphericalHarmonics::kColorChannelBlue, 8);
    outCoefficients[6].w_ = 1.0f;
}

void SphericalHarmonicsL2::GetShaderConstantsFromNormalizedSH(const SphericalHarmonicsL2& probe, Vector4(&outCoefficients)[kVec4Count])
{
    // Constant + Linear
    SphericalHarmonics::ColorChannel iC;
    for (iC = SphericalHarmonics::kColorChannelRed; iC < SphericalHarmonics::kColorChannelCount; iC++)
    {
        // In the shader we multiply the normal is not swizzled, so it's normal.xyz.
        // Swizzle the coefficients to be in { x, y, z, DC } order.
        outCoefficients[iC].x_ = probe.GetCoefficient(iC, 3);
        outCoefficients[iC].y_ = probe.GetCoefficient(iC, 1);
        outCoefficients[iC].z_ = probe.GetCoefficient(iC, 2);
        outCoefficients[iC].w_ = probe.GetCoefficient(iC, 0) - probe.GetCoefficient(iC, 6);
    }

    // Quadratic polynomials
    for (iC = SphericalHarmonics::kColorChannelRed; iC < SphericalHarmonics::kColorChannelCount; iC++)
    {
        outCoefficients[iC + 3].x_ = probe.GetCoefficient(iC, 4);
        outCoefficients[iC + 3].y_ = probe.GetCoefficient(iC, 5);
        outCoefficients[iC + 3].z_ = probe.GetCoefficient(iC, 6) * 3.0f;
        outCoefficients[iC + 3].w_ = probe.GetCoefficient(iC, 7);
    }

    // Final quadratic polynomial
    outCoefficients[6].x_ = probe.GetCoefficient(SphericalHarmonics::kColorChannelRed, 8);
    outCoefficients[6].y_ = probe.GetCoefficient(SphericalHarmonics::kColorChannelGreen, 8);
    outCoefficients[6].z_ = probe.GetCoefficient(SphericalHarmonics::kColorChannelBlue, 8);
    outCoefficients[6].w_ = 1.f;
}

}
