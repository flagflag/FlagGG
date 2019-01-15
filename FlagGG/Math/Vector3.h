#ifndef __VECTOR3__
#define __VECTOR3__

#include "Export.h"

namespace FlagGG
{
	namespace Math
	{
		struct FlagGG_API IntVector3
		{
			IntVector3();

			IntVector3(int x, int y, int z);

			IntVector3(const IntVector3& other);

			int x_;

			int y_;

			int z_;
		};

		struct FlagGG_API Vector3
		{
			Vector3();

			Vector3(float x, float y, float z);

			Vector3(const Vector3& other);

			Vector3 operator +(const Vector3& rhs) const;

			Vector3 operator -() const;

			Vector3 operator -(const Vector3& rhs) const;

			Vector3 operator *(float rhs) const;

			Vector3 operator *(const Vector3& rhs) const;

			Vector3 operator /(float rhs) const;

			Vector3 operator /(const Vector3& rhs) const;

			Vector3& operator +=(const Vector3& rhs);

			Vector3& operator -=(const Vector3& rhs);

			Vector3& operator *=(float rhs);

			Vector3& operator *=(const Vector3& rhs);

			Vector3& operator /=(float rhs);

			Vector3& operator /=(const Vector3& rhs);

			void Normalize();

			float Length() const;

			float LengthSquared() const;

			float DotProduct(const Vector3& rhs) const;

			float AbsDotProduct(const Vector3& rhs) const;

			float ProjectOntoAxis(const Vector3& axis) const;

			Vector3 ProjectOntoPlane(const Vector3& origin, const Vector3& normal) const;

			Vector3 ProjectOntoLine(const Vector3& from, const Vector3& to, bool clamped = false) const;

			float DistanceToPoint(const Vector3& point) const;

			float DistanceToPlane(const Vector3& origin, const Vector3& normal) const;

			Vector3 Orthogonalize(const Vector3& axis) const;

			Vector3 CrossProduct(const Vector3& rhs) const;

			Vector3 Abs() const;

			Vector3 Lerp(const Vector3& rhs, float t) const;

			bool Equals(const Vector3& rhs) const;

			float Angle(const Vector3& rhs) const;

			bool IsNaN() const;

			Vector3 Normalized() const;

			float x_;

			float y_;

			float z_;
		};
	}
}

#endif