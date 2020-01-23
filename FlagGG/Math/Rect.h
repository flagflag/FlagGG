#pragma once

#include "Export.h"
#include "Define.h"
#include "Math/Vector2.h"
#include "Math/Math.h"

namespace FlagGG
{
	namespace Math
	{
		class FlagGG_API Rect
		{
		public:
			Rect() NOEXCEPT :
				min_(F_INFINITY, F_INFINITY),
				max_(-F_INFINITY, -F_INFINITY)
			{
			}

			Rect(const Vector2& min, const Vector2& max) NOEXCEPT :
			min_(min),
				max_(max)
			{
			}

			Rect(Real left, Real top, Real right, Real bottom) NOEXCEPT :
			min_(left, top),
				max_(right, bottom)
			{
			}

			explicit Rect(const Real* data) NOEXCEPT :
			min_(data[0], data[1]),
				max_(data[2], data[3])
			{
			}

			Rect(const Rect& rect) NOEXCEPT = default;

			Rect& operator =(const Rect& rhs) NOEXCEPT = default;

			bool operator ==(const Rect& rhs) const { return min_ == rhs.min_ && max_ == rhs.max_; }

			bool operator !=(const Rect& rhs) const { return min_ != rhs.min_ || max_ != rhs.max_; }

			Rect& operator +=(const Rect& rhs)
			{
				min_ += rhs.min_;
				max_ += rhs.max_;
				return *this;
			}

			Rect& operator -=(const Rect& rhs)
			{
				min_ -= rhs.min_;
				max_ -= rhs.max_;
				return *this;
			}

			Rect& operator /=(Real value)
			{
				min_ /= value;
				max_ /= value;
				return *this;
			}

			Rect& operator *=(Real value)
			{
				min_ *= value;
				max_ *= value;
				return *this;
			}

			Rect operator /(Real value) const
			{
				return Rect(min_ / value, max_ / value);
			}

			Rect operator *(Real value) const
			{
				return Rect(min_ * value, max_ * value);
			}

			Rect operator +(const Rect& rhs) const
			{
				return Rect(min_ + rhs.min_, max_ + rhs.max_);
			}

			Rect operator -(const Rect& rhs) const
			{
				return Rect(min_ - rhs.min_, max_ - rhs.max_);
			}

			void Define(const Rect& rect)
			{
				min_ = rect.min_;
				max_ = rect.max_;
			}

			void Define(const Vector2& min, const Vector2& max)
			{
				min_ = min;
				max_ = max;
			}

			void Define(const Vector2& point)
			{
				min_ = max_ = point;
			}

			void Merge(const Vector2& point)
			{
				if (point.x_ < min_.x_)
					min_.x_ = point.x_;
				if (point.x_ > max_.x_)
					max_.x_ = point.x_;
				if (point.y_ < min_.y_)
					min_.y_ = point.y_;
				if (point.y_ > max_.y_)
					max_.y_ = point.y_;
			}

			void Merge(const Rect& rect)
			{
				if (rect.min_.x_ < min_.x_)
					min_.x_ = rect.min_.x_;
				if (rect.min_.y_ < min_.y_)
					min_.y_ = rect.min_.y_;
				if (rect.max_.x_ > max_.x_)
					max_.x_ = rect.max_.x_;
				if (rect.max_.y_ > max_.y_)
					max_.y_ = rect.max_.y_;
			}

			void Clear()
			{
				min_ = Vector2(F_INFINITY, F_INFINITY);
				max_ = Vector2(-F_INFINITY, -F_INFINITY);
			}

			void Clip(const Rect& rect);

			bool Defined() const
			{
				return min_.x_ != F_INFINITY;
			}

			Vector2 Center() const { return (max_ + min_) * 0.5f; }

			Vector2 Size() const { return max_ - min_; }

			Vector2 HalfSize() const { return (max_ - min_) * 0.5f; }

			bool Equals(const Rect& rhs) const { return min_.Equals(rhs.min_) && max_.Equals(rhs.max_); }

			Intersection IsInside(const Vector2& point) const
			{
				if (point.x_ < min_.x_ || point.y_ < min_.y_ || point.x_ > max_.x_ || point.y_ > max_.y_)
					return OUTSIDE;
				else
					return INSIDE;
			}

			Intersection IsInside(const Rect& rect) const
			{
				if (rect.max_.x_ < min_.x_ || rect.min_.x_ > max_.x_ || rect.max_.y_ < min_.y_ || rect.min_.y_ > max_.y_)
					return OUTSIDE;
				else if (rect.min_.x_ < min_.x_ || rect.max_.x_ > max_.x_ || rect.min_.y_ < min_.y_ || rect.max_.y_ > max_.y_)
					return INTERSECTS;
				else
					return INSIDE;
			}

			const void* Data() const { return &min_.x_; }

			Container::String ToString() const;

			Vector2 Min() const { return min_; }

			Vector2 Max() const { return max_; }

			Real Left() const { return min_.x_; }

			Real Top() const { return min_.y_; }

			Real Right() const { return max_.x_; }

			Real Bottom() const { return max_.y_; }

			Vector2 min_;
			Vector2 max_;

			static const Rect FULL;
			static const Rect POSITIVE;
			static const Rect ZERO;
		};

		class FlagGG_API IntRect
		{
		public:
			IntRect() NOEXCEPT :
				left_(0),
				top_(0),
				right_(0),
				bottom_(0)
			{
			}

			IntRect(const IntVector2& min, const IntVector2& max) NOEXCEPT :
			left_(min.x_),
				top_(min.y_),
				right_(max.x_),
				bottom_(max.y_)
			{
			}

			IntRect(int left, int top, int right, int bottom) NOEXCEPT :
			left_(left),
				top_(top),
				right_(right),
				bottom_(bottom)
			{
			}

			explicit IntRect(const int* data) NOEXCEPT :
			left_(data[0]),
				top_(data[1]),
				right_(data[2]),
				bottom_(data[3])
			{
			}

			bool operator ==(const IntRect& rhs) const
			{
				return left_ == rhs.left_ && top_ == rhs.top_ && right_ == rhs.right_ && bottom_ == rhs.bottom_;
			}

			bool operator !=(const IntRect& rhs) const
			{
				return left_ != rhs.left_ || top_ != rhs.top_ || right_ != rhs.right_ || bottom_ != rhs.bottom_;
			}

			IntRect& operator +=(const IntRect& rhs)
			{
				left_ += rhs.left_;
				top_ += rhs.top_;
				right_ += rhs.right_;
				bottom_ += rhs.bottom_;
				return *this;
			}

			IntRect& operator -=(const IntRect& rhs)
			{
				left_ -= rhs.left_;
				top_ -= rhs.top_;
				right_ -= rhs.right_;
				bottom_ -= rhs.bottom_;
				return *this;
			}

			IntRect& operator /=(Real value)
			{
				left_ = static_cast<int>(left_ / value);
				top_ = static_cast<int>(top_ / value);
				right_ = static_cast<int>(right_ / value);
				bottom_ = static_cast<int>(bottom_ / value);
				return *this;
			}

			IntRect& operator *=(Real value)
			{
				left_ = static_cast<int>(left_ * value);
				top_ = static_cast<int>(top_ * value);
				right_ = static_cast<int>(right_ * value);
				bottom_ = static_cast<int>(bottom_ * value);
				return *this;
			}

			IntRect operator /(Real value) const
			{
				return{
					static_cast<int>(left_ / value), static_cast<int>(top_ / value),
					static_cast<int>(right_ / value), static_cast<int>(bottom_ / value)
				};
			}

			IntRect operator *(Real value) const
			{
				return{
					static_cast<int>(left_ * value), static_cast<int>(top_ * value),
					static_cast<int>(right_ * value), static_cast<int>(bottom_ * value)
				};
			}

			IntRect operator +(const IntRect& rhs) const
			{
				return{
					left_ + rhs.left_, top_ + rhs.top_,
					right_ + rhs.right_, bottom_ + rhs.bottom_
				};
			}

			IntRect operator -(const IntRect& rhs) const
			{
				return{
					left_ - rhs.left_, top_ - rhs.top_,
					right_ - rhs.right_, bottom_ - rhs.bottom_
				};
			}

			IntVector2 Size() const { return IntVector2(Width(), Height()); }

			int Width() const { return right_ - left_; }

			int Height() const { return bottom_ - top_; }

			Intersection IsInside(const IntVector2& point) const
			{
				if (point.x_ < left_ || point.y_ < top_ || point.x_ >= right_ || point.y_ >= bottom_)
					return OUTSIDE;
				else
					return INSIDE;
			}

			void Clip(const IntRect& rect);

			void Merge(const IntRect& rect);

			const int* Data() const { return &left_; }

			Container::String ToString() const;

			IntVector2 Min() const { return{ left_, top_ }; }

			IntVector2 Max() const { return{ right_, bottom_ }; }

			int Left() const { return left_; }

			int Top() const { return top_; }

			int Right() const { return right_; }

			int Bottom() const { return bottom_; }

			int left_;
			int top_;
			int right_;
			int bottom_;

			static const IntRect ZERO;
		};

	}
}
