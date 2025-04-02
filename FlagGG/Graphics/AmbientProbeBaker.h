//
// 球谐烘培
//

#pragma once

#include "Math/Color.h"
#include "Math/SphericalHarmonicsL2.h"

namespace FlagGG
{

class Image;

FlagGG_API bool BakeAmbientProbe(Image* cubemap, SphericalHarmonicsL2& outAmbientProbe);

/// 采样球谐（注意：使用Y轴朝上的坐标系）
FlagGG_API Color SampleProbe(const SphericalHarmonicsL2::Vec4Coeffs& vec4Coeffs, const Vector4& normal);

}
