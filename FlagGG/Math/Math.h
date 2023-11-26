#ifndef __MATH__
#define __MATH__

#include "Export.h"
#include "Container/Str.h"
#include "Core/BaseTypes.h"

#include <math.h>

namespace FlagGG
{

class Matrix4;
class Vector3;

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

template < class T >
inline T Max(T value1, T value2)
{
	return value1 > value2 ? value1 : value2;
}

template < class T >
inline T Min(T value1, T value2)
{
	return value1 < value2 ? value1 : value2;
}

FlagGG_API bool IsNaN(Real number);

FlagGG_API Real Sin(Real angle);

FlagGG_API Real Cos(Real angle);

FlagGG_API Real Tan(Real angle);

FlagGG_API Real Asin(Real angle);

FlagGG_API Real Acos(Real angle);

FlagGG_API Real Atan(Real angle);


FlagGG_API Real Abs(Real number);

FlagGG_API Real Sqrt(Real number);

/// Return X in power Y.
template <class T> inline T Pow(T x, T y) { return pow(x, y); }

FlagGG_API Real Ln(Real number);

/// Return floating-point remainder of X/Y.
template <class T> inline T Mod(T x, T y) { return fmod(x, y); }

FlagGG_API Real Fract(Real number);

FlagGG_API Real Floor(Real number);

FlagGG_API Int32 FloorToInt(Real number);

FlagGG_API Real Round(Real number);

FlagGG_API Int32 RoundToInt(Real number);

FlagGG_API Real Ceil(Real number);

FlagGG_API Int32 CeilToInt(Real number);


FlagGG_API Real Lerp(float from, float to, float t);

FlagGG_API int Compare(Real _1, Real _2);

FlagGG_API Real Equals(Real _1, Real _2);

FlagGG_API Real Equals(Real _1, Real _2, Real tolerance);

FlagGG_API Real Clamp(Real target, Real min, Real max);

FlagGG_API UInt32 FloatToRawIntBits(Real value);

FlagGG_API UInt32 HashString(const char* str);

FlagGG_API UInt32 SDBM_Hash(UInt32 hashValue, UInt8 charValue);

FlagGG_API bool IsPowerOfTwo(UInt32 value);

FlagGG_API UInt32 NextPowerOfTwo(UInt32 value);

// λ�ƾ���
FlagGG_API Matrix4 MatrixTranslation(Real dx, Real dy, Real dz);

// X��˳ʱ����ת����
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
FlagGG_API Matrix4 MatrixRotationX(Real angle);

// Y��˳ʱ����ת����
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
FlagGG_API Matrix4 MatrixRotationY(Real angle);

// Z��˳ʱ����ת����
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
FlagGG_API Matrix4 MatrixRotationZ(Real angle);

// ������˳ʱ����ת����
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
FlagGG_API Matrix4 MatrixRotationAxis(const Vector3& axis, Real angle);

FlagGG_API Vector3 Vector3TransformNormal(const Vector3& target, const Matrix4& T);

FlagGG_API Vector3 Vector3TransformCoord(const Vector3& target, const Matrix4& T);

/**
  * @brief
  * @param eye - �������λ��
  * @param at  - �۲���λ��
  * @param up  - ����������Ϸ���
  */
FlagGG_API Matrix4 MatrixLookAtLH(const Matrix4& eye, const Matrix4& at, const Matrix4& up);

/**
  * @brief ͶӰ����任
  * @param fovy   - ����ָ���Ի���Ϊ��λ�������������y���ϵĳ���Ƕȣ�������Ƕȣ�View of View��������Ƕ�Խ��ӳ�䵽ͶӰ�����е�ͼ�ξ�ԽС����֮��ͶӰͼ���Խ��
  * @param aspect - ����������Ļ��ʾ���ĺ��ݱȣ�����ֵ��Ϊ��Ļ�Ŀ��/�߶ȡ���Ӧ��ͬ��������ʾ��Ļ������16/9��4/3�ȵȣ�������ʾ��ͶӰͼ����ܻ�ʹͼ������
  * @param zn     - ��ʾ�ӽ����н��ü���������������λ�ã������۵���������ش���֮��ľ���
  * @param zf     - ��ʾ�ӽ�����Զ�ü���������������λ�ã������۵��������ɫǽ�ڡ�֮��ľ���
  * ע��������Բο����https://blog.csdn.net/poem_qianmo/article/details/8408723
  */
FlagGG_API Matrix4 MatrixPerspectiveFovLH(Real fovy, Real aspect, Real zn, Real zf);

}

#endif