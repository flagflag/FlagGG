#include "Math/Ray.h"
#include "Math/Math.h"
#include "Math/Vector4.h"

namespace FlagGG
{

Vector3 Ray::ClosestPoint(const Ray& ray) const
{
	// 参考： http://paulbourke.net/geometry/lineline3d/
	Vector3 p13 = origin_ - ray.origin_;
	Vector3 p43 = ray.direction_;
	Vector3 p21 = direction_;

	Real d1343 = p13.DotProduct(p43);
	Real d4321 = p43.DotProduct(p21);
	Real d1321 = p13.DotProduct(p21);
	Real d4343 = p43.DotProduct(p43);
	Real d2121 = p21.DotProduct(p21);

	Real d = d2121 * d4343 - d4321 * d4321;
	if (Abs(d) < EPS)
		return origin_;
	Real n = d1343 * d4321 - d1321 * d4343;
	Real a = n / d;

	return origin_ + a * direction_;
}

Real Ray::HitDistance(const BoundingBox& box) const
{
	if (!box.Defined())
		return F_INFINITY;

	if (box.IsInside(origin_))
		return 0.0f;

	Real dist = F_INFINITY;

	// X方向的面
	if (origin_.x_ < box.min_.x_ && direction_.x_ > 0.0f)
	{
		Real x = (box.min_.x_ - origin_.x_) / direction_.x_;
		if (x < dist)
		{
			Vector3 point = origin_ + x * direction_;
			if (point.y_ >= box.min_.y_ && point.y_ <= box.max_.y_ && point.z_ >= box.min_.z_ && point.z_ <= box.max_.z_)
				dist = x;
		}
	}
	if (origin_.x_ > box.max_.x_ && direction_.x_ < 0.0f)
	{
		Real x = (box.max_.x_ - origin_.x_) / direction_.x_;
		if (x < dist)
		{
			Vector3 point = origin_ + x * direction_;
			if (point.y_ >= box.min_.y_ && point.y_ <= box.max_.y_ && point.z_ >= box.min_.z_ && point.z_ <= box.max_.z_)
				dist = x;
		}
	}
	// Y方向的面
	if (origin_.y_ < box.min_.y_ && direction_.y_ > 0.0f)
	{
		Real x = (box.min_.y_ - origin_.y_) / direction_.y_;
		if (x < dist)
		{
			Vector3 point = origin_ + x * direction_;
			if (point.x_ >= box.min_.x_ && point.x_ <= box.max_.x_ && point.z_ >= box.min_.z_ && point.z_ <= box.max_.z_)
				dist = x;
		}
	}
	if (origin_.y_ > box.max_.y_ && direction_.y_ < 0.0f)
	{
		Real x = (box.max_.y_ - origin_.y_) / direction_.y_;
		if (x < dist)
		{
			Vector3 point = origin_ + x * direction_;
			if (point.x_ >= box.min_.x_ && point.x_ <= box.max_.x_ && point.z_ >= box.min_.z_ && point.z_ <= box.max_.z_)
				dist = x;
		}
	}
	// Z方向的面
	if (origin_.z_ < box.min_.z_ && direction_.z_ > 0.0f)
	{
		Real x = (box.min_.z_ - origin_.z_) / direction_.z_;
		if (x < dist)
		{
			Vector3 point = origin_ + x * direction_;
			if (point.x_ >= box.min_.x_ && point.x_ <= box.max_.x_ && point.y_ >= box.min_.y_ && point.y_ <= box.max_.y_)
				dist = x;
		}
	}
	if (origin_.z_ > box.max_.z_ && direction_.z_ < 0.0f)
	{
		Real x = (box.max_.z_ - origin_.z_) / direction_.z_;
		if (x < dist)
		{
			Vector3 point = origin_ + x * direction_;
			if (point.x_ >= box.min_.x_ && point.x_ <= box.max_.x_ && point.y_ >= box.min_.y_ && point.y_ <= box.max_.y_)
				dist = x;
		}
	}

	return dist;
}

Real Ray::HitDistance(const Vector3& v0, const Vector3& v1, const Vector3& v2, Vector3* outNormal, Vector3* outBary) const
{
	// Based on Fast, Minimum Storage Ray/Triangle Intersection by Möller & Trumbore
	// http://www.graphics.cornell.edu/pubs/1997/MT97.pdf
	// Calculate edge vectors
	Vector3 edge1(v1 - v0);
	Vector3 edge2(v2 - v0);

	// Calculate determinant & check backfacing
	Vector3 p(direction_.CrossProduct(edge2));
	Real det = edge1.DotProduct(p);
	if (det >= EPS)
	{
		// Calculate u & v parameters and test
		Vector3 t(origin_ - v0);
		Real u = t.DotProduct(p);
		if (u >= 0.0f && u <= det)
		{
			Vector3 q(t.CrossProduct(edge1));
			Real v = direction_.DotProduct(q);
			if (v >= 0.0f && u + v <= det)
			{
				Real distance = edge2.DotProduct(q) / det;
				// Discard hits behind the ray
				if (distance >= 0.0f)
				{
					// There is an intersection, so calculate distance & optional normal
					if (outNormal)
						*outNormal = edge1.CrossProduct(edge2);
					if (outBary)
						*outBary = Vector3(1 - (u / det) - (v / det), u / det, v / det);

					return distance;
				}
			}
		}
	}

	return F_INFINITY;
}

Real Ray::HitDistance(const void* vertexData, unsigned vertexStride, unsigned vertexStart, unsigned vertexCount,
	Vector3* outNormal, Vector2* outUV, unsigned uvOffset) const
{
	Real nearest = F_INFINITY;
	const unsigned char* vertices = ((const unsigned char*)vertexData) + vertexStart * vertexStride;
	unsigned index = 0, nearestIdx = F_MAX_UNSIGNED;
	Vector3 barycentric;
	Vector3* outBary = outUV ? &barycentric : nullptr;

	while (index + 2 < vertexCount)
	{
		const Vector3& v0 = *((const Vector3*)(&vertices[index * vertexStride]));
		const Vector3& v1 = *((const Vector3*)(&vertices[(index + 1) * vertexStride]));
		const Vector3& v2 = *((const Vector3*)(&vertices[(index + 2) * vertexStride]));
		Real distance = HitDistance(v0, v1, v2, outNormal, outBary);
		if (distance < nearest)
		{
			nearestIdx = index;
			nearest = distance;
		}
		index += 3;
	}

	if (outUV)
	{
		if (nearestIdx == F_MAX_UNSIGNED)
			*outUV = Vector2::ZERO;
		else
		{
			const Vector2& uv0 = *((const Vector2*)(&vertices[uvOffset + nearestIdx * vertexStride]));
			const Vector2& uv1 = *((const Vector2*)(&vertices[uvOffset + (nearestIdx + 1) * vertexStride]));
			const Vector2& uv2 = *((const Vector2*)(&vertices[uvOffset + (nearestIdx + 2) * vertexStride]));
			*outUV = Vector2(uv0.x_ * barycentric.x_ + uv1.x_ * barycentric.y_ + uv2.x_ * barycentric.z_,
				uv0.y_ * barycentric.x_ + uv1.y_ * barycentric.y_ + uv2.y_ * barycentric.z_);
		}
	}

	return nearest;
}

Real Ray::HitDistance(const void* vertexData, unsigned vertexStride, const void* indexData, unsigned indexSize,
	unsigned indexStart, unsigned indexCount, Vector3* outNormal, Vector2* outUV, unsigned uvOffset) const
{
	Real nearest = F_INFINITY;
	const auto* vertices = (const unsigned char*)vertexData;
	Vector3 barycentric;
	Vector3* outBary = outUV ? &barycentric : nullptr;

	// 16-bit indices
	if (indexSize == sizeof(unsigned short))
	{
		const unsigned short* indices = ((const unsigned short*)indexData) + indexStart;
		const unsigned short* indicesEnd = indices + indexCount;
		const unsigned short* nearestIndices = nullptr;

		while (indices < indicesEnd)
		{
			const Vector3& v0 = *((const Vector3*)(&vertices[indices[0] * vertexStride]));
			const Vector3& v1 = *((const Vector3*)(&vertices[indices[1] * vertexStride]));
			const Vector3& v2 = *((const Vector3*)(&vertices[indices[2] * vertexStride]));
			Real distance = HitDistance(v0, v1, v2, outNormal, outBary);
			if (distance < nearest)
			{
				nearestIndices = indices;
				nearest = distance;
			}
			indices += 3;
		}

		if (outUV)
		{
			if (nearestIndices == nullptr)
				*outUV = Vector2::ZERO;
			else
			{
				const Vector2& uv0 = *((const Vector2*)(&vertices[uvOffset + nearestIndices[0] * vertexStride]));
				const Vector2& uv1 = *((const Vector2*)(&vertices[uvOffset + nearestIndices[1] * vertexStride]));
				const Vector2& uv2 = *((const Vector2*)(&vertices[uvOffset + nearestIndices[2] * vertexStride]));
				*outUV = Vector2(uv0.x_ * barycentric.x_ + uv1.x_ * barycentric.y_ + uv2.x_ * barycentric.z_,
					uv0.y_ * barycentric.x_ + uv1.y_ * barycentric.y_ + uv2.y_ * barycentric.z_);
			}
		}
	}
	// 32-bit indices
	else
	{
		const unsigned* indices = ((const unsigned*)indexData) + indexStart;
		const unsigned* indicesEnd = indices + indexCount;
		const unsigned* nearestIndices = nullptr;

		while (indices < indicesEnd)
		{
			const Vector3& v0 = *((const Vector3*)(&vertices[indices[0] * vertexStride]));
			const Vector3& v1 = *((const Vector3*)(&vertices[indices[1] * vertexStride]));
			const Vector3& v2 = *((const Vector3*)(&vertices[indices[2] * vertexStride]));
			Real distance = HitDistance(v0, v1, v2, outNormal, outBary);
			if (distance < nearest)
			{
				nearestIndices = indices;
				nearest = distance;
			}
			indices += 3;
		}

		if (outUV)
		{
			if (nearestIndices == nullptr)
				*outUV = Vector2::ZERO;
			else
			{
				const Vector2& uv0 = *((const Vector2*)(&vertices[uvOffset + nearestIndices[0] * vertexStride]));
				const Vector2& uv1 = *((const Vector2*)(&vertices[uvOffset + nearestIndices[1] * vertexStride]));
				const Vector2& uv2 = *((const Vector2*)(&vertices[uvOffset + nearestIndices[2] * vertexStride]));
				*outUV = Vector2(uv0.x_ * barycentric.x_ + uv1.x_ * barycentric.y_ + uv2.x_ * barycentric.z_,
					uv0.y_ * barycentric.x_ + uv1.y_ * barycentric.y_ + uv2.y_ * barycentric.z_);
			}
		}
	}

	return nearest;
}

bool Ray::InsideGeometry(const void* vertexData, unsigned vertexSize, unsigned vertexStart, unsigned vertexCount) const
{
	Real currentFrontFace = F_INFINITY;
	Real currentBackFace = F_INFINITY;
	const unsigned char* vertices = ((const unsigned char*)vertexData) + vertexStart * vertexSize;
	unsigned index = 0;

	while (index + 2 < vertexCount)
	{
		const Vector3& v0 = *((const Vector3*)(&vertices[index * vertexSize]));
		const Vector3& v1 = *((const Vector3*)(&vertices[(index + 1) * vertexSize]));
		const Vector3& v2 = *((const Vector3*)(&vertices[(index + 2) * vertexSize]));
		Real frontFaceDistance = HitDistance(v0, v1, v2);
		Real backFaceDistance = HitDistance(v2, v1, v0);
		currentFrontFace = Min(frontFaceDistance > 0.0f ? frontFaceDistance : F_INFINITY, currentFrontFace);
		currentBackFace = Min(backFaceDistance > 0.0f ? backFaceDistance : F_INFINITY, currentBackFace);
		index += 3;
	}

	if (currentFrontFace != F_INFINITY || currentBackFace != F_INFINITY)
		return currentBackFace < currentFrontFace;

	return false;
}

bool Ray::InsideGeometry(const void* vertexData, unsigned vertexSize, const void* indexData, unsigned indexSize,
	unsigned indexStart, unsigned indexCount) const
{
	Real currentFrontFace = F_INFINITY;
	Real currentBackFace = F_INFINITY;
	const auto* vertices = (const unsigned char*)vertexData;

	// 16-bit indices
	if (indexSize == sizeof(unsigned short))
	{
		const unsigned short* indices = ((const unsigned short*)indexData) + indexStart;
		const unsigned short* indicesEnd = indices + indexCount;

		while (indices < indicesEnd)
		{
			const Vector3& v0 = *((const Vector3*)(&vertices[indices[0] * vertexSize]));
			const Vector3& v1 = *((const Vector3*)(&vertices[indices[1] * vertexSize]));
			const Vector3& v2 = *((const Vector3*)(&vertices[indices[2] * vertexSize]));
			Real frontFaceDistance = HitDistance(v0, v1, v2);
			Real backFaceDistance = HitDistance(v2, v1, v0);
			currentFrontFace = Min(frontFaceDistance > 0.0f ? frontFaceDistance : F_INFINITY, currentFrontFace);
			currentBackFace = Min(backFaceDistance > 0.0f ? backFaceDistance : F_INFINITY, currentBackFace);
			indices += 3;
		}
	}
	// 32-bit indices
	else
	{
		const unsigned* indices = ((const unsigned*)indexData) + indexStart;
		const unsigned* indicesEnd = indices + indexCount;

		while (indices < indicesEnd)
		{
			const Vector3& v0 = *((const Vector3*)(&vertices[indices[0] * vertexSize]));
			const Vector3& v1 = *((const Vector3*)(&vertices[indices[1] * vertexSize]));
			const Vector3& v2 = *((const Vector3*)(&vertices[indices[2] * vertexSize]));
			Real frontFaceDistance = HitDistance(v0, v1, v2);
			Real backFaceDistance = HitDistance(v2, v1, v0);
			currentFrontFace = Min(frontFaceDistance > 0.0f ? frontFaceDistance : F_INFINITY, currentFrontFace);
			currentBackFace = Min(backFaceDistance > 0.0f ? backFaceDistance : F_INFINITY, currentBackFace);
			indices += 3;
		}
	}

	if (currentFrontFace != F_INFINITY || currentBackFace != F_INFINITY)
		return currentBackFace < currentFrontFace;

	return false;
}

Ray Ray::Transformed(const Matrix3x4& transform) const
{
	Ray ret;
	ret.origin_ = transform * origin_;
	ret.direction_ = transform * Vector4(direction_, 0.0f);
	return ret;
}

}
