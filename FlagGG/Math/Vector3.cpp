#include "Vector3.h"
#include "Math/Math.h"

namespace FlagGG
{
	namespace Math
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

		Vector3::Vector3(float x, float y, float z) :
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

		Vector3 Vector3::operator *(float rhs) const
		{
			return Vector3(x_ * rhs, y_ * rhs, z_ * rhs);
		}

		Vector3 Vector3::operator *(const Vector3& rhs) const
		{ 
			return Vector3(x_ * rhs.x_, y_ * rhs.y_, z_ * rhs.z_);
		}

		Vector3 Vector3::operator /(float rhs) const
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

		Vector3& Vector3::operator *=(float rhs)
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

		Vector3& Vector3::operator /=(float rhs)
		{
			float invRhs = 1.0f / rhs;
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
			float lenSquared = LengthSquared();
			if (!Math::Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
			{
				float invLen = 1.0f / Math::Sqrt(lenSquared);
				x_ *= invLen;
				y_ *= invLen;
				z_ *= invLen;
			}
		}

		float Vector3::Length() const
		{ 
			return Math::Sqrt(x_ * x_ + y_ * y_ + z_ * z_);
		}

		float Vector3::LengthSquared() const 
		{ 
			return x_ * x_ + y_ * y_ + z_ * z_; 
		}

		float Vector3::DotProduct(const Vector3& rhs) const
		{ 
			return x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_;
		}

		float Vector3::AbsDotProduct(const Vector3& rhs) const
		{
			return Math::Abs(x_ * rhs.x_) + Math::Abs(y_ * rhs.y_) + Math::Abs(z_ * rhs.z_);
		}

		float Vector3::ProjectOntoAxis(const Vector3& axis) const
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
			const float lengthSquared = direction.LengthSquared();
			float factor = (*this - from).DotProduct(direction) / lengthSquared;

			if (clamped)
				factor = Math::Clamp(factor, 0.0f, 1.0f);

			return from + direction * factor;
		}

		float Vector3::DistanceToPoint(const Vector3& point) const
		{ 
			return (*this - point).Length();
		}

		float Vector3::DistanceToPlane(const Vector3& origin, const Vector3& normal) const
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
			return Vector3(Math::Abs(x_), Math::Abs(y_), Math::Abs(z_));
		}

		Vector3 Vector3::Lerp(const Vector3& rhs, float t) const
		{ 
			return *this * (1.0f - t) + rhs * t;
		}

		bool Vector3::Equals(const Vector3& rhs) const
		{
			return Math::Equals(x_, rhs.x_) && Math::Equals(y_, rhs.y_) && Math::Equals(z_, rhs.z_);
		}

		float Vector3::Angle(const Vector3& rhs) const
		{
			return Math::Acos(DotProduct(rhs) / (Length() * rhs.Length()));
		}

		bool Vector3::IsNaN() const
		{
			return Math::IsNaN(x_) || Math::IsNaN(y_) || Math::IsNaN(z_);
		}

		Vector3 Vector3::Normalized() const
		{
			float lenSquared = LengthSquared();
			if (!Math::Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
			{
				float invLen = 1.0f / Math::Sqrt(lenSquared);
				return *this * invLen;
			}
			
			return *this;
		}
	}
}
