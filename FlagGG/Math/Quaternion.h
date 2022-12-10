#pragma once

#include "Export.h"
#include "Define.h"
#include "Math/Matrix3.h"
#include "Math/Vector3.h"
#include "Container/Str.h"

namespace FlagGG
{

class FlagGG_API Quaternion
{
public:
	Quaternion() NOEXCEPT;

	Quaternion(const Quaternion& quat) NOEXCEPT;

	Quaternion(Real w, Real x, Real y, Real z) NOEXCEPT;

	explicit Quaternion(const Real* data) NOEXCEPT;

	Quaternion(Real angle, const Vector3& axis) NOEXCEPT;

	explicit Quaternion(Real angle) NOEXCEPT;

	Quaternion(Real x, Real y, Real z) NOEXCEPT;

	Quaternion(const Vector3& start, const Vector3& end) NOEXCEPT;

	Quaternion(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis) NOEXCEPT;

	explicit Quaternion(const Matrix3& matrix) NOEXCEPT;

	Quaternion& operator =(const Quaternion& rhs) NOEXCEPT;
	Quaternion& operator +=(const Quaternion& rhs);

	Quaternion& operator *=(Real rhs);

	bool operator ==(const Quaternion& rhs) const;

	bool operator !=(const Quaternion& rhs) const;

	Quaternion operator *(Real rhs) const;

	Quaternion operator -() const;

	Quaternion operator +(const Quaternion& rhs) const;

	Quaternion operator -(const Quaternion& rhs) const;

	Quaternion operator *(const Quaternion& rhs) const;

	Vector3 operator *(const Vector3& rhs) const;

	void FromAngleAxis(Real angle, const Vector3& axis);

	void FromEulerAngles(Real x, Real y, Real z);

	void FromRotationTo(const Vector3& start, const Vector3& end);

	void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);

	void FromRotationMatrix(const Matrix3& matrix);
			
	bool FromLookRotation(const Vector3& direction, const Vector3& up = Vector3::UP);

	void Normalize();

	Quaternion Normalized() const;

	Quaternion Inverse() const;

	Real LengthSquared() const;

	Real DotProduct(const Quaternion& rhs) const;

	bool Equals(const Quaternion& rhs) const;

	bool IsNaN() const;

	Quaternion Conjugate() const;

	Vector3 EulerAngles() const;

	Real YawAngle() const;

	Real PitchAngle() const;

	Real RollAngle() const;

	Vector3 Axis() const;

	Real Angle() const;

	Matrix3 RotationMatrix() const;

	Quaternion Slerp(const Quaternion& rhs, Real t) const;

	Quaternion Nlerp(const Quaternion& rhs, Real t, bool shortestPath = false) const;

	const Real* Data() const { return &w_; }

	String ToString() const;

	Real w_;
	Real x_;
	Real y_;
	Real z_;

	static const Quaternion IDENTITY;
};

}
