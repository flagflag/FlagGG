#pragma once

#include "Export.h"
#include "Define.h"
#include "Math/Math.h"
#include "Math/Rect.h"
#include "Math/Vector3.h"
#include "Math/Matrix3x4.h"
#include "Container/Str.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{
	namespace Math
	{
		class FlagGG_API BoundingBox
		{
		public:
			BoundingBox() noexcept :
				min_(F_INFINITY, F_INFINITY, F_INFINITY),
				max_(-F_INFINITY, -F_INFINITY, -F_INFINITY)
			{
			}

			BoundingBox(const BoundingBox& box) NOEXCEPT :
				min_(box.min_),
				max_(box.max_)
			{
			}

			explicit BoundingBox(const Rect& rect) NOEXCEPT :
				min_(Vector3(rect.min_, 0.0f)),
				max_(Vector3(rect.max_, 0.0f))
			{
			}

			BoundingBox(const Vector3& min, const Vector3& max) NOEXCEPT :
				min_(min),
				max_(max)
			{
			}

			BoundingBox(Real min, Real max) NOEXCEPT :
				min_(Vector3(min, min, min)),
				max_(Vector3(max, max, max))
			{
			}

			BoundingBox(const Vector3* vertices, unsigned count) :
				min_(F_INFINITY, F_INFINITY, F_INFINITY),
				max_(-F_INFINITY, -F_INFINITY, -F_INFINITY)
			{
				Define(vertices, count);
			}

			BoundingBox& operator =(const BoundingBox& rhs) NOEXCEPT
			{
				min_ = rhs.min_;
				max_ = rhs.max_;
				return *this;
			}

			BoundingBox& operator =(const Rect& rhs) NOEXCEPT
			{
				min_ = Vector3(rhs.min_, 0.0f);
				max_ = Vector3(rhs.max_, 0.0f);
				return *this;
			}

			bool operator ==(const BoundingBox& rhs) const { return (min_ == rhs.min_ && max_ == rhs.max_); }

			bool operator !=(const BoundingBox& rhs) const { return (min_ != rhs.min_ || max_ != rhs.max_); }

			void Define(const BoundingBox& box)
			{
				Define(box.min_, box.max_);
			}

			void Define(const Rect& rect)
			{
				Define(Vector3(rect.min_, 0.0f), Vector3(rect.max_, 0.0f));
			}

			void Define(const Vector3& min, const Vector3& max)
			{
				min_ = min;
				max_ = max;
			}

			void Define(Real min, Real max)
			{
				min_ = Vector3(min, min, min);
				max_ = Vector3(max, max, max);
			}

			void Define(const Vector3& point)
			{
				min_ = max_ = point;
			}

			void Merge(const Vector3& point)
			{
				if (point.x_ < min_.x_)
					min_.x_ = point.x_;
				if (point.y_ < min_.y_)
					min_.y_ = point.y_;
				if (point.z_ < min_.z_)
					min_.z_ = point.z_;
				if (point.x_ > max_.x_)
					max_.x_ = point.x_;
				if (point.y_ > max_.y_)
					max_.y_ = point.y_;
				if (point.z_ > max_.z_)
					max_.z_ = point.z_;
			}

			void Merge(const BoundingBox& box)
			{
				if (box.min_.x_ < min_.x_)
					min_.x_ = box.min_.x_;
				if (box.min_.y_ < min_.y_)
					min_.y_ = box.min_.y_;
				if (box.min_.z_ < min_.z_)
					min_.z_ = box.min_.z_;
				if (box.max_.x_ > max_.x_)
					max_.x_ = box.max_.x_;
				if (box.max_.y_ > max_.y_)
					max_.y_ = box.max_.y_;
				if (box.max_.z_ > max_.z_)
					max_.z_ = box.max_.z_;
			}

			void Define(const Vector3* vertices, unsigned count);
			void Merge(const Vector3* vertices, unsigned count);
			void Clip(const BoundingBox& box);
			void Transform(const Matrix3& transform);
			void Transform(const Matrix3x4& transform);

			void Clear()
			{
				min_ = Vector3(F_INFINITY, F_INFINITY, F_INFINITY);
				max_ = Vector3(-F_INFINITY, -F_INFINITY, -F_INFINITY);
			}

			bool Defined() const
			{
				return min_.x_ != F_INFINITY;
			}

			Vector3 Center() const { return (max_ + min_) * 0.5f; }
			Vector3 Size() const { return max_ - min_; }
			Vector3 HalfSize() const { return (max_ - min_) * 0.5f; }

			BoundingBox Transformed(const Matrix3& transform) const;
			BoundingBox Transformed(const Matrix3x4& transform) const;
			Rect Projected(const Matrix4& projection) const;
			Real DistanceToPoint(const Vector3& point) const;

			Intersection IsInside(const Vector3& point) const
			{
				if (point.x_ < min_.x_ || point.x_ > max_.x_ || point.y_ < min_.y_ || point.y_ > max_.y_ ||
					point.z_ < min_.z_ || point.z_ > max_.z_)
					return OUTSIDE;
				else
					return INSIDE;
			}

			Intersection IsInside(const BoundingBox& box) const
			{
				if (box.max_.x_ < min_.x_ || box.min_.x_ > max_.x_ || box.max_.y_ < min_.y_ || box.min_.y_ > max_.y_ ||
					box.max_.z_ < min_.z_ || box.min_.z_ > max_.z_)
					return OUTSIDE;
				else if (box.min_.x_ < min_.x_ || box.max_.x_ > max_.x_ || box.min_.y_ < min_.y_ || box.max_.y_ > max_.y_ ||
					box.min_.z_ < min_.z_ || box.max_.z_ > max_.z_)
					return INTERSECTS;
				else
					return INSIDE;
			}

			Intersection IsInsideFast(const BoundingBox& box) const
			{
				if (box.max_.x_ < min_.x_ || box.min_.x_ > max_.x_ || box.max_.y_ < min_.y_ || box.min_.y_ > max_.y_ ||
					box.max_.z_ < min_.z_ || box.min_.z_ > max_.z_)
					return OUTSIDE;
				else
					return INSIDE;
			}

			Container::String ToString() const;

			Vector3 min_;
			Real dummyMin_{}; // 暂时没用

			Vector3 max_;
			Real dummyMax_{}; // 暂时没用
		};
	}
}
