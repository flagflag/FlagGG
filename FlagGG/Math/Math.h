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

// λ�ƾ���
Matrix4 FlagGG_API MatrixTranslation(Real dx, Real dy, Real dz);

// X��˳ʱ����ת����
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
Matrix4 FlagGG_API MatrixRotationX(Real angle);

// Y��˳ʱ����ת����
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
Matrix4 FlagGG_API MatrixRotationY(Real angle);

// Z��˳ʱ����ת����
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
Matrix4 FlagGG_API MatrixRotationZ(Real angle);

// ������˳ʱ����ת����
// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
Matrix4 FlagGG_API MatrixRotationAxis(const Vector3& axis, Real angle);

Vector3 FlagGG_API Vector3TransformNormal(const Vector3& target, const Matrix4& T);

Vector3 FlagGG_API Vector3TransformCoord(const Vector3& target, const Matrix4& T);

/**
  * @brief
  * @param eye - �������λ��
  * @param at  - �۲���λ��
  * @param up  - ����������Ϸ���
  */
Matrix4 FlagGG_API MatrixLookAtLH(const Matrix4& eye, const Matrix4& at, const Matrix4& up);

/**
  * @brief ͶӰ����任
  * @param fovy   - ����ָ���Ի���Ϊ��λ�������������y���ϵĳ���Ƕȣ�������Ƕȣ�View of View��������Ƕ�Խ��ӳ�䵽ͶӰ�����е�ͼ�ξ�ԽС����֮��ͶӰͼ���Խ��
  * @param aspect - ����������Ļ��ʾ���ĺ��ݱȣ�����ֵ��Ϊ��Ļ�Ŀ��/�߶ȡ���Ӧ��ͬ��������ʾ��Ļ������16/9��4/3�ȵȣ�������ʾ��ͶӰͼ����ܻ�ʹͼ������
  * @param zn     - ��ʾ�ӽ����н��ü���������������λ�ã������۵���������ش���֮��ľ���
  * @param zf     - ��ʾ�ӽ�����Զ�ü���������������λ�ã������۵��������ɫǽ�ڡ�֮��ľ���
  * ע��������Բο����https://blog.csdn.net/poem_qianmo/article/details/8408723
  */
Matrix4 FlagGG_API MatrixPerspectiveFovLH(Real fovy, Real aspect, Real zn, Real zf);

}

#endif