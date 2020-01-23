#pragma once

#include "Export.h"
#include "Define.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Matrix3x4.h"
#include "BoundingBox.h"

namespace FlagGG
{
	namespace Math
	{
		class FlagGG_API Ray
		{
		public:
			Ray() NOEXCEPT = default;

			Ray(const Vector3& origin, const Vector3& direction) NOEXCEPT
			{
				Define(origin, direction);
			}

			Ray(const Ray& ray) NOEXCEPT = default;

			Ray& operator =(const Ray& rhs) NOEXCEPT = default;

			bool operator ==(const Ray& rhs) const { return origin_ == rhs.origin_ && direction_ == rhs.direction_; }

			bool operator !=(const Ray& rhs) const { return origin_ != rhs.origin_ || direction_ != rhs.direction_; }

			void Define(const Vector3& origin, const Vector3& direction)
			{
				origin_ = origin;
				direction_ = direction.Normalized();
			}

			Vector3 Project(const Vector3& point) const
			{
				Vector3 offset = point - origin_;
				return origin_ + offset.DotProduct(direction_) * direction_;
			}

			Real Distance(const Vector3& point) const
			{
				Vector3 projected = Project(point);
				return (point - projected).Length();
			}

			Vector3 ClosestPoint(const Ray& ray) const;
			// 和BoundingBox三个面的距离，如果射线远点在BoundingBox内部，则距离为0
			Real HitDistance(const BoundingBox& box) const;
			Real HitDistance(const Vector3& v0, const Vector3& v1, const Vector3& v2, Vector3* outNormal = nullptr, Vector3* outBary = nullptr) const;
			Real HitDistance
			(const void* vertexData, unsigned vertexStride, unsigned vertexStart, unsigned vertexCount, Vector3* outNormal = nullptr,
				Vector2* outUV = nullptr, unsigned uvOffset = 0) const;
			Real HitDistance(const void* vertexData, unsigned vertexStride, const void* indexData, unsigned indexSize, unsigned indexStart,
				unsigned indexCount, Vector3* outNormal = nullptr, Vector2* outUV = nullptr, unsigned uvOffset = 0) const;
			bool InsideGeometry(const void* vertexData, unsigned vertexSize, unsigned vertexStart, unsigned vertexCount) const;
			bool InsideGeometry(const void* vertexData, unsigned vertexSize, const void* indexData, unsigned indexSize, unsigned indexStart,
				unsigned indexCount) const;
			Ray Transformed(const Matrix3x4& transform) const;

			Vector3 origin_;
			Vector3 direction_;
		};

	}
}
