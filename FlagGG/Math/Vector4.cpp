#include "Math/Vector4.h"
#include "Math/Math.h"

#include <cstdio>

namespace FlagGG
{
	namespace Math
	{
		const Vector4 Vector4::ZERO;
		const Vector4 Vector4::ONE(1.0f, 1.0f, 1.0f, 1.0f);

		Vector4::Vector4() :
			x_(0.0f),
			y_(0.0f),
			z_(0.0f),
			w_(0.0f)
		{
		}

		Vector4::Vector4(const Vector3& vector, float w) :
			x_(vector.x_),
			y_(vector.y_),
			z_(vector.z_),
			w_(w)
		{
		}

		Vector4::Vector4(float x, float y, float z, float w) :
			x_(x),
			y_(y),
			z_(z),
			w_(w)
		{
		}

		Vector4::Vector4(const float* data) :
			x_(data[0]),
			y_(data[1]),
			z_(data[2]),
			w_(data[3])
		{
		}

		bool Vector4::operator ==(const Vector4& rhs) const
		{
			return x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_ && w_ == rhs.w_;
		}

		bool Vector4::operator !=(const Vector4& rhs) const
		{
			return x_ != rhs.x_ || y_ != rhs.y_ || z_ != rhs.z_ || w_ != rhs.w_;
		}

		Vector4 Vector4::operator +(const Vector4& rhs) const
		{
			return Vector4(x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_, w_ + rhs.w_);
		}

		Vector4 Vector4::operator -() const
		{
			return Vector4(-x_, -y_, -z_, -w_);
		}

		Vector4 Vector4::operator -(const Vector4& rhs) const
		{
			return Vector4(x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_, w_ - rhs.w_);
		}

		Vector4 Vector4::operator *(float rhs) const
		{
			return Vector4(x_ * rhs, y_ * rhs, z_ * rhs, w_ * rhs);
		}

		Vector4 Vector4::operator *(const Vector4& rhs) const
		{
			return Vector4(x_ * rhs.x_, y_ * rhs.y_, z_ * rhs.z_, w_ * rhs.w_);
		}

		Vector4 Vector4::operator /(float rhs) const
		{
			return Vector4(x_ / rhs, y_ / rhs, z_ / rhs, w_ / rhs);
		}

		Vector4 Vector4::operator /(const Vector4& rhs) const
		{
			return Vector4(x_ / rhs.x_, y_ / rhs.y_, z_ / rhs.z_, w_ / rhs.w_);
		}

		Vector4& Vector4::operator +=(const Vector4& rhs)
		{
			x_ += rhs.x_;
			y_ += rhs.y_;
			z_ += rhs.z_;
			w_ += rhs.w_;
			return *this;
		}

		Vector4& Vector4::operator -=(const Vector4& rhs)
		{
			x_ -= rhs.x_;
			y_ -= rhs.y_;
			z_ -= rhs.z_;
			w_ -= rhs.w_;
			return *this;
		}

		Vector4& Vector4::operator *=(float rhs)
		{
			x_ *= rhs;
			y_ *= rhs;
			z_ *= rhs;
			w_ *= rhs;
			return *this;
		}

		Vector4& Vector4::operator *=(const Vector4& rhs)
		{
			x_ *= rhs.x_;
			y_ *= rhs.y_;
			z_ *= rhs.z_;
			w_ *= rhs.w_;
			return *this;
		}

		Vector4& Vector4::operator /=(float rhs)
		{
			float invRhs = 1.0f / rhs;
			x_ *= invRhs;
			y_ *= invRhs;
			z_ *= invRhs;
			w_ *= invRhs;
			return *this;
		}

		Vector4& Vector4::operator /=(const Vector4& rhs)
		{
			x_ /= rhs.x_;
			y_ /= rhs.y_;
			z_ /= rhs.z_;
			w_ /= rhs.w_;
			return *this;
		}

		float Vector4::operator[](unsigned index) const
		{
			return (&x_)[index];
		}

		float& Vector4::operator[](unsigned index)
		{
			return (&x_)[index];
		}

		float Vector4::DotProduct(const Vector4& rhs) const
		{
			return x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_ + w_ * rhs.w_;
		}


		float Vector4::AbsDotProduct(const Vector4& rhs) const
		{
			return FlagGG::Math::Abs(x_ * rhs.x_) + FlagGG::Math::Abs(y_ * rhs.y_) + FlagGG::Math::Abs(z_ * rhs.z_) + FlagGG::Math::Abs(w_ * rhs.w_);
		}

		float Vector4::ProjectOntoAxis(const Vector3& axis) const
		{
			return DotProduct(Vector4(axis.Normalized(), 0.0f));
		}

		Vector4 Vector4::Abs() const { return Vector4(FlagGG::Math::Abs(x_), FlagGG::Math::Abs(y_), FlagGG::Math::Abs(z_), FlagGG::Math::Abs(w_)); }

		Vector4 Vector4::Lerp(const Vector4& rhs, float t) const { return *this * (1.0f - t) + rhs * t; }

		bool Vector4::Equals(const Vector4& rhs) const
		{
			return FlagGG::Math::Equals(x_, rhs.x_) && FlagGG::Math::Equals(y_, rhs.y_) && FlagGG::Math::Equals(z_, rhs.z_) && FlagGG::Math::Equals(w_, rhs.w_);
		}

		bool Vector4::IsNaN() const { return FlagGG::Math::IsNaN(x_) || FlagGG::Math::IsNaN(y_) || FlagGG::Math::IsNaN(z_) || FlagGG::Math::IsNaN(w_); }

		const float* Vector4::Data() const { return &x_; }

		unsigned Vector4::ToHash() const
		{
			unsigned hash = 37;
			hash = 37 * hash + FloatToRawIntBits(x_);
			hash = 37 * hash + FloatToRawIntBits(y_);
			hash = 37 * hash + FloatToRawIntBits(z_);
			hash = 37 * hash + FloatToRawIntBits(w_);

			return hash;
		}

		Container::String Vector4::ToString() const
		{
			char tempBuffer[Container::CONVERSION_BUFFER_LENGTH];
			sprintf(tempBuffer, "%g %g %g %g", x_, y_, z_, w_);
			return Container::String(tempBuffer);
		}
	}
}
