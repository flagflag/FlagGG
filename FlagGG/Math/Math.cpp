#include "Math.h"

#include <math.h>

#if WIN32 || WIN64
#include <d3dx9math.h>
#endif

namespace FlagGG
{
	namespace Math
	{
		const float PI = asin(1.0f) * 2;
		const float EPS = 1e-6;
		const float F_INFINITY = (float)HUGE_VAL;

		bool IsNaN(float number)
		{
			return isnan(number);
		}

		float Sin(float angle)
		{
			return sin(angle * 180.0f / PI);
		}

		float Cos(float angle)
		{
			return cos(angle * 180.0f / PI);
		}

		float Tan(float angle)
		{
			return tan(angle * 180.0f / PI);
		}

		float Asin(float angle)
		{
			return asin(angle * 180.0f / PI);
		}

		float Acos(float angle)
		{
			return acos(angle * 180.0f / PI);
		}

		float Atan(float angle)
		{
			return atan(angle * 180.0f / PI);
		}

		float Abs(float number)
		{
			return fabs(number);
		}

		float Sqrt(float number)
		{
			return sqrt(number);
		}

		float Fract(float number)
		{
			return number - floor(number);
		}

		int Compare(float _1, float _2)
		{
			if (fabs(_1 - _2) <  EPS) return 0;
			return _1 < _2 ? -1 : 1;
		}

		float Equals(float _1, float _2)
		{
			return Compare(_1, _2) == 0;
		}

		float Clamp(float target, float min, float max)
		{
			if (target < min)
				return min;
			else if (target > max)
				return max;
			else
				return target;
		}

		Matrix4 MatrixTranslation(float dx, float dy, float dz)
		{
			return Matrix4(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				dx, dy, dz, 1.0f
				);
		}

		Matrix4 MatrixRotationX(float angle)
		{
			float sina = Math::Sin(angle);
			float cosa = Math::Cos(angle);

			return Matrix4(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, cosa, -sina, 0.0f,
				0.0f, sina, cosa, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				);
		}

		Matrix4 MatrixRotationY(float angle)
		{
			float sina = Math::Sin(angle);
			float cosa = Math::Cos(angle);

			return Matrix4(
				cosa, 0.0f, sina, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				-sina, 0.0f, cosa, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				);
		}

		Matrix4 MatrixRotationZ(float angle)
		{
			float sina = Math::Sin(angle);
			float cosa = Math::Cos(angle);

			return Matrix4(
				cosa, -sina, 0.0f, 0.0f,
				sina, cosa, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				);
		}

		Matrix4 MatrixRotationAxis(const Vector3& axis, float angle)
		{
			float x = axis.x_;
			float y = axis.y_;
			float z = axis.z_;
			float sina = Math::Sin(angle);
			float cosa = Math::Cos(angle);

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

		Matrix4 MatrixPerspectiveFovLH(float fovy, float aspect, float zn, float zf)
		{
#if WIN32 || WIN64
			D3DXMATRIX out;
			D3DXMatrixPerspectiveFovLH(
				&out,
				fovy,
				aspect,
				zn,
				zf
				);
#endif

			Math::Matrix4 output;
#if WIN32 || WIN64
			memcpy(&output, &out, sizeof(Math::Matrix4));
#endif

			return output;
		}
	}
}
