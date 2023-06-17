#include "Math/BoundingBox.h"
#include "Math/Matrix4.h"
#include "Math/Frustum.h"
#include "Math/Polyhedron.h"

namespace FlagGG
{

void BoundingBox::Define(const Vector3* vertices, unsigned count)
{
    Clear();

    if (!count)
        return;

    Merge(vertices, count);
}

void BoundingBox::Define(const Frustum& frustum)
{
    Clear();
    Define(frustum.vertices_, NUM_FRUSTUM_VERTICES);
}

void BoundingBox::Define(const Polyhedron& poly)
{
    Clear();
    Merge(poly);
}

void BoundingBox::Define(const Sphere& sphere)
{
    const Vector3& center = sphere.center_;
    float radius = sphere.radius_;

    min_ = center + Vector3(-radius, -radius, -radius);
    max_ = center + Vector3(radius, radius, radius);
}

void BoundingBox::Merge(const Vector3* vertices, unsigned count)
{
    while (count--)
        Merge(*vertices++);
}

void BoundingBox::Merge(const Frustum& frustum)
{
    Merge(frustum.vertices_, NUM_FRUSTUM_VERTICES);
}

void BoundingBox::Merge(const Polyhedron& poly)
{
    for (unsigned i = 0; i < poly.faces_.Size(); ++i)
    {
        const PODVector<Vector3>& face = poly.faces_[i];
        if (!face.Empty())
            Merge(&face[0], face.Size());
    }
}

void BoundingBox::Merge(const Sphere& sphere)
{
    const Vector3& center = sphere.center_;
    float radius = sphere.radius_;

    Merge(center + Vector3(radius, radius, radius));
    Merge(center + Vector3(-radius, -radius, -radius));
}

void BoundingBox::Clip(const BoundingBox& box)
{
    if (box.min_.x_ > min_.x_)
        min_.x_ = box.min_.x_;
    if (box.max_.x_ < max_.x_)
        max_.x_ = box.max_.x_;
    if (box.min_.y_ > min_.y_)
        min_.y_ = box.min_.y_;
    if (box.max_.y_ < max_.y_)
        max_.y_ = box.max_.y_;
    if (box.min_.z_ > min_.z_)
        min_.z_ = box.min_.z_;
    if (box.max_.z_ < max_.z_)
        max_.z_ = box.max_.z_;

    if (min_.x_ > max_.x_ || min_.y_ > max_.y_ || min_.z_ > max_.z_)
    {
        min_ = Vector3(F_INFINITY, F_INFINITY, F_INFINITY);
        max_ = Vector3(-F_INFINITY, -F_INFINITY, -F_INFINITY);
    }
}

void BoundingBox::Transform(const Matrix3& transform)
{
    *this = Transformed(Matrix3x4(transform));
}

void BoundingBox::Transform(const Matrix3x4& transform)
{
    *this = Transformed(transform);
}

BoundingBox BoundingBox::Transformed(const Matrix3& transform) const
{
    return Transformed(Matrix3x4(transform));
}

BoundingBox BoundingBox::Transformed(const Matrix3x4& transform) const
{
    Vector3 newCenter = transform * Center();
    Vector3 oldEdge = Size() * 0.5f;
    Vector3 newEdge = Vector3(
        Abs(transform.m00_) * oldEdge.x_ + Abs(transform.m01_) * oldEdge.y_ + Abs(transform.m02_) * oldEdge.z_,
        Abs(transform.m10_) * oldEdge.x_ + Abs(transform.m11_) * oldEdge.y_ + Abs(transform.m12_) * oldEdge.z_,
        Abs(transform.m20_) * oldEdge.x_ + Abs(transform.m21_) * oldEdge.y_ + Abs(transform.m22_) * oldEdge.z_
    );

    return BoundingBox(newCenter - newEdge, newCenter + newEdge);
}

Rect BoundingBox::Projected(const Matrix4& projection) const
{
    Vector3 projMin = min_;
    Vector3 projMax = max_;
    if (projMin.z_ < F_MIN_NEARCLIP)
        projMin.z_ = F_MIN_NEARCLIP;
    if (projMax.z_ < F_MIN_NEARCLIP)
        projMax.z_ = F_MIN_NEARCLIP;

    Vector3 vertices[8];
    vertices[0] = projMin;
    vertices[1] = Vector3(projMax.x_, projMin.y_, projMin.z_);
    vertices[2] = Vector3(projMin.x_, projMax.y_, projMin.z_);
    vertices[3] = Vector3(projMax.x_, projMax.y_, projMin.z_);
    vertices[4] = Vector3(projMin.x_, projMin.y_, projMax.z_);
    vertices[5] = Vector3(projMax.x_, projMin.y_, projMax.z_);
    vertices[6] = Vector3(projMin.x_, projMax.y_, projMax.z_);
    vertices[7] = projMax;

    Rect rect;
    for (const auto& vertice : vertices)
    {
        Vector3 projected = projection * vertice;
        rect.Merge(Vector2(projected.x_, projected.y_));
    }

    return rect;
}

float BoundingBox::DistanceToPoint(const Vector3& point) const
{
    const Vector3 offset = Center() - point;
    const Vector3 absOffset(Abs(offset.x_), Abs(offset.y_), Abs(offset.z_));
    return VectorMax(Vector3::ZERO, absOffset - HalfSize()).Length();
}

Intersection BoundingBox::IsInside(const Sphere& sphere) const
{
    float distSquared = 0;
    float temp;
    const Vector3& center = sphere.center_;

    if (center.x_ < min_.x_)
    {
        temp = center.x_ - min_.x_;
        distSquared += temp * temp;
    }
    else if (center.x_ > max_.x_)
    {
        temp = center.x_ - max_.x_;
        distSquared += temp * temp;
    }
    if (center.y_ < min_.y_)
    {
        temp = center.y_ - min_.y_;
        distSquared += temp * temp;
    }
    else if (center.y_ > max_.y_)
    {
        temp = center.y_ - max_.y_;
        distSquared += temp * temp;
    }
    if (center.z_ < min_.z_)
    {
        temp = center.z_ - min_.z_;
        distSquared += temp * temp;
    }
    else if (center.z_ > max_.z_)
    {
        temp = center.z_ - max_.z_;
        distSquared += temp * temp;
    }

    float radius = sphere.radius_;
    if (distSquared >= radius * radius)
        return OUTSIDE;
    else if (center.x_ - radius < min_.x_ || center.x_ + radius > max_.x_ || center.y_ - radius < min_.y_ ||
             center.y_ + radius > max_.y_ || center.z_ - radius < min_.z_ || center.z_ + radius > max_.z_)
        return INTERSECTS;
    else
        return INSIDE;
}

Intersection BoundingBox::IsInsideFast(const Sphere& sphere) const
{
    float distSquared = 0;
    float temp;
    const Vector3& center = sphere.center_;

    if (center.x_ < min_.x_)
    {
        temp = center.x_ - min_.x_;
        distSquared += temp * temp;
    }
    else if (center.x_ > max_.x_)
    {
        temp = center.x_ - max_.x_;
        distSquared += temp * temp;
    }
    if (center.y_ < min_.y_)
    {
        temp = center.y_ - min_.y_;
        distSquared += temp * temp;
    }
    else if (center.y_ > max_.y_)
    {
        temp = center.y_ - max_.y_;
        distSquared += temp * temp;
    }
    if (center.z_ < min_.z_)
    {
        temp = center.z_ - min_.z_;
        distSquared += temp * temp;
    }
    else if (center.z_ > max_.z_)
    {
        temp = center.z_ - max_.z_;
        distSquared += temp * temp;
    }

    float radius = sphere.radius_;
    if (distSquared >= radius * radius)
        return OUTSIDE;
    else
        return INSIDE;
}

String BoundingBox::ToString() const
{
    return min_.ToString() + " - " + max_.ToString();
}

}
