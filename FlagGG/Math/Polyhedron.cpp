#include "Math/Frustum.h"
#include "Math/Polyhedron.h"

#ifdef _MSC_VER
#pragma warning(disable:6293)
#endif

namespace FlagGG
{

void Polyhedron::Define(const BoundingBox& box)
{
    Vector3 vertices[8];
    vertices[0] = box.min_;
    vertices[1] = Vector3(box.max_.x_, box.min_.y_, box.min_.z_);
    vertices[2] = Vector3(box.min_.x_, box.max_.y_, box.min_.z_);
    vertices[3] = Vector3(box.max_.x_, box.max_.y_, box.min_.z_);
    vertices[4] = Vector3(box.min_.x_, box.min_.y_, box.max_.z_);
    vertices[5] = Vector3(box.max_.x_, box.min_.y_, box.max_.z_);
    vertices[6] = Vector3(box.min_.x_, box.max_.y_, box.max_.z_);
    vertices[7] = box.max_;

    faces_.Resize(6);
    SetFace(0, vertices[3], vertices[7], vertices[5], vertices[1]);
    SetFace(1, vertices[6], vertices[2], vertices[0], vertices[4]);
    SetFace(2, vertices[6], vertices[7], vertices[3], vertices[2]);
    SetFace(3, vertices[1], vertices[5], vertices[4], vertices[0]);
    SetFace(4, vertices[7], vertices[6], vertices[4], vertices[5]);
    SetFace(5, vertices[2], vertices[3], vertices[1], vertices[0]);
}

void Polyhedron::Define(const Frustum& frustum)
{
    const Vector3* vertices = frustum.vertices_;

    faces_.Resize(6);
    SetFace(0, vertices[0], vertices[4], vertices[5], vertices[1]);
    SetFace(1, vertices[7], vertices[3], vertices[2], vertices[6]);
    SetFace(2, vertices[7], vertices[4], vertices[0], vertices[3]);
    SetFace(3, vertices[1], vertices[5], vertices[6], vertices[2]);
    SetFace(4, vertices[4], vertices[7], vertices[6], vertices[5]);
    SetFace(5, vertices[3], vertices[0], vertices[1], vertices[2]);
}

void Polyhedron::AddFace(const Vector3& v0, const Vector3& v1, const Vector3& v2)
{
    faces_.Resize(faces_.Size() + 1);
    PODVector<Vector3>& face = faces_[faces_.Size() - 1];
    face.Resize(3);
    face[0] = v0;
    face[1] = v1;
    face[2] = v2;
}

void Polyhedron::AddFace(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3)
{
    faces_.Resize(faces_.Size() + 1);
    PODVector<Vector3>& face = faces_[faces_.Size() - 1];
    face.Resize(4);
    face[0] = v0;
    face[1] = v1;
    face[2] = v2;
    face[3] = v3;
}

void Polyhedron::AddFace(const PODVector<Vector3>& face)
{
    faces_.Push(face);
}

void Polyhedron::Clip(const Plane& plane)
{
    clippedVertices_.Clear();

    for (unsigned i = 0; i < faces_.Size(); ++i)
    {
        PODVector<Vector3>& face = faces_[i];
        Vector3 lastVertex;
        float lastDistance = 0.0f;

        outFace_.Clear();

        for (unsigned j = 0; j < face.Size(); ++j)
        {
            float distance = plane.Distance(face[j]);
            if (distance >= 0.0f)
            {
                if (lastDistance < 0.0f)
                {
                    float t = lastDistance / (lastDistance - distance);
                    Vector3 clippedVertex = lastVertex + t * (face[j] - lastVertex);
                    outFace_.Push(clippedVertex);
                    clippedVertices_.Push(clippedVertex);
                }

                outFace_.Push(face[j]);
            }
            else
            {
                if (lastDistance >= 0.0f && j != 0)
                {
                    float t = lastDistance / (lastDistance - distance);
                    Vector3 clippedVertex = lastVertex + t * (face[j] - lastVertex);
                    outFace_.Push(clippedVertex);
                    clippedVertices_.Push(clippedVertex);
                }
            }

            lastVertex = face[j];
            lastDistance = distance;
        }

        // Recheck the distances of the last and first vertices and add the final clipped vertex if applicable
        float distance = plane.Distance(face[0]);
        if ((lastDistance < 0.0f && distance >= 0.0f) || (lastDistance >= 0.0f && distance < 0.0f))
        {
            float t = lastDistance / (lastDistance - distance);
            Vector3 clippedVertex = lastVertex + t * (face[0] - lastVertex);
            outFace_.Push(clippedVertex);
            clippedVertices_.Push(clippedVertex);
        }

        // Do not keep faces which are less than triangles
        if (outFace_.Size() < 3)
            outFace_.Clear();

        face = outFace_;
    }

    // Remove empty faces
    for (unsigned i = faces_.Size() - 1; i < faces_.Size(); --i)
    {
        if (faces_[i].Empty())
            faces_.Erase(i);
    }

    // Create a new face from the clipped vertices. First remove duplicates
    for (unsigned i = 0; i < clippedVertices_.Size(); ++i)
    {
        for (unsigned j = clippedVertices_.Size() - 1; j > i; --j)
        {
            if (clippedVertices_[j].Equals(clippedVertices_[i]))
                clippedVertices_.Erase(j);
        }
    }

    if (clippedVertices_.Size() > 3)
    {
        outFace_.Clear();

        // Start with the first vertex
        outFace_.Push(clippedVertices_.Front());
        clippedVertices_.Erase(0);

        while (!clippedVertices_.Empty())
        {
            // Then add the vertex which is closest to the last added
            const Vector3& lastAdded = outFace_.Back();
            float bestDistance = F_INFINITY;
            unsigned bestIndex = 0;

            for (unsigned i = 0; i < clippedVertices_.Size(); ++i)
            {
                float distance = (clippedVertices_[i] - lastAdded).LengthSquared();
                if (distance < bestDistance)
                {
                    bestDistance = distance;
                    bestIndex = i;
                }
            }

            outFace_.Push(clippedVertices_[bestIndex]);
            clippedVertices_.Erase(bestIndex);
        }

        faces_.Push(outFace_);
    }
}

void Polyhedron::Clip(const Frustum& frustum)
{
    for (const auto& plane : frustum.planes_)
        Clip(plane);
}

void Polyhedron::Clip(const BoundingBox& box)
{
    Vector3 vertices[8];
    vertices[0] = box.min_;
    vertices[1] = Vector3(box.max_.x_, box.min_.y_, box.min_.z_);
    vertices[2] = Vector3(box.min_.x_, box.max_.y_, box.min_.z_);
    vertices[3] = Vector3(box.max_.x_, box.max_.y_, box.min_.z_);
    vertices[4] = Vector3(box.min_.x_, box.min_.y_, box.max_.z_);
    vertices[5] = Vector3(box.max_.x_, box.min_.y_, box.max_.z_);
    vertices[6] = Vector3(box.min_.x_, box.max_.y_, box.max_.z_);
    vertices[7] = box.max_;

    Clip(Plane(vertices[5], vertices[7], vertices[3]));
    Clip(Plane(vertices[0], vertices[2], vertices[6]));
    Clip(Plane(vertices[3], vertices[7], vertices[6]));
    Clip(Plane(vertices[4], vertices[5], vertices[1]));
    Clip(Plane(vertices[4], vertices[6], vertices[7]));
    Clip(Plane(vertices[1], vertices[3], vertices[2]));
}

void Polyhedron::Clear()
{
    faces_.Clear();
}

void Polyhedron::Transform(const Matrix3& transform)
{
    for (unsigned i = 0; i < faces_.Size(); ++i)
    {
        PODVector<Vector3>& face = faces_[i];
        for (unsigned j = 0; j < face.Size(); ++j)
            face[j] = transform * face[j];
    }
}

void Polyhedron::Transform(const Matrix3x4& transform)
{
    for (unsigned i = 0; i < faces_.Size(); ++i)
    {
        PODVector<Vector3>& face = faces_[i];
        for (unsigned j = 0; j < face.Size(); ++j)
            face[j] = transform * face[j];
    }
}

Polyhedron Polyhedron::Transformed(const Matrix3& transform) const
{
    Polyhedron ret;
    ret.faces_.Resize(faces_.Size());

    for (unsigned i = 0; i < faces_.Size(); ++i)
    {
        const PODVector<Vector3>& face = faces_[i];
        PODVector<Vector3>& newFace = ret.faces_[i];
        newFace.Resize(face.Size());

        for (unsigned j = 0; j < face.Size(); ++j)
            newFace[j] = transform * face[j];
    }

    return ret;
}

Polyhedron Polyhedron::Transformed(const Matrix3x4& transform) const
{
    Polyhedron ret;
    ret.faces_.Resize(faces_.Size());

    for (unsigned i = 0; i < faces_.Size(); ++i)
    {
        const PODVector<Vector3>& face = faces_[i];
        PODVector<Vector3>& newFace = ret.faces_[i];
        newFace.Resize(face.Size());

        for (unsigned j = 0; j < face.Size(); ++j)
            newFace[j] = transform * face[j];
    }

    return ret;
}

void Polyhedron::SetFace(unsigned index, const Vector3& v0, const Vector3& v1, const Vector3& v2)
{
    PODVector<Vector3>& face = faces_[index];
    face.Resize(3);
    face[0] = v0;
    face[1] = v1;
    face[2] = v2;
}

void Polyhedron::SetFace(unsigned index, const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3)
{
    PODVector<Vector3>& face = faces_[index];
    face.Resize(4);
    face[0] = v0;
    face[1] = v1;
    face[2] = v2;
    face[3] = v3;
}

}
