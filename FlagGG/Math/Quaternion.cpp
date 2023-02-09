#include "Math/Quaternion.h"
#include "Math/Math.h"

#include <cstdio>
#include <math.h>

namespace FlagGG
{

const Quaternion Quaternion::IDENTITY;

Quaternion::Quaternion()
	:w_(1.0f),
	x_(0.0f),
	y_(0.0f),
	z_(0.0f)
{
}

Quaternion::Quaternion(const Quaternion& quat)
	:w_(quat.w_),
	x_(quat.x_),
	y_(quat.y_),
	z_(quat.z_)
{
}

Quaternion::Quaternion(Real w, Real x, Real y, Real z)
	:w_(w),
	x_(x),
	y_(y),
	z_(z)
{
}

Quaternion::Quaternion(const Real* data)
	:w_(data[0]),
	x_(data[1]),
	y_(data[2]),
	z_(data[3])
{
}

Quaternion::Quaternion(Real angle, const Vector3& axis)
{
	FromAngleAxis(angle, axis);
}

Quaternion::Quaternion(Real angle)
{
	FromAngleAxis(angle, Vector3::FORWARD);
}

Quaternion::Quaternion(Real x, Real y, Real z)
{
	FromEulerAngles(x, y, z);
}

Quaternion::Quaternion(const Vector3& start, const Vector3& end)
{
	FromRotationTo(start, end);
}

Quaternion::Quaternion(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
{
	FromAxes(xAxis, yAxis, zAxis);
}

Quaternion::Quaternion(const Matrix3& matrix)
{
	FromRotationMatrix(matrix);
}

Quaternion& Quaternion::operator=(const Quaternion& rhs)
{
	w_ = rhs.w_;
	x_ = rhs.x_;
	y_ = rhs.y_;
	z_ = rhs.z_;
	return *this;
}

Quaternion& Quaternion::operator +=(const Quaternion& rhs)
{
	w_ += rhs.w_;
	x_ += rhs.x_;
	y_ += rhs.y_;
	z_ += rhs.z_;
	return *this;
}

Quaternion& Quaternion::operator *=(Real rhs)
{
	w_ *= rhs;
	x_ *= rhs;
	y_ *= rhs;
	z_ *= rhs;
	return *this;
}

bool Quaternion::operator ==(const Quaternion& rhs) const
{
	return w_ == rhs.w_ && x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_;
}

bool Quaternion::operator !=(const Quaternion& rhs) const { return !(*this == rhs); }

Quaternion Quaternion::operator *(Real rhs) const
{
	return Quaternion(w_ * rhs, x_ * rhs, y_ * rhs, z_ * rhs);
}

Quaternion Quaternion::operator -() const
{
	return Quaternion(-w_, -x_, -y_, -z_);
}

Quaternion Quaternion::operator +(const Quaternion& rhs) const
{
	return Quaternion(w_ + rhs.w_, x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
}

Quaternion Quaternion::operator -(const Quaternion& rhs) const
{
	return Quaternion(w_ - rhs.w_, x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_);
}

Quaternion Quaternion::operator *(const Quaternion& rhs) const
{
	return Quaternion(
		w_ * rhs.w_ - x_ * rhs.x_ - y_ * rhs.y_ - z_ * rhs.z_,
		w_ * rhs.x_ + x_ * rhs.w_ + y_ * rhs.z_ - z_ * rhs.y_,
		w_ * rhs.y_ + y_ * rhs.w_ + z_ * rhs.x_ - x_ * rhs.z_,
		w_ * rhs.z_ + z_ * rhs.w_ + x_ * rhs.y_ - y_ * rhs.x_
		);
}

Vector3 Quaternion::operator *(const Vector3& rhs) const
{
	Vector3 qVec(x_, y_, z_);
	Vector3 cross1(qVec.CrossProduct(rhs));
	Vector3 cross2(qVec.CrossProduct(cross1));

	return rhs + (cross1 * w_ + cross2) * 2.0f;
}

void Quaternion::FromAngleAxis(Real angle, const Vector3& axis)
{
	Vector3 normAxis = axis.Normalized();
	angle *= F_DEGTORAD_2;
	Real sinAngle = sinf(angle);
	Real cosAngle = cosf(angle);

	w_ = cosAngle;
	x_ = normAxis.x_ * sinAngle;
	y_ = normAxis.y_ * sinAngle;
	z_ = normAxis.z_ * sinAngle;
}

void Quaternion::FromEulerAngles(Real x, Real y, Real z)
{
	// Order of rotations: Z first, then X, then Y (mimics typical FPS camera with gimbal lock at top/bottom)
	x *= F_DEGTORAD_2;
	y *= F_DEGTORAD_2;
	z *= F_DEGTORAD_2;
	Real sinX = sinf(x);
	Real cosX = cosf(x);
	Real sinY = sinf(y);
	Real cosY = cosf(y);
	Real sinZ = sinf(z);
	Real cosZ = cosf(z);

	w_ = cosY * cosX * cosZ + sinY * sinX * sinZ;
	x_ = cosY * sinX * cosZ + sinY * cosX * sinZ;
	y_ = sinY * cosX * cosZ - cosY * sinX * sinZ;
	z_ = cosY * cosX * sinZ - sinY * sinX * cosZ;
}

void Quaternion::FromRotationTo(const Vector3& start, const Vector3& end)
{
	Vector3 normStart = start.Normalized();
	Vector3 normEnd = end.Normalized();
	Real d = normStart.DotProduct(normEnd);

	if (d > -1.0f + EPS)
	{
		Vector3 c = normStart.CrossProduct(normEnd);
		Real s = sqrtf((1.0f + d) * 2.0f);
		Real invS = 1.0f / s;

		x_ = c.x_ * invS;
		y_ = c.y_ * invS;
		z_ = c.z_ * invS;
		w_ = 0.5f * s;
	}
	else
	{
		Vector3 axis = Vector3::RIGHT.CrossProduct(normStart);
		if (axis.Length() < EPS)
			axis = Vector3::UP.CrossProduct(normStart);

		FromAngleAxis(180.f, axis);
	}
}

void Quaternion::FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
{
	Matrix3 matrix(
		xAxis.x_, yAxis.x_, zAxis.x_,
		xAxis.y_, yAxis.y_, zAxis.y_,
		xAxis.z_, yAxis.z_, zAxis.z_
		);

	FromRotationMatrix(matrix);
}

void Quaternion::FromRotationMatrix(const Matrix3& matrix)
{
	Real t = matrix.m00_ + matrix.m11_ + matrix.m22_;

	if (t > 0.0f)
	{
		Real invS = 0.5f / sqrtf(1.0f + t);

		x_ = (matrix.m21_ - matrix.m12_) * invS;
		y_ = (matrix.m02_ - matrix.m20_) * invS;
		z_ = (matrix.m10_ - matrix.m01_) * invS;
		w_ = 0.25f / invS;
	}
	else
	{
		if (matrix.m00_ > matrix.m11_ && matrix.m00_ > matrix.m22_)
		{
			Real invS = 0.5f / sqrtf(1.0f + matrix.m00_ - matrix.m11_ - matrix.m22_);

			x_ = 0.25f / invS;
			y_ = (matrix.m01_ + matrix.m10_) * invS;
			z_ = (matrix.m20_ + matrix.m02_) * invS;
			w_ = (matrix.m21_ - matrix.m12_) * invS;
		}
		else if (matrix.m11_ > matrix.m22_)
		{
			Real invS = 0.5f / sqrtf(1.0f + matrix.m11_ - matrix.m00_ - matrix.m22_);

			x_ = (matrix.m01_ + matrix.m10_) * invS;
			y_ = 0.25f / invS;
			z_ = (matrix.m12_ + matrix.m21_) * invS;
			w_ = (matrix.m02_ - matrix.m20_) * invS;
		}
		else
		{
			Real invS = 0.5f / sqrtf(1.0f + matrix.m22_ - matrix.m00_ - matrix.m11_);

			x_ = (matrix.m02_ + matrix.m20_) * invS;
			y_ = (matrix.m12_ + matrix.m21_) * invS;
			z_ = 0.25f / invS;
			w_ = (matrix.m10_ - matrix.m01_) * invS;
		}
	}
}

bool Quaternion::FromLookRotation(const Vector3& direction, const Vector3& up)
{
	Quaternion ret;
	Vector3 forward = direction.Normalized();

	Vector3 v = forward.CrossProduct(up);
	// If direction & up are parallel and crossproduct becomes zero, use FromRotationTo() fallback
	if (v.LengthSquared() >= EPS)
	{
		v.Normalize();
		Vector3 up = v.CrossProduct(forward);
		Vector3 right = up.CrossProduct(forward);
		ret.FromAxes(right, up, forward);
	}
	else
		ret.FromRotationTo(Vector3::FORWARD, forward);

	if (!ret.IsNaN())
	{
		(*this) = ret;
		return true;
	}
	else
		return false;
}

void Quaternion::Normalize()
{
	Real lenSquared = LengthSquared();
	if (!FlagGG::Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
	{
		Real invLen = 1.0f / sqrtf(lenSquared);
		w_ *= invLen;
		x_ *= invLen;
		y_ *= invLen;
		z_ *= invLen;
	}
}

Quaternion Quaternion::Normalized() const
{
	Real lenSquared = LengthSquared();
	if (!FlagGG::Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
	{
		Real invLen = 1.0f / sqrtf(lenSquared);
		return *this * invLen;
	}
	else
		return *this;
}

Quaternion Quaternion::Inverse() const
{
	Real lenSquared = LengthSquared();
	if (lenSquared == 1.0f)
		return Conjugate();
	else if (lenSquared >= EPS)
		return Conjugate() * (1.0f / lenSquared);
	else
		return IDENTITY;
}

Real Quaternion::LengthSquared() const
{
	return w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_;
}

Real Quaternion::DotProduct(const Quaternion& rhs) const
{
	return w_ * rhs.w_ + x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_;
}

bool Quaternion::Equals(const Quaternion& rhs) const
{
	return FlagGG::Equals(w_, rhs.w_) && FlagGG::Equals(x_, rhs.x_) && FlagGG::Equals(y_, rhs.y_) && FlagGG::Equals(z_, rhs.z_);
}

bool Quaternion::IsNaN() const { return FlagGG::IsNaN(w_) || FlagGG::IsNaN(x_) || FlagGG::IsNaN(y_) || FlagGG::IsNaN(z_); }

Quaternion Quaternion::Conjugate() const
{
	return Quaternion(w_, -x_, -y_, -z_);
}

Vector3 Quaternion::EulerAngles() const
{
	// Derivation from http://www.geometrictools.com/Documentation/EulerAngles.pdf
	// Order of rotations: Z first, then X, then Y
	Real check = 2.0f * (-y_ * z_ + w_ * x_);

	if (check < -0.995f)
	{
		return Vector3(
			-90.0f,
			0.0f,
			-atan2f(2.0f * (x_ * z_ - w_ * y_), 1.0f - 2.0f * (y_ * y_ + z_ * z_)) * F_RADTODEG
			);
	}
	else if (check > 0.995f)
	{
		return Vector3(
			90.0f,
			0.0f,
			atan2f(2.0f * (x_ * z_ - w_ * y_), 1.0f - 2.0f * (y_ * y_ + z_ * z_)) * F_RADTODEG
			);
	}
	else
	{
		return Vector3(
			asinf(check) * F_RADTODEG,
			atan2f(2.0f * (x_ * z_ + w_ * y_), 1.0f - 2.0f * (x_ * x_ + y_ * y_)) * F_RADTODEG,
			atan2f(2.0f * (x_ * y_ + w_ * z_), 1.0f - 2.0f * (x_ * x_ + z_ * z_)) * F_RADTODEG
			);
	}
}

Real Quaternion::YawAngle() const
{
	return EulerAngles().y_;
}

Real Quaternion::PitchAngle() const
{
	return EulerAngles().x_;
}

Real Quaternion::RollAngle() const
{
	return EulerAngles().z_;
}

Vector3 Quaternion::Axis() const
{
	return Vector3(x_, y_, z_) / sqrt(1. - w_ * w_);
}

Real Quaternion::Angle() const
{
	return 2 * Acos(w_);
}

Matrix3 Quaternion::RotationMatrix() const
{
	return Matrix3(
		1.0f - 2.0f * y_ * y_ - 2.0f * z_ * z_,
		2.0f * x_ * y_ - 2.0f * w_ * z_,
		2.0f * x_ * z_ + 2.0f * w_ * y_,
		2.0f * x_ * y_ + 2.0f * w_ * z_,
		1.0f - 2.0f * x_ * x_ - 2.0f * z_ * z_,
		2.0f * y_ * z_ - 2.0f * w_ * x_,
		2.0f * x_ * z_ - 2.0f * w_ * y_,
		2.0f * y_ * z_ + 2.0f * w_ * x_,
		1.0f - 2.0f * x_ * x_ - 2.0f * y_ * y_
		);
}

Quaternion Quaternion::Slerp(const Quaternion& rhs, Real t) const
{
	Real cosAngle = DotProduct(rhs);
	Real sign = 1.0f;

	if (cosAngle < 0.0f)
	{
		cosAngle = -cosAngle;
		sign = -1.0f;
	}

	Real angle = acosf(cosAngle);
	Real sinAngle = sinf(angle);
	Real t1, t2;

	if (sinAngle > 0.001f)
	{
		Real invSinAngle = 1.0f / sinAngle;
		t1 = sinf((1.0f - t) * angle) * invSinAngle;
		t2 = sinf(t * angle) * invSinAngle;
	}
	else
	{
		t1 = 1.0f - t;
		t2 = t;
	}

	return *this * t1 + (rhs * sign) * t2;
}

Quaternion Quaternion::Nlerp(const Quaternion& rhs, Real t, bool shortestPath) const
{
	Quaternion result;
	Real fCos = DotProduct(rhs);
	if (fCos < 0.0f && shortestPath)
		result = (*this) + (((-rhs) - (*this)) * t);
	else
		result = (*this) + ((rhs - (*this)) * t);
	result.Normalize();
	return result;
}

String Quaternion::ToString() const
{
	char tempBuffer[CONVERSION_BUFFER_LENGTH];
	sprintf(tempBuffer, "%g %g %g %g", w_, x_, y_, z_);
	return String(tempBuffer);
}

}
