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
const Real F_EPSILON = 1e-6;
const Int32 F_MIN_INT = 0x80000000;
const Int32 F_MAX_INT = 0x7fffffff;
const UInt32 F_MIN_UNSIGNED = 0x00000000;
const UInt32 F_MAX_UNSIGNED = 0xffffffff;
const Real F_INFINITY = (Real)HUGE_VAL;
const Real F_MIN_NEARCLIP = 0.01f;
const Real F_DEGTORAD = PI / 180.0f;
const Real F_DEGTORAD_2 = PI / 360.0f;
const Real F_RADTODEG = 1.0f / F_DEGTORAD;
const Real F_OVERSQRT2 = float(0.7071067811865475244008443621048490);

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

Real Ln(Real number)
{
	return log(number);
}

Real Fract(Real number)
{
	return number - floor(number);
}

Real Floor(Real number)
{
	return floor(number);
}

Int32 FloorToInt(Real number)
{
	return static_cast<Int32>(Floor(number));
}

Real Round(Real number)
{
	return round(number);
}

Int32 RoundToInt(Real number)
{
	return static_cast<Int32>(Round(number));
}

Real Ceil(Real number)
{
	return ceil(number);
}

Int32 CeilToInt(Real number)
{
	return static_cast<Int32>(Ceil(number));
}

Real Lerp(float from, float to, float t)
{
	return to * t + from * (1.0F - t);
}

int Compare(Real _1, Real _2)
{
	if (fabs(_1 - _2) < F_EPSILON) return 0;
	return _1 < _2 ? -1 : 1;
}

Real Equals(Real _1, Real _2)
{
	return Compare(_1, _2) == 0;
}

Real Equals(Real _1, Real _2, Real tolerance)
{
	return fabs(_1 - _2) < tolerance;
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

UInt32 SDBM_Hash(UInt32 hashValue, UInt8 charValue)
{
	return static_cast<UInt32>(charValue) + (hashValue << 6u) + (hashValue << 16u) - hashValue;
}

bool IsPowerOfTwo(UInt32 value)
{
	return !(value & (value - 1));
}

UInt32 NextPowerOfTwo(UInt32 value)
{
	// http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
	--value;
	value |= value >> 1u;
	value |= value >> 2u;
	value |= value >> 4u;
	value |= value >> 8u;
	value |= value >> 16u;
	return ++value;
}

static Int32 GSRandSeed;

void SRandInit(Int32 Seed)
{
	GSRandSeed = Seed;
}

Int32 GetRandSeed()
{
	return GSRandSeed;
}

float SRand()
{
	GSRandSeed = (GSRandSeed * 196314165) + 907633515;
	union { float f; Int32 i; } Result;
	union { float f; Int32 i; } Temp;
	const float SRandTemp = 1.0f;
	Temp.f = SRandTemp;
	Result.i = (Temp.i & 0xff800000) | (GSRandSeed & 0x007fffff);
	return Fract(Result.f);
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
