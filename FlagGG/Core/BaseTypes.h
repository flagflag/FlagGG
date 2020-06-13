#pragma once

#include <stdint.h>

typedef uint8_t Byte;
typedef int8_t Int8;
typedef uint8_t UInt8;
typedef int16_t Int16;
typedef uint16_t UInt16;
typedef int32_t Int32;
typedef uint32_t UInt32;
typedef int64_t Int64;
typedef uint64_t UInt64;
typedef size_t Size;
typedef float Real;
typedef double RealD;


static const Int8  Int8_MIN = Int8(-128);
static const Int8  Int8_MAX = Int8(127);

static const UInt8  Uint8_MIN = UInt8(0);
static const UInt8  Uint8_MAX = UInt8(255);

static const Int16 Int16_MIN = Int16(-32768);
static const Int16 Int16_MAX = Int16(32767);

static const UInt16 Uint16_MIN = UInt16(0);
static const UInt16 Uint16_MAX = UInt16(65535);

static const Int32 Int32_MIN = Int32(-2147483647 - 1);
static const Int32 Int32_MAX = Int32(2147483647);

static const UInt32 Uint32_MIN = UInt32(0);
static const UInt32 Uint32_MAX = UInt32(0xffffffff);

static const Real Float32_MIN = Real(1.175494351e-38F);
static const Real Float32_MAX = Real(3.402823466e+38F);

static const Real Float32_Sqrt2 = Real(1.41421356237309504880f);
static const Real Float32_SqrtHalf = Real(0.7071067811865475244008443f);

extern const Real Float32_Inf;
static const Real Float32_One = Real(1.0);
static const Real Float32_Half = Real(0.5);
static const Real Float32_Zero = Real(0.0);

static const Real Float32_PI = Real(3.14159265358979323846);
static const Real Float32_2PI = Real(2.0 * 3.14159265358979323846);
static const Real Float32_InversePI = Real(1.0 / 3.14159265358979323846);
static const Real Float32_HalfPI = Real(0.5 * 3.14159265358979323846);
static const Real Float32_2InversePI = Real(2.0 / 3.14159265358979323846);
static const Real Float32_Inverse2PI = Real(0.5 / 3.14159265358979323846);


#ifndef MTU
#define MTU 1500 //ÒÔÌ«Íø±ê×¼MTU
#endif
