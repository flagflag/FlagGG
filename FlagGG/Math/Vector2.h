#ifndef __VECTOR2__
#define __VECTOR2__

#include "Export.h"
#include "Container/Str.h"
#include "Math/Math.h"

#include <math.h>

namespace FlagGG
{
	namespace Math
	{
		class FlagGG_API IntVector2
		{
		public:
			IntVector2() NOEXCEPT :
				x_(0),
				y_(0)
			{
			}

			IntVector2(int x, int y) NOEXCEPT :
			x_(x),
				y_(y)
			{
			}

			explicit IntVector2(const int* data) NOEXCEPT :
			x_(data[0]),
				y_(data[1])
			{
			}

			explicit IntVector2(const float* data) :
				x_((int)data[0]),
				y_((int)data[1])
			{
			}

			IntVector2(const IntVector2& rhs) NOEXCEPT = default;

			IntVector2& operator =(const IntVector2& rhs) NOEXCEPT = default;

			bool operator ==(const IntVector2& rhs) const { return x_ == rhs.x_ && y_ == rhs.y_; }

			bool operator !=(const IntVector2& rhs) const { return x_ != rhs.x_ || y_ != rhs.y_; }

			IntVector2 operator +(const IntVector2& rhs) const { return IntVector2(x_ + rhs.x_, y_ + rhs.y_); }

			IntVector2 operator -() const { return IntVector2(-x_, -y_); }

			IntVector2 operator -(const IntVector2& rhs) const { return IntVector2(x_ - rhs.x_, y_ - rhs.y_); }

			IntVector2 operator *(int rhs) const { return IntVector2(x_ * rhs, y_ * rhs); }

			IntVector2 operator *(const IntVector2& rhs) const { return IntVector2(x_ * rhs.x_, y_ * rhs.y_); }

			IntVector2 operator /(int rhs) const { return IntVector2(x_ / rhs, y_ / rhs); }

			IntVector2 operator /(const IntVector2& rhs) const { return IntVector2(x_ / rhs.x_, y_ / rhs.y_); }

			IntVector2& operator +=(const IntVector2& rhs)
			{
				x_ += rhs.x_;
				y_ += rhs.y_;
				return *this;
			}

			IntVector2& operator -=(const IntVector2& rhs)
			{
				x_ -= rhs.x_;
				y_ -= rhs.y_;
				return *this;
			}

			IntVector2& operator *=(int rhs)
			{
				x_ *= rhs;
				y_ *= rhs;
				return *this;
			}

			IntVector2& operator *=(const IntVector2& rhs)
			{
				x_ *= rhs.x_;
				y_ *= rhs.y_;
				return *this;
			}

			IntVector2& operator /=(int rhs)
			{
				x_ /= rhs;
				y_ /= rhs;
				return *this;
			}

			IntVector2& operator /=(const IntVector2& rhs)
			{
				x_ /= rhs.x_;
				y_ /= rhs.y_;
				return *this;
			}

			const int* Data() const { return &x_; }

			Container::String ToString() const;

			unsigned ToHash() const { return (unsigned)x_ * 31 + (unsigned)y_; }

			float Length() const { return sqrtf((float)(x_ * x_ + y_ * y_)); }

			int x_;
			int y_;

			static const IntVector2 ZERO;
			static const IntVector2 LEFT;
			static const IntVector2 RIGHT;
			static const IntVector2 UP;
			static const IntVector2 DOWN;
			static const IntVector2 ONE;
		};

		class FlagGG_API Vector2
		{
		public:
			Vector2() NOEXCEPT :
				x_(0.0f),
				y_(0.0f)
			{
			}

			Vector2(const Vector2& vector) NOEXCEPT = default;

			explicit Vector2(const IntVector2& vector) NOEXCEPT :
			x_((float)vector.x_),
				y_((float)vector.y_)
			{
			}

			Vector2(float x, float y) NOEXCEPT :
			x_(x),
				y_(y)
			{
			}

			explicit Vector2(const float* data) NOEXCEPT :
			x_(data[0]),
				y_(data[1])
			{
			}

			Vector2& operator =(const Vector2& rhs) NOEXCEPT = default;

			bool operator ==(const Vector2& rhs) const { return x_ == rhs.x_ && y_ == rhs.y_; }

			bool operator !=(const Vector2& rhs) const { return x_ != rhs.x_ || y_ != rhs.y_; }

			Vector2 operator +(const Vector2& rhs) const { return Vector2(x_ + rhs.x_, y_ + rhs.y_); }

			Vector2 operator -() const { return Vector2(-x_, -y_); }

			Vector2 operator -(const Vector2& rhs) const { return Vector2(x_ - rhs.x_, y_ - rhs.y_); }

			Vector2 operator *(float rhs) const { return Vector2(x_ * rhs, y_ * rhs); }

			Vector2 operator *(const Vector2& rhs) const { return Vector2(x_ * rhs.x_, y_ * rhs.y_); }

			Vector2 operator /(float rhs) const { return Vector2(x_ / rhs, y_ / rhs); }

			Vector2 operator /(const Vector2& rhs) const { return Vector2(x_ / rhs.x_, y_ / rhs.y_); }

			Vector2& operator +=(const Vector2& rhs)
			{
				x_ += rhs.x_;
				y_ += rhs.y_;
				return *this;
			}

			Vector2& operator -=(const Vector2& rhs)
			{
				x_ -= rhs.x_;
				y_ -= rhs.y_;
				return *this;
			}

			Vector2& operator *=(float rhs)
			{
				x_ *= rhs;
				y_ *= rhs;
				return *this;
			}

			Vector2& operator *=(const Vector2& rhs)
			{
				x_ *= rhs.x_;
				y_ *= rhs.y_;
				return *this;
			}

			Vector2& operator /=(float rhs)
			{
				float invRhs = 1.0f / rhs;
				x_ *= invRhs;
				y_ *= invRhs;
				return *this;
			}

			Vector2& operator /=(const Vector2& rhs)
			{
				x_ /= rhs.x_;
				y_ /= rhs.y_;
				return *this;
			}

			void Normalize()
			{
				float lenSquared = LengthSquared();
				if (!Math::Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
				{
					float invLen = 1.0f / sqrtf(lenSquared);
					x_ *= invLen;
					y_ *= invLen;
				}
			}

			float Length() const { return sqrtf(x_ * x_ + y_ * y_); }

			float LengthSquared() const { return x_ * x_ + y_ * y_; }

			float DotProduct(const Vector2& rhs) const { return x_ * rhs.x_ + y_ * rhs.y_; }

			float AbsDotProduct(const Vector2& rhs) const { return Math::Abs(x_ * rhs.x_) + Math::Abs(y_ * rhs.y_); }

			float ProjectOntoAxis(const Vector2& axis) const { return DotProduct(axis.Normalized()); }

			float Angle(const Vector2& rhs) const { return Math::Acos(DotProduct(rhs) / (Length() * rhs.Length())); }

			Vector2 Abs() const { return Vector2(Math::Abs(x_), Math::Abs(y_)); }

			Vector2 Lerp(const Vector2& rhs, float t) const { return *this * (1.0f - t) + rhs * t; }

			bool Equals(const Vector2& rhs) const { return Math::Equals(x_, rhs.x_) && Math::Equals(y_, rhs.y_); }

			bool IsNaN() const { return Math::IsNaN(x_) || Math::IsNaN(y_); }

			Vector2 Normalized() const
			{
				float lenSquared = LengthSquared();
				if (!Math::Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
				{
					float invLen = 1.0f / sqrtf(lenSquared);
					return *this * invLen;
				}
				else
					return *this;
			}

			const float* Data() const { return &x_; }

			Container::String ToString() const;

			float x_;
			float y_;

			static const Vector2 ZERO;
			static const Vector2 LEFT;
			static const Vector2 RIGHT;
			static const Vector2 UP;
			static const Vector2 DOWN;
			static const Vector2 ONE;
		};
	}
}

#endif