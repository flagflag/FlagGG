#include "SphericalHarmonicsL1.h"
#include "Math/SphericalHarmonicsL2.h"

namespace FlagGG
{

void SphericalHarmonicsL1::SetZero()
{
    memset(sh, 0, sizeof(sh));
}

void SphericalHarmonicsL1::AddWeighted(const SphericalHarmonicsL1& src, float weight)
{
    for (int i = 0; i < SphericalHarmonicsL1::kFloatCount; i++)
    {
        sh[i] += src.sh[i] * weight;
    }
}

void SphericalHarmonicsL1::Shrink(const SphericalHarmonicsL2& src)
{
    // Copy DC component.
    for (int c = 0; c < SphericalHarmonics::kColorChannelCount; c++)
        GetCoefficient((SphericalHarmonics::ColorChannel)c, 0) = src.GetCoefficient((SphericalHarmonics::ColorChannel)c, 0);

    // Swizzle Directional components.
    for (int c = 0; c < SphericalHarmonics::kColorChannelCount; c++)
        GetCoefficient((SphericalHarmonics::ColorChannel)c, 2) = src.GetCoefficient((SphericalHarmonics::ColorChannel)c, 1);

    for (int c = 0; c < SphericalHarmonics::kColorChannelCount; c++)
        GetCoefficient((SphericalHarmonics::ColorChannel)c, 3) = src.GetCoefficient((SphericalHarmonics::ColorChannel)c, 2);

    for (int c = 0; c < SphericalHarmonics::kColorChannelCount; c++)
        GetCoefficient((SphericalHarmonics::ColorChannel)c, 1) = src.GetCoefficient((SphericalHarmonics::ColorChannel)c, 3);
}

}
