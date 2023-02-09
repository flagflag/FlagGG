#ifndef __MATH__
#define __MATH__

#include "Export.h"
#include "Container/Str.h"
#include "Core/BaseTypes.h"

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
extern FlagGG_API const Real EPS;
extern FlagGG_API const Int32 F_MIN_INT;
extern FlagGG_API const Int32 F_MAX_INT;
extern FlagGG_API const UInt32 F_MIN_UNSIGNED;
extern FlagGG_API const UInt32 F_MAX_UNSIGNED;
extern FlagGG_API const Real F_INFINITY;
extern FlagGG_API const Real F_MIN_NEARCLIP;
extern FlagGG_API const Real F_DEGTORAD;
extern FlagGG_API const Real F_DEGTORAD_2;
extern FlagGG_API const Real F_RADTODEG;

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

bool FlagGG_API IsNaN(Real number);

Real FlagGG_API Sin(Real angle);

Real FlagGG_API Cos(Real angle);

Real FlagGG_API Tan(Real angle);

Real FlagGG_API Asin(Real angle);

Real FlagGG_API Acos(Real angle);

Real FlagGG_API Atan(Real angle);


Real FlagGG_API Abs(Real number);

Real FlagGG_API Sqrt(Real number);

Real FlagGG_API Fract(Real number);


int FlagGG_API Compare(Real _1, Real _2);

Real FlagGG_API Equals(Real _1, Real _2);

Real FlagGG_API Clamp(Real target, Real min, Real max);

UInt32 FlagGG_API FloatToRawIntBits(Real value);

UInt32 FlagGG_API HashString(const char* str);

UInt32 FlagGG_API SDBM_Hash(UInt32 hashValue, UInt8 charValue);

// 位移矩阵
Matrix4 FlagGG_API MatrixTranslation(Real dx, Real dy, Real dz);

// X轴顺时针旋转矩阵
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
Matrix4 FlagGG_API MatrixRotationX(Real angle);

// Y轴顺时针旋转矩阵
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
Matrix4 FlagGG_API MatrixRotationY(Real angle);

// Z轴顺时针旋转矩阵
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
Matrix4 FlagGG_API MatrixRotationZ(Real angle);

// 任意轴顺时针旋转矩阵
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
Matrix4 FlagGG_API MatrixRotationAxis(const Vector3& axis, Real angle);

Vector3 FlagGG_API Vector3TransformNormal(const Vector3& target, const Matrix4& T);

Vector3 FlagGG_API Vector3TransformCoord(const Vector3& target, const Matrix4& T);

/**
  * @brief
  * @param eye - 摄像机的位置
  * @param at  - 观察点的位置
  * @param up  - 摄像机的向上分量
  */
Matrix4 FlagGG_API MatrixLookAtLH(const Matrix4& eye, const Matrix4& at, const Matrix4& up);

/**
  * @brief 投影矩阵变换
  * @param fovy   - 用于指定以弧度为单位的虚拟摄像机在y轴上的成像角度，即视域角度（View of View），成像角度越大，映射到投影窗口中的图形就越小；反之，投影图像就越大
  * @param aspect - 用于描述屏幕显示区的横纵比，他的值就为屏幕的宽度/高度。对应不同比例的显示屏幕，比如16/9，4/3等等，最终显示的投影图像可能会使图像被拉伸
  * @param zn     - 表示视截体中近裁剪面距我们摄像机的位置，即人眼到“室内落地窗”之间的距离
  * @param zf     - 表示视截体中远裁剪面距我们摄像机的位置，即人眼到“室外黑色墙壁”之间的距离
  * 注：具体可以参考这里：https://blog.csdn.net/poem_qianmo/article/details/8408723
  */
Matrix4 FlagGG_API MatrixPerspectiveFovLH(Real fovy, Real aspect, Real zn, Real zf);

}

#endif