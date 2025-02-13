#pragma once

#include "Export.h"
#include "Container/Str.h"
#include "Core/BaseTypes.h"
#include "Core/BaseMacro.h"

#include <math.h>
#include <stdlib.h>

namespace FlagGG
{

class Matrix4;
class Vector3;
class Vector4;

enum Intersection
{
	OUTSIDE,
	INTERSECTS,
	INSIDE
};

extern FlagGG_API const Real PI;
extern FlagGG_API const Real F_EPSILON;
extern FlagGG_API const Int32 F_MIN_INT;
extern FlagGG_API const Int32 F_MAX_INT;
extern FlagGG_API const UInt32 F_MIN_UNSIGNED;
extern FlagGG_API const UInt32 F_MAX_UNSIGNED;
extern FlagGG_API const Real F_INFINITY;
extern FlagGG_API const Real F_MIN_NEARCLIP;
extern FlagGG_API const Real F_DEGTORAD;
extern FlagGG_API const Real F_DEGTORAD_2;
extern FlagGG_API const Real F_RADTODEG;
extern FlagGG_API const Real F_OVERSQRT2;

#define SMALL_NUMBER		(1.e-8f)
#define KINDA_SMALL_NUMBER	(1.e-4f)
#define BIG_NUMBER			(3.4e+38f)

template <class T>
FORCEINLINE T Max(T value1, T value2)
{
	return value1 > value2 ? value1 : value2;
}

template <class T>
FORCEINLINE T Min(T value1, T value2)
{
	return value1 < value2 ? value1 : value2;
}

FlagGG_API bool IsNaN(Real number);

FlagGG_API bool IsFinite(Real number);

FlagGG_API Real Sin(Real angle);

FlagGG_API Real Cos(Real angle);

FlagGG_API Real Tan(Real angle);

FlagGG_API Real Asin(Real angle);

FlagGG_API Real Acos(Real angle);

FlagGG_API Real Atan(Real angle);


FlagGG_API Real Abs(Real number);

FlagGG_API Real Sqrt(Real number);

/// Return X in power Y.
template <class T>
FORCEINLINE T Pow(T x, T y) { return pow(x, y); }

FlagGG_API Real Ln(Real number);

/// Return floating-point remainder of X/Y.
template <class T>
FORCEINLINE T Mod(T x, T y) { return fmod(x, y); }

FlagGG_API Real Fract(Real number);

FlagGG_API Real Floor(Real number);

FlagGG_API Int32 FloorToInt(Real number);

FlagGG_API Real Round(Real number);

FlagGG_API Int32 RoundToInt(Real number);

FlagGG_API Real Ceil(Real number);

FlagGG_API Int32 CeilToInt(Real number);

template <class T>
FORCEINLINE T Lerp(const T& from, const T& to, const float t)
{
	return to * t + from * (1.0f - t);
}

template <class T>
FORCEINLINE T CubicInterp(const T& P0, const T& T0, const T& P1, const T& T1, const float A)
{
	const float A2 = A * A;
	const float A3 = A2 * A;

	return T((((2 * A3) - (3 * A2) + 1) * P0) + ((A3 - (2 * A2) + A) * T0) + ((A3 - A2) * T1) + (((-2 * A3) + (3 * A2)) * P1));
}

template <class T>
FORCEINLINE T CubicInterpDerivative(const T& P0, const T& T0, const T& P1, const T& T1, const float A)
{
	T a = 6.f * P0 + 3.f * T0 + 3.f * T1 - 6.f * P1;
	T b = -6.f * P0 - 4.f * T0 - 2.f * T1 + 6.f * P1;
	T c = T0;

	const float A2 = A * A;

	return T((a * A2) + (b * A) + c);
}

FlagGG_API int Compare(Real _1, Real _2);

FlagGG_API Real Equals(Real _1, Real _2);

FlagGG_API Real Equals(Real _1, Real _2, Real tolerance);

template <class T>
FORCEINLINE bool IsNearlyZero(const T& value)
{
	return value.IsNearlyZero();
}

template <>
FORCEINLINE bool IsNearlyZero(const float& value)
{
	return Abs(value) < F_EPSILON;
}

template <class T>
FORCEINLINE T Clamp(const T x, const T min, const T max) { return (x < min) ? min : (x < max) ? x : max; }

FlagGG_API UInt32 FloatToRawIntBits(Real value);

FlagGG_API UInt32 HashString(const char* str);

FlagGG_API UInt32 HashBuffer(const void* buffer, UInt32 bufferSize);

FlagGG_API UInt32 SDBM_Hash(UInt32 hashValue, UInt8 charValue);

FlagGG_API bool IsPowerOfTwo(UInt32 value);

FlagGG_API UInt32 NextPowerOfTwo(UInt32 value);

FORCEINLINE Int32 Rand() { return rand(); }

FORCEINLINE float Random() { return Rand() / 32768.0f; }

void SRandInit(Int32 Seed);

Int32 GetRandSeed();

float SRand();

// 位移矩阵
FlagGG_API Matrix4 MatrixTranslation(Real dx, Real dy, Real dz);

// X轴顺时针旋转矩阵
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
FlagGG_API Matrix4 MatrixRotationX(Real angle);

// Y轴顺时针旋转矩阵
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
FlagGG_API Matrix4 MatrixRotationY(Real angle);

// Z轴顺时针旋转矩阵
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
FlagGG_API Matrix4 MatrixRotationZ(Real angle);

// 任意轴顺时针旋转矩阵
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
FlagGG_API Matrix4 MatrixRotationAxis(const Vector3& axis, Real angle);

FlagGG_API Vector3 Vector3TransformNormal(const Vector3& target, const Matrix4& T);

FlagGG_API Vector3 Vector3TransformCoord(const Vector3& target, const Matrix4& T);

/**
  * @brief
  * @param eye - 摄像机的位置
  * @param at  - 观察点的位置
  * @param up  - 摄像机的向上分量
  */
FlagGG_API Matrix4 MatrixLookAtLH(const Matrix4& eye, const Matrix4& at, const Matrix4& up);

/**
  * @brief 投影矩阵变换
  * @param fovy   - 用于指定以弧度为单位的虚拟摄像机在y轴上的成像角度，即视域角度（View of View），成像角度越大，映射到投影窗口中的图形就越小；反之，投影图像就越大
  * @param aspect - 用于描述屏幕显示区的横纵比，他的值就为屏幕的宽度/高度。对应不同比例的显示屏幕，比如16/9，4/3等等，最终显示的投影图像可能会使图像被拉伸
  * @param nearZ  - 表示视截体中近裁剪面距我们摄像机的位置，即人眼到“室内落地窗”之间的距离
  * @param farZ   - 表示视截体中远裁剪面距我们摄像机的位置，即人眼到“室外黑色墙壁”之间的距离
  * 注：具体可以参考这里：https://blog.csdn.net/poem_qianmo/article/details/8408723
  */
FlagGG_API Matrix4 CreatePerspectiveMatrix(Real fov, Real aspect, Real nearZ, Real farZ, Real zoom);

/**
  * @brief 投影矩阵变换（ReverseZ）
  * @param fovy   - 用于指定以弧度为单位的虚拟摄像机在y轴上的成像角度，即视域角度（View of View），成像角度越大，映射到投影窗口中的图形就越小；反之，投影图像就越大
  * @param aspect - 用于描述屏幕显示区的横纵比，他的值就为屏幕的宽度/高度。对应不同比例的显示屏幕，比如16/9，4/3等等，最终显示的投影图像可能会使图像被拉伸
  * @param nearZ  - 表示视截体中近裁剪面距我们摄像机的位置，即人眼到“室内落地窗”之间的距离
  * @param farZ   - 表示视截体中远裁剪面距我们摄像机的位置，即人眼到“室外黑色墙壁”之间的距离
  * 注：具体可以参考这里：https://blog.csdn.net/poem_qianmo/article/details/8408723
  */
FlagGG_API Matrix4 CreateReverseZPerspectiveMatrix(Real fov, Real aspect, Real nearZ, Real farZ, Real zoom);

/**
 * @brief 创建设备深度到世界Z的转换
 */
FlagGG_API Vector4 CreateDeviceZToWorldZTransform(const Matrix4& projectMatrix);

}
