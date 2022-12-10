#include "Math.h"
#include "Vector3.h"
#include "Matrix4.h"

#include <math.h>

#if _WIN32
#include <d3dx9math.h>
#endif

namespace FlagGG
{

const Real PI = asin(1.0f) * 2;
const Real EPS = 1e-6;
const Int32 F_MIN_INT = 0x80000000;
const Int32 F_MAX_INT = 0x7fffffff;
const UInt32 F_MIN_UNSIGNED = 0x00000000;
const UInt32 F_MAX_UNSIGNED = 0xffffffff;
const Real F_INFINITY = (Real)HUGE_VAL;
const Real F_MIN_NEARCLIP = 0.01f;
const Real DEGTORAD = PI / 180.0f;
const Real DEGTORAD_2 = PI / 360.0f;
const Real RADTODEG = 1.0f / DEGTORAD;

bool IsNaN(Real number)
{
	return isnan(number);
}

Real Sin(Real angle)
{
	return sin(angle * 180.0f / PI);
}

Real Cos(Real angle)
{
	return cos(angle * 180.0f / PI);
}

Real Tan(Real angle)
{
	return tan(angle * 180.0f / PI);
}

Real Asin(Real angle)
{
	return asin(angle * 180.0f / PI);
}

Real Acos(Real angle)
{
	return acos(angle * 180.0f / PI);
}

Real Atan(Real angle)
{
	return atan(angle * 180.0f / PI);
}

Real Abs(Real number)
{
	return fabs(number);
}

Real Sqrt(Real number)
{
	return sqrt(number);
}

Real Fract(Real number)
{
	return number - floor(number);
}

int Compare(Real _1, Real _2)
{
	if (fabs(_1 - _2) <  EPS) return 0;
	return _1 < _2 ? -1 : 1;
}

Real Equals(Real _1, Real _2)
{
	return Compare(_1, _2) == 0;
}

Real Clamp(Real target, Real min, Real max)
{
	if (target < min)
		return min;
	else if (target > max)
		return max;
	else
		return target;
}

UInt32 FloatToRawIntBits(Real value)
{
	return *((UInt32*)&value);
}

UInt32 HashString(const char* str)
{
	UInt32 hash = 0;
	while (*str)
	{
		// SDBM Hash
		hash = static_cast<UInt32>(*str) + (hash << 6u) + (hash << 16u) - hash;
		++str;
	}

	return hash;
}

Matrix4 MatrixTranslation(Real dx, Real dy, Real dz)
{
	return Matrix4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		dx, dy, dz, 1.0f
		);
}

Matrix4 MatrixRotationX(Real angle)
{
	Real sina = Sin(angle);
	Real cosa = Cos(angle);

	return Matrix4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cosa, -sina, 0.0f,
		0.0f, sina, cosa, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);
}

Matrix4 MatrixRotationY(Real angle)
{
	Real sina = Sin(angle);
	Real cosa = Cos(angle);

	return Matrix4(
		cosa, 0.0f, sina, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sina, 0.0f, cosa, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);
}

Matrix4 MatrixRotationZ(Real angle)
{
	Real sina = Sin(angle);
	Real cosa = Cos(angle);

	return Matrix4(
		cosa, -sina, 0.0f, 0.0f,
		sina, cosa, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);
}

Matrix4 MatrixRotationAxis(const Vector3& axis, Real angle)
{
	Real x = axis.x_;
	Real y = axis.y_;
	Real z = axis.z_;
	Real sina = Sin(angle);
	Real cosa = Cos(angle);

	return Matrix4(
			cosa + (1.0f - cosa) * x * x,	(1.0f - cosa) * x * y - sina * z,	(1.0f - cosa) * x * z + sina * y,	0.0f,
		(1.0f - cosa) * y * z + sina * z,		cosa + (1.0f - cosa) * y * y,	(1.0f - cosa) * y * z - sina * x,	0.0f,
		(1.0f - cosa) * z * x - sina * y,	(1.0f - cosa) * z * y + sina * x,		cosa + (1.0f - cosa) * z * z,	0.0f,
									0.0f,								0.0f,								0.0f,	1.0f
		);
}

Vector3 Vector3TransformNormal(const Vector3& target, const Matrix4& T)
{
	Matrix4 mat(
		target.x_, target.y_, target.z_, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
		);

	mat = mat * T;

	return Vector3(mat.m00_, mat.m01_, mat.m02_);
}

Vector3 Vector3TransformCoord(const Vector3& target, const Matrix4& T)
{
	Matrix4 mat(
		target.x_, target.y_, target.z_, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
		);

	mat = mat * T;

	return Vector3(mat.m00_ / mat.m03_, mat.m01_ / mat.m03_, mat.m02_ / mat.m03_);
}

Matrix4 MatrixPerspectiveFovLH(Real fovy, Real aspect, Real zn, Real zf)
{
#if _WIN32
	D3DXMATRIX out;
	D3DXMatrixPerspectiveFovLH(
		&out,
		fovy,
		aspect,
		zn,
		zf
		);
#endif

	Matrix4 output;
#if _WIN32
	memcpy(&output, &out, sizeof(Matrix4));
#endif

	return output;
}

}
