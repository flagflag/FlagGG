/******************************************************************************
 *  This file is a part of Ultralight, an ultra-portable web-browser engine.  *
 *                                                                            *
 *  See <https://ultralig.ht> for licensing and more.                         *
 *                                                                            *
 *  (C) 2023 Ultralight, Inc.                                                 *
 *****************************************************************************/
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/Geometry.h>
#include <Ultralight/private/Vector.h>

namespace ultralight {

// 32-bit RGBA, RGB is in sRGB gamma-space, Alpha is linear, not premultiplied
typedef uint32_t Color;

#define UltralightRGBA(r, g, b, a) \
    static_cast<uint32_t>( \
        (static_cast<uint8_t>(r) << 24u) | \
        (static_cast<uint8_t>(g) << 16u) | \
        (static_cast<uint8_t>(b) << 8u) | \
        (static_cast<uint8_t>(a) << 0u))

#define UltralightRGB(r, g, b) \
    static_cast<uint32_t>( \
        (static_cast<uint8_t>(r) << 24u) | \
        (static_cast<uint8_t>(g) << 16u) | \
        (static_cast<uint8_t>(b) << 8u) | \
        (255 << 0))

#define UltralightColorGetR(color)      (((color) >> 24) & 0xFF)
#define UltralightColorGetG(color)      (((color) >> 16) & 0xFF)
#define UltralightColorGetB(color)      (((color) >>  8) & 0xFF)
#define UltralightColorGetA(color)      (((color) >>  0) & 0xFF)
#define UltralightColorGetFloat4(color) { UltralightColorGetR(color) / 255.0f, \
                                          UltralightColorGetG(color) / 255.0f, \
                                          UltralightColorGetB(color) / 255.0f, \
                                          UltralightColorGetA(color) / 255.0f }
#define UltralightColorGetRaw(color) { (float)UltralightColorGetR(color), \
                                       (float)UltralightColorGetG(color), \
                                       (float)UltralightColorGetB(color), \
                                       (float)UltralightColorGetA(color) }

#define UltralightColorTRANSPARENT  0x00000000
#define UltralightColorBLACK        0x000000FF
#define UltralightColorWHITE        0xFFFFFFFF
#define UltralightColorRED          0xFF0000FF
#define UltralightColorGREEN        0x00FF00FF
#define UltralightColorBLUE         0x0000FFFF
#define UltralightColorYELLOW       0xFFFF00FF

inline Color MakeColorAlpha(float alpha) {
  return UltralightRGBA(255, 255, 255, 255 * alpha);
}

inline float SRGBToLinear(float val) {
  return (val <= 0.04045f) ? val / 12.92f : powf((val + 0.055f) / 1.055f, 2.4f);
}

inline float LinearToSRGB(float val) {
  return (val <= 0.0031308f) ? val * 12.92f : (1.055f * pow(val, 1.0f / 2.4f) - 0.055f);
}

inline float Luminance(vec4 col) {
  return 0.299f * col.x + 0.587f * col.y + 0.114f * col.z;
}

// We perform all blending in linear space on GPU so in order to simulate
// naive sRGB blending (as is common in other browsers with CSS rgba()) we
// calculate the luminance and adjust the alpha.
#define COMPENSATE_SRGB_LUMINANCE 1

inline float mix(float a, float b, float t) {
  return a * (1.0f - t) + b * t;
}

// Convert un-premultiplied sRGB color to premultiplied linear color
inline vec4 ToPremultipliedLinear(Color input, float a = 1.0f) {
  // Step 1: Convert RGB channels from sRGB to linear space.
  // Step 2: Premultiply alpha with RGB channels.
  
  vec4 col = UltralightColorGetFloat4(input);
#if COMPENSATE_SRGB_LUMINANCE
  float luma = Luminance(col);
  float alpha = col.w * a;
  col.w = mix(pow(alpha, 1.0f / 4.3f), pow(alpha, 1.7f), luma);
  col.w = mix(alpha, col.w, std::min(alpha * 2.0f, 1.0f));
#endif
  col.x = SRGBToLinear(col.x) * col.w;
  col.y = SRGBToLinear(col.y) * col.w;
  col.z = SRGBToLinear(col.z) * col.w;

  return col;
}

inline Color ToSRGB(vec4 col) {
  col.x = LinearToSRGB(col.x);
  col.y = LinearToSRGB(col.y);
  col.z = LinearToSRGB(col.z);
  col *= 255.0f;

  return UltralightRGBA(col.x, col.y, col.z, col.w);
}

inline vec4 PremultiplyVec4(Color input, float a = 1.0f) {
  vec4 col = UltralightColorGetFloat4(input);
  col.w *= a;
  col.x *= col.w;
  col.y *= col.w;
  col.z *= col.w;
  return col;
}

inline Color Premultiply(Color input, float a = 1.0f) {
  vec4 col = PremultiplyVec4(input, a);
  col *= 255.0f;
  return UltralightRGBA(col.x, col.y, col.z, col.w);
}

struct UExport GradientStop {
  float stop;
  Color color;
};

struct UExport Gradient {
  Vector<GradientStop> stops;
  Point p0;
  Point p1;
  float r0;
  float r1;
  bool is_radial;
};

struct Pattern;

enum FillRule {
  kFillRule_NonZero,
  kFillRule_EvenOdd,
};

enum LineCap {
  kLineCap_Butt,
  kLineCap_Round,
  kLineCap_Square,
};

enum LineJoin {
  kLineJoin_Miter,
  kLineJoin_Round,
  kLineJoin_Bevel,
};

enum CompositeOp {
  kCompositeOp_Clear,
  kCompositeOp_Copy,
  kCompositeOp_SourceOver,
  kCompositeOp_SourceIn,
  kCompositeOp_SourceOut,
  kCompositeOp_SourceAtop,
  kCompositeOp_DestinationOver,
  kCompositeOp_DestinationIn,
  kCompositeOp_DestinationOut,
  kCompositeOp_DestinationAtop,
  kCompositeOp_XOR,
  kCompositeOp_PlusDarker,
  kCompositeOp_PlusLighter,
  kCompositeOp_Difference
};

enum BlendMode {
  kBlendMode_Normal = 1, // Start with 1 to match SVG's blendmode enumeration.
  kBlendMode_Multiply,
  kBlendMode_Screen,
  kBlendMode_Darken,
  kBlendMode_Lighten,
  kBlendMode_Overlay,
  kBlendMode_ColorDodge,
  kBlendMode_ColorBurn,
  kBlendMode_HardLight,
  kBlendMode_SoftLight,
  kBlendMode_Difference,
  kBlendMode_Exclusion,
  kBlendMode_Hue,
  kBlendMode_Saturation,
  kBlendMode_Color,
  kBlendMode_Luminosity,
  kBlendMode_PlusDarker,
  kBlendMode_PlusLighter
};

// Combined blend op used in shader code, matches Cairo operators
enum BlendOp {
  kBlendOp_Clear = 0,
  kBlendOp_Source,
  kBlendOp_Over,
  kBlendOp_In,
  kBlendOp_Out,
  kBlendOp_Atop,
  kBlendOp_DestOver,
  kBlendOp_DestIn,
  kBlendOp_DestOut,
  kBlendOp_DestAtop,
  kBlendOp_XOR,
  kBlendOp_Darken,
  kBlendOp_Add,
  kBlendOp_Difference,
  kBlendOp_Multiply,
  kBlendOp_Screen,
  kBlendOp_Overlay,
  kBlendOp_Lighten,
  kBlendOp_ColorDodge,
  kBlendOp_ColorBurn,
  kBlendOp_HardLight,
  kBlendOp_SoftLight,
  kBlendOp_Exclusion,
  kBlendOp_Hue,
  kBlendOp_Saturation,
  kBlendOp_Color,
  kBlendOp_Luminosity
};

inline BlendOp CompositeOpToBlendOp(CompositeOp op)
{
  switch (op) {
  case kCompositeOp_Clear:
    return kBlendOp_Clear;
  case kCompositeOp_Copy:
    return kBlendOp_Source;
  case kCompositeOp_SourceOver:
    return kBlendOp_Over;
  case kCompositeOp_SourceIn:
    return kBlendOp_In;
  case kCompositeOp_SourceOut:
    return kBlendOp_Out;
  case kCompositeOp_SourceAtop:
    return kBlendOp_Atop;
  case kCompositeOp_DestinationOver:
    return kBlendOp_DestOver;
  case kCompositeOp_DestinationIn:
    return kBlendOp_DestIn;
  case kCompositeOp_DestinationOut:
    return kBlendOp_DestOut;
  case kCompositeOp_DestinationAtop:
    return kBlendOp_DestAtop;
  case kCompositeOp_XOR:
    return kBlendOp_XOR;
  case kCompositeOp_PlusDarker:
    return kBlendOp_Darken;
  case kCompositeOp_PlusLighter:
    return kBlendOp_Add;
  case kCompositeOp_Difference:
    return kBlendOp_Difference;
  default:
    return kBlendOp_Source;
  }
}

inline BlendOp ToBlendOp(CompositeOp op, BlendMode mode)
{
  switch (mode) {
  case kBlendMode_Normal:
    return CompositeOpToBlendOp(op);
  case kBlendMode_Multiply:
    return kBlendOp_Multiply;
  case kBlendMode_Screen:
    return kBlendOp_Screen;
  case kBlendMode_Overlay:
    return kBlendOp_Overlay;
  case kBlendMode_Darken:
    return kBlendOp_Darken;
  case kBlendMode_Lighten:
    return kBlendOp_Lighten;
  case kBlendMode_ColorDodge:
    return kBlendOp_ColorDodge;
  case kBlendMode_ColorBurn:
    return kBlendOp_ColorBurn;
  case kBlendMode_HardLight:
    return kBlendOp_HardLight;
  case kBlendMode_SoftLight:
    return kBlendOp_SoftLight;
  case kBlendMode_Difference:
    return kBlendOp_Difference;
  case kBlendMode_Exclusion:
    return kBlendOp_Exclusion;
  case kBlendMode_Hue:
    return kBlendOp_Hue;
  case kBlendMode_Saturation:
    return kBlendOp_Saturation;
  case kBlendMode_Color:
    return kBlendOp_Color;
  case kBlendMode_Luminosity:
    return kBlendOp_Luminosity;
  default:
    return kBlendOp_Over;
  }
}

struct UExport Paint {
  Color color;
};

enum BorderStyle {
  kBorderStyle_None,
  kBorderStyle_Dotted,
  kBorderStyle_Dashed,
  kBorderStyle_Solid,
  kBorderStyle_Double,
  kBorderStyle_Groove,
  kBorderStyle_Ridge,
  kBorderStyle_Inset,
  kBorderStyle_Outset
};

struct UExport BorderPaint {
  Color color;
  BorderStyle style;
};

}  // namespace ultralight
