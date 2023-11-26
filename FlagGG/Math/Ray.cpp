#include "Math/Ray.h"
#include "Math/Math.h"
#include "Math/Vector4.h"
#include "Math/BoundingBox.h"
#include "Math/Plane.h"
#include "Math/Frustum.h"
#include "Math/Sphere.h"

namespace FlagGG
{

Vector3 Ray::ClosestPoint(const Ray& ray) const
{
    // Algorithm based on http://paulbourke.net/geometry/lineline3d/
    Vector3 p13 = origin_ - ray.origin_;
    Vector3 p43 = ray.direction_;
    Vector3 p21 = direction_;

    float d1343 = p13.DotProduct(p43);
    float d4321 = p43.DotProduct(p21);
    float d1321 = p13.DotProduct(p21);
    float d4343 = p43.DotProduct(p43);
    float d2121 = p21.DotProduct(p21);

    float d = d2121 * d4343 - d4321 * d4321;
    if (Abs(d) < F_EPSILON)
        return origin_;
    float n = d1343 * d4321 - d1321 * d4343;
    float a = n / d;

    return origin_ + a * direction_;
}

float Ray::HitDistance(const Plane& plane) const
{
    float d = plane.normal_.DotProduct(direction_);
    if (Abs(d) >= F_EPSILON)
    {
        float t = -(plane.normal_.DotProduct(origin_) + plane.d_) / d;
        if (t >= 0.0f)
            return t;
        else
            return F_INFINITY;
    }
    else
        return F_INFINITY;
}

float Ray::HitDistance(const BoundingBox& box, float insideValue) const
{
    // If undefined, no hit (infinite distance)
    if (!box.Defined())
        return F_INFINITY;

    // Check for ray origin being inside the box
    if (box.IsInside(origin_))
        return insideValue;

    float dist = F_INFINITY;

    // Check for intersecting in the X-direction
    if (origin_.x_ < box.min_.x_ && direction_.x_ > 0.0f)
    {
        float x = (box.min_.x_ - origin_.x_) / direction_.x_;
        if (x < dist)
        {
            Vector3 point = origin_ + x * direction_;
            if (point.y_ >= box.min_.y_ && point.y_ <= box.max_.y_ && point.z_ >= box.min_.z_ && point.z_ <= box.max_.z_)
                dist = x;
        }
    }
    if (origin_.x_ > box.max_.x_ && direction_.x_ < 0.0f)
    {
        float x = (box.max_.x_ - origin_.x_) / direction_.x_;
        if (x < dist)
        {
            Vector3 point = origin_ + x * direction_;
            if (point.y_ >= box.min_.y_ && point.y_ <= box.max_.y_ && point.z_ >= box.min_.z_ && point.z_ <= box.max_.z_)
                dist = x;
        }
    }
    // Check for intersecting in the Y-direction
    if (origin_.y_ < box.min_.y_ && direction_.y_ > 0.0f)
    {
        float x = (box.min_.y_ - origin_.y_) / direction_.y_;
        if (x < dist)
        {
            Vector3 point = origin_ + x * direction_;
            if (point.x_ >= box.min_.x_ && point.x_ <= box.max_.x_ && point.z_ >= box.min_.z_ && point.z_ <= box.max_.z_)
                dist = x;
        }
    }
    if (origin_.y_ > box.max_.y_ && direction_.y_ < 0.0f)
    {
        float x = (box.max_.y_ - origin_.y_) / direction_.y_;
        if (x < dist)
        {
            Vector3 point = origin_ + x * direction_;
            if (point.x_ >= box.min_.x_ && point.x_ <= box.max_.x_ && point.z_ >= box.min_.z_ && point.z_ <= box.max_.z_)
                dist = x;
        }
    }
    // Check for intersecting in the Z-direction
    if (origin_.z_ < box.min_.z_ && direction_.z_ > 0.0f)
    {
        float x = (box.min_.z_ - origin_.z_) / direction_.z_;
        if (x < dist)
        {
            Vector3 point = origin_ + x * direction_;
            if (point.x_ >= box.min_.x_ && point.x_ <= box.max_.x_ && point.y_ >= box.min_.y_ && point.y_ <= box.max_.y_)
                dist = x;
        }
    }
    if (origin_.z_ > box.max_.z_ && direction_.z_ < 0.0f)
    {
        float x = (box.max_.z_ - origin_.z_) / direction_.z_;
        if (x < dist)
        {
            Vector3 point = origin_ + x * direction_;
            if (point.x_ >= box.min_.x_ && point.x_ <= box.max_.x_ && point.y_ >= box.min_.y_ && point.y_ <= box.max_.y_)
                dist = x;
        }
    }

    return dist;
}

float Ray::HitDistance(const Frustum& frustum, bool solidInside) const
{
    float maxOutside = 0.0f;
    float minInside = F_INFINITY;
    bool allInside = true;

    for (const auto& plane : frustum.planes_)
    {
        float distance = HitDistance(plane);

        if (plane.Distance(origin_) < 0.0f)
        {
            maxOutside = Max(maxOutside, distance);
            allInside = false;
        }
        else
            minInside = Min(minInside, distance);
    }

    if (allInside)
        return solidInside ? 0.0f : minInside;
    else if (maxOutside <= minInside)
        return maxOutside;
    else
        return F_INFINITY;
}

float Ray::HitDistance(const Sphere& sphere) const
{
    Vector3 centeredOrigin = origin_ - sphere.center_;
    float squaredRadius = sphere.radius_ * sphere.radius_;

    // Check if ray originates inside the sphere
    if (centeredOrigin.LengthSquared() <= squaredRadius)
        return 0.0f;

    // Calculate intersection by quadratic equation
    float a = direction_.DotProduct(direction_);
    float b = 2.0f * centeredOrigin.DotProduct(direction_);
    float c = centeredOrigin.DotProduct(centeredOrigin) - squaredRadius;
    float d = b * b - 4.0f * a * c;

    // No solution
    if (d < 0.0f)
        return F_INFINITY;

    // Get the nearer solution
    float dSqrt = sqrtf(d);
    float dist = (-b - dSqrt) / (2.0f * a);
    if (dist >= 0.0f)
        return dist;
    else
        return (-b + dSqrt) / (2.0f * a);
}

float Ray::HitDistance(const Vector3& v0, const Vector3& v1, const Vector3& v2, Vector3* outNormal, Vector3* outBary, bool doubleSide) const
{
    // Based on Fast, Minimum Storage Ray/Triangle Intersection by Möller & Trumbore
    // http://www.graphics.cornell.edu/pubs/1997/MT97.pdf
    // Calculate edge vectors
    Vector3 edge1(v1 - v0);
    Vector3 edge2(v2 - v0);

    // Calculate determinant & check backfacing
    Vector3 p(direction_.CrossProduct(edge2));
    float det = edge1.DotProduct(p);
    if (det >= F_EPSILON || (doubleSide && (det <= -F_EPSILON)))
    {
        float f = 1.0f / det;
        // Calculate u & v parameters and test
        Vector3 t(origin_ - v0);
        float u = t.DotProduct(p) * f;
        if (u >= 0.0f && u <= 1.0f)
        {
            Vector3 q(t.CrossProduct(edge1));
            float v = direction_.DotProduct(q) * f;
            if (v >= 0.0f && u + v <= 1.0f)
            {
                float distance = edge2.DotProduct(q) / det;
                // Discard hits behind the ray
                if (distance >= 0.0f)
                {
                    // There is an intersection, so calculate distance & optional normal
                    if (outNormal)
                        *outNormal = edge1.CrossProduct(edge2);
                    if (outBary)
                        *outBary = Vector3(1 - u - v, u, v);

                    return distance;
                }
            }
        }
    }

    return F_INFINITY;
}

float Ray::HitDistance(const void* vertexData, unsigned vertexStride, unsigned vertexStart, unsigned vertexCount,
    Vector3* outNormal, Vector2* outUV, unsigned uvOffset) const
{
    float nearest = F_INFINITY;
    const unsigned char* vertices = ((const unsigned char*)vertexData) + vertexStart * vertexStride;
    unsigned index = 0, nearestIdx = F_MAX_UNSIGNED;
    Vector3 barycentric;
    Vector3* outBary = outUV ? &barycentric : nullptr;

    while (index + 2 < vertexCount)
    {
        const Vector3& v0 = *((const Vector3*)(&vertices[index * vertexStride]));
        const Vector3& v1 = *((const Vector3*)(&vertices[(index + 1) * vertexStride]));
        const Vector3& v2 = *((const Vector3*)(&vertices[(index + 2) * vertexStride]));
        float distance = HitDistance(v0, v1, v2, outNormal, outBary);
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
            // Interpolate the UV coordinate using barycentric coordinate
            const Vector2& uv0 = *((const Vector2*)(&vertices[uvOffset + nearestIdx * vertexStride]));
            const Vector2& uv1 = *((const Vector2*)(&vertices[uvOffset + (nearestIdx + 1) * vertexStride]));
            const Vector2& uv2 = *((const Vector2*)(&vertices[uvOffset + (nearestIdx + 2) * vertexStride]));
            *outUV = Vector2(uv0.x_ * barycentric.x_ + uv1.x_ * barycentric.y_ + uv2.x_ * barycentric.z_,
                uv0.y_ * barycentric.x_ + uv1.y_ * barycentric.y_ + uv2.y_ * barycentric.z_);
        }
    }

    return nearest;
}

float Ray::HitDistance(const void* vertexData, unsigned vertexStride, const void* indexData, unsigned indexSize,
    unsigned indexStart, unsigned indexCount, Vector3* outNormal, Vector2* outUV, unsigned uvOffset) const
{
    float nearest = F_INFINITY;
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
            float distance = HitDistance(v0, v1, v2, outNormal, outBary);
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
                // Interpolate the UV coordinate using barycentric coordinate
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
            float distance = HitDistance(v0, v1, v2, outNormal, outBary);
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
                // Interpolate the UV coordinate using barycentric coordinate
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
    float currentFrontFace = F_INFINITY;
    float currentBackFace = F_INFINITY;
    const unsigned char* vertices = ((const unsigned char*)vertexData) + vertexStart * vertexSize;
    unsigned index = 0;

    while (index + 2 < vertexCount)
    {
        const Vector3& v0 = *((const Vector3*)(&vertices[index * vertexSize]));
        const Vector3& v1 = *((const Vector3*)(&vertices[(index + 1) * vertexSize]));
        const Vector3& v2 = *((const Vector3*)(&vertices[(index + 2) * vertexSize]));
        float frontFaceDistance = HitDistance(v0, v1, v2);
        float backFaceDistance = HitDistance(v2, v1, v0);
        currentFrontFace = Min(frontFaceDistance > 0.0f ? frontFaceDistance : F_INFINITY, currentFrontFace);
        // A backwards face is just a regular one, with the vertices in the opposite order. This essentially checks backfaces by
        // checking reversed frontfaces
        currentBackFace = Min(backFaceDistance > 0.0f ? backFaceDistance : F_INFINITY, currentBackFace);
        index += 3;
    }

    // If the closest face is a backface, that means that the ray originates from the inside of the geometry
    // NOTE: there may be cases where both are equal, as in, no collision to either. This is prevented in the most likely case
    // (ray doesn't hit either) by this conditional
    if (currentFrontFace != F_INFINITY || currentBackFace != F_INFINITY)
        return currentBackFace < currentFrontFace;

    // It is still possible for two triangles to be equally distant from the triangle, however, this is extremely unlikely.
    // As such, it is safe to assume they are not
    return false;
}

bool Ray::InsideGeometry(const void* vertexData, unsigned vertexSize, const void* indexData, unsigned indexSize,
    unsigned indexStart, unsigned indexCount) const
{
    float currentFrontFace = F_INFINITY;
    float currentBackFace = F_INFINITY;
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
            float frontFaceDistance = HitDistance(v0, v1, v2);
            float backFaceDistance = HitDistance(v2, v1, v0);
            currentFrontFace = Min(frontFaceDistance > 0.0f ? frontFaceDistance : F_INFINITY, currentFrontFace);
            // A backwards face is just a regular one, with the vertices in the opposite order. This essentially checks backfaces by
            // checking reversed frontfaces
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
            float frontFaceDistance = HitDistance(v0, v1, v2);
            float backFaceDistance = HitDistance(v2, v1, v0);
            currentFrontFace = Min(frontFaceDistance > 0.0f ? frontFaceDistance : F_INFINITY, currentFrontFace);
            // A backwards face is just a regular one, with the vertices in the opposite order. This essentially checks backfaces by
            // checking reversed frontfaces
            currentBackFace = Min(backFaceDistance > 0.0f ? backFaceDistance : F_INFINITY, currentBackFace);
            indices += 3;
        }
    }

    // If the closest face is a backface, that means that the ray originates from the inside of the geometry
    // NOTE: there may be cases where both are equal, as in, no collision to either. This is prevented in the most likely case
    // (ray doesn't hit either) by this conditional
    if (currentFrontFace != F_INFINITY || currentBackFace != F_INFINITY)
        return currentBackFace < currentFrontFace;

    // It is still possible for two triangles to be equally distant from the triangle, however, this is extremely unlikely.
    // As such, it is safe to assume they are not
    return false;
}

bool Ray::InsideMultiGeometry(const void* vertexData, unsigned vertexSize, const void* indexData, unsigned indexSize, unsigned indexStart, unsigned indexCount) const
{
    const auto* vertices = (const unsigned char*)vertexData;

    unsigned frontFaceNum = 0;
    unsigned backFaceNum = 0;
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
            float frontFaceDistance = HitDistance(v0, v1, v2);
            float backFaceDistance = HitDistance(v2, v1, v0);
            
            frontFaceDistance = frontFaceDistance > 0.0f ? frontFaceDistance : F_INFINITY;
            backFaceDistance = backFaceDistance > 0.0f ? backFaceDistance : F_INFINITY;
            if (frontFaceDistance < F_INFINITY)
            {
                frontFaceNum++;
            }
            if (backFaceDistance < F_INFINITY)
            {
                backFaceNum++;
            }

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
            float frontFaceDistance = HitDistance(v0, v1, v2);
            float backFaceDistance = HitDistance(v2, v1, v0);
            frontFaceDistance = frontFaceDistance > 0.0f ? frontFaceDistance : F_INFINITY;
            backFaceDistance = backFaceDistance > 0.0f ? backFaceDistance : F_INFINITY;
            if (frontFaceDistance < F_INFINITY)
            {
                frontFaceNum++;
            }
            if (backFaceDistance < F_INFINITY)
            {
                backFaceNum++;
            }
            indices += 3;
        }
    }

    return backFaceNum > frontFaceNum;
}

Ray Ray::Transformed(const Matrix3x4& transform) const
{
    Ray ret;
    ret.origin_ = transform * origin_;
    ret.direction_ = transform * Vector4(direction_, 0.0f);
    return ret;
}

}
