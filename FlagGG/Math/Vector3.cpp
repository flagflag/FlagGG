#include "Math/Vector3.h"
#include "Math/Math.h"

namespace FlagGG
{

const Vector3 Vector3::ZERO;
const Vector3 Vector3::LEFT(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::RIGHT(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UP(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::DOWN(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::FORWARD(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::BACK(0.0f, 0.0f, -1.0f);
const Vector3 Vector3::ONE(1.0f, 1.0f, 1.0f);

IntVector3::IntVector3() :
	x_(0),
	y_(0),
	z_(0)
{
}

IntVector3::IntVector3(int x, int y, int z) :
	x_(x),
	y_(y),
	z_(z)
{
}

IntVector3::IntVector3(const IntVector3& other)
{
	x_ = other.x_;
	y_ = other.y_;
	z_ = other.z_;
}

Vector3::Vector3() :
	x_(0.0f),
	y_(0.0f),
	z_(0.0f)
{
}

Vector3::Vector3(Real x, Real y, Real z) :
	x_(x),
	y_(y),
	z_(z)
{
}

Vector3::Vector3(const Vector3& other)
{
	x_ = other.x_;
	y_ = other.y_;
	z_ = other.z_;
}

Vector3::Vector3(const Vector2& vector, Real z) :
	x_(vector.x_),
	y_(vector.y_),
	z_(z)
{
}

bool Vector3::operator ==(const Vector3& rhs) const
{
	return x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_;
}

bool Vector3::operator !=(const Vector3& rhs) const
{
	return x_ != rhs.x_ || y_ != rhs.y_ || z_ != rhs.z_;
}

Vector3 Vector3::operator +(const Vector3& rhs) const
{ 
	return Vector3(x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
}

Vector3 Vector3::operator -() const
{ 
	return Vector3(-x_, -y_, -z_); 
}

Vector3 Vector3::operator -(const Vector3& rhs) const
{ 
	return Vector3(x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_);
}

Vector3 Vector3::operator *(Real rhs) const
{
	return Vector3(x_ * rhs, y_ * rhs, z_ * rhs);
}

Vector3 Vector3::operator *(const Vector3& rhs) const
{ 
	return Vector3(x_ * rhs.x_, y_ * rhs.y_, z_ * rhs.z_);
}

Vector3 Vector3::operator /(Real rhs) const
{ 
	return Vector3(x_ / rhs, y_ / rhs, z_ / rhs);
}

Vector3 Vector3::operator /(const Vector3& rhs) const
{ 
	return Vector3(x_ / rhs.x_, y_ / rhs.y_, z_ / rhs.z_);
}

Vector3& Vector3::operator +=(const Vector3& rhs)
{
	x_ += rhs.x_;
	y_ += rhs.y_;
	z_ += rhs.z_;
	return *this;
}

Vector3& Vector3::operator -=(const Vector3& rhs)
{
	x_ -= rhs.x_;
	y_ -= rhs.y_;
	z_ -= rhs.z_;
	return *this;
}

Vector3& Vector3::operator *=(Real rhs)
{
	x_ *= rhs;
	y_ *= rhs;
	z_ *= rhs;
	return *this;
}

Vector3& Vector3::operator *=(const Vector3& rhs)
{
	x_ *= rhs.x_;
	y_ *= rhs.y_;
	z_ *= rhs.z_;
	return *this;
}

Vector3& Vector3::operator /=(Real rhs)
{
	Real invRhs = 1.0f / rhs;
	x_ *= invRhs;
	y_ *= invRhs;
	z_ *= invRhs;
	return *this;
}

Vector3& Vector3::operator /=(const Vector3& rhs)
{
	x_ /= rhs.x_;
	y_ /= rhs.y_;
	z_ /= rhs.z_;
	return *this;
}

void Vector3::Normalize()
{
	Real lenSquared = LengthSquared();
	if (!FlagGG::Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
	{
		Real invLen = 1.0f / Sqrt(lenSquared);
		x_ *= invLen;
		y_ *= invLen;
		z_ *= invLen;
	}
}

Real Vector3::Length() const
{ 
	return Sqrt(x_ * x_ + y_ * y_ + z_ * z_);
}

Real Vector3::LengthSquared() const 
{ 
	return x_ * x_ + y_ * y_ + z_ * z_; 
}

Real Vector3::DotProduct(const Vector3& rhs) const
{ 
	return x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_;
}

Real Vector3::AbsDotProduct(const Vector3& rhs) const
{
	return FlagGG::Abs(x_ * rhs.x_) + FlagGG::Abs(y_ * rhs.y_) + FlagGG::Abs(z_ * rhs.z_);
}

Real Vector3::ProjectOntoAxis(const Vector3& axis) const
{ 
	return DotProduct(axis.Normalized());
}

Vector3 Vector3::ProjectOntoPlane(const Vector3& origin, const Vector3& normal) const
{
	const Vector3 delta = *this - origin;
	return *this - normal.Normalized() * delta.ProjectOntoAxis(normal);
}

Vector3 Vector3::ProjectOntoLine(const Vector3& from, const Vector3& to, bool clamped) const
{
	const Vector3 direction = to - from;
	const Real lengthSquared = direction.LengthSquared();
	Real factor = (*this - from).DotProduct(direction) / lengthSquared;

	if (clamped)
		factor = Clamp(factor, 0.0f, 1.0f);

	return from + direction * factor;
}

Real Vector3::DistanceToPoint(const Vector3& point) const
{ 
	return (*this - point).Length();
}

Real Vector3::DistanceToPlane(const Vector3& origin, const Vector3& normal) const
{ 
	return (*this - origin).ProjectOntoAxis(normal);
}

Vector3 Vector3::Orthogonalize(const Vector3& axis) const
{
	return axis.CrossProduct(*this).CrossProduct(axis).Normalized();
}

Vector3 Vector3::CrossProduct(const Vector3& rhs) const
{
	return Vector3(
		y_ * rhs.z_ - z_ * rhs.y_,
		z_ * rhs.x_ - x_ * rhs.z_,
		x_ * rhs.y_ - y_ * rhs.x_
		);
}

Vector3 Vector3::Abs() const
{ 
	return Vector3(FlagGG::Abs(x_), FlagGG::Abs(y_), FlagGG::Abs(z_));
}

Vector3 Vector3::Lerp(const Vector3& rhs, Real t) const
{ 
	return *this * (1.0f - t) + rhs * t;
}

bool Vector3::Equals(const Vector3& rhs) const
{
	return FlagGG::Equals(x_, rhs.x_) && FlagGG::Equals(y_, rhs.y_) && FlagGG::Equals(z_, rhs.z_);
}

Real Vector3::Angle(const Vector3& rhs) const
{
	return Acos(DotProduct(rhs) / (Length() * rhs.Length()));
}

bool Vector3::IsNaN() const
{
	return FlagGG::IsNaN(x_) || FlagGG::IsNaN(y_) || FlagGG::IsNaN(z_);
}

Vector3 Vector3::Normalized() const
{
	Real lenSquared = LengthSquared();
	if (!FlagGG::Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
	{
		Real invLen = 1.0f / Sqrt(lenSquared);
		return *this * invLen;
	}
			
	return *this;
}

String Vector3::ToString() const
{
	char tempBuffer[CONVERSION_BUFFER_LENGTH];
	sprintf(tempBuffer, "%d %d %d", x_, y_, z_);
	return String(tempBuffer);
}

}
