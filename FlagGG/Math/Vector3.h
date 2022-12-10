#ifndef __VECTOR3__
#define __VECTOR3__

#include "Export.h"
#include "Define.h"
#include "Math/Vector2.h"

namespace FlagGG
{

class FlagGG_API IntVector3
{
public:
	IntVector3();

	IntVector3(int x, int y, int z);

	IntVector3(const IntVector3& other);

	int x_;

	int y_;

	int z_;
};

class FlagGG_API Vector3
{
public:
	Vector3() NOEXCEPT;

	Vector3(Real x, Real y, Real z) NOEXCEPT;

	Vector3(const Vector3& other) NOEXCEPT;

	Vector3(const Vector2& vector, Real z) NOEXCEPT;

	Vector3& operator =(const Vector3& rhs) NOEXCEPT = default;

	bool operator ==(const Vector3& rhs) const;

	bool operator !=(const Vector3& rhs) const;

	Vector3 operator +(const Vector3& rhs) const;

	Vector3 operator -() const;

	Vector3 operator -(const Vector3& rhs) const;

	Vector3 operator *(Real rhs) const;

	Vector3 operator *(const Vector3& rhs) const;

	Vector3 operator /(Real rhs) const;

	Vector3 operator /(const Vector3& rhs) const;

	Vector3& operator +=(const Vector3& rhs);

	Vector3& operator -=(const Vector3& rhs);

	Vector3& operator *=(Real rhs);

	Vector3& operator *=(const Vector3& rhs);

	Vector3& operator /=(Real rhs);

	Vector3& operator /=(const Vector3& rhs);

	void Normalize();

	Real Length() const;

	Real LengthSquared() const;

	Real DotProduct(const Vector3& rhs) const;

	Real AbsDotProduct(const Vector3& rhs) const;

	Real ProjectOntoAxis(const Vector3& axis) const;

	Vector3 ProjectOntoPlane(const Vector3& origin, const Vector3& normal) const;

	Vector3 ProjectOntoLine(const Vector3& from, const Vector3& to, bool clamped = false) const;

	Real DistanceToPoint(const Vector3& point) const;

	Real DistanceToPlane(const Vector3& origin, const Vector3& normal) const;

	Vector3 Orthogonalize(const Vector3& axis) const;

	Vector3 CrossProduct(const Vector3& rhs) const;

	Vector3 Abs() const;

	Vector3 Lerp(const Vector3& rhs, Real t) const;

	bool Equals(const Vector3& rhs) const;

	Real Angle(const Vector3& rhs) const;

	bool IsNaN() const;

	Vector3 Normalized() const;

	String ToString() const;

	Real x_;

	Real y_;

	Real z_;

	static const Vector3 ZERO;
	static const Vector3 LEFT;
	static const Vector3 RIGHT;
	static const Vector3 UP;
	static const Vector3 DOWN;
	static const Vector3 FORWARD;
	static const Vector3 BACK;
	static const Vector3 ONE;
};

inline Vector3 operator *(Real lhs, const Vector3& rhs) { return rhs * lhs; }

inline Vector3 VectorLerp(const Vector3& lhs, const Vector3& rhs, const Vector3& t) { return lhs + (rhs - lhs) * t; }

inline Vector3 VectorMin(const Vector3& lhs, const Vector3& rhs) { return Vector3(Min(lhs.x_, rhs.x_), Min(lhs.y_, rhs.y_), Min(lhs.z_, rhs.z_)); }

inline Vector3 VectorMax(const Vector3& lhs, const Vector3& rhs) { return Vector3(Max(lhs.x_, rhs.x_), Max(lhs.y_, rhs.y_), Max(lhs.z_, rhs.z_)); }

}

#endif