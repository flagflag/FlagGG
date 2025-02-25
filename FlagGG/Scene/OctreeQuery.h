#pragma once

#include "Export.h"
#include "Define.h"
#include "Math/Vector3.h"
#include "Math/Ray.h"
#include "Math/Frustum.h"
#include "Container/Vector.h"
#include "Math/Math.h"
#include "Graphics/GraphicsDef.h"

namespace FlagGG
{

class Node;
class DrawableComponent;

class FlagGG_API OctreeQuery
{
public:
	OctreeQuery(PODVector<DrawableComponent*>& result, UInt8 drawableFlags, UInt32 viewMask) :
		result_(result),
		drawableFlags_(drawableFlags),
		viewMask_(viewMask)
	{
	}

	virtual ~OctreeQuery() = default;

	// É¾³ý¿½±´º¯Êý
	OctreeQuery(const OctreeQuery & rhs) = delete;
	// É¾³ý¿½±´º¯Êý
	OctreeQuery& operator =(const OctreeQuery& rhs) = delete;

	/// Intersection test for an octant.
	virtual Intersection TestOctant(const BoundingBox & box, bool inside) = 0;
	/// Intersection test for drawables.
	virtual void TestDrawables(DrawableComponent** start, DrawableComponent** end, bool inside) = 0;

	/// Result vector reference.
	PODVector<DrawableComponent*>& result_;
	/// DrawableComponent flags to include.
	UInt8 drawableFlags_;
	/// DrawableComponent layers to include.
	UInt32 viewMask_;
};

/// Point octree query.
class FlagGG_API PointOctreeQuery : public OctreeQuery
{
public:
	/// Construct with point and query parameters.
	PointOctreeQuery(PODVector<DrawableComponent*>& result, const Vector3& point, UInt8 drawableFlags = DRAWABLE_ANY,
		UInt32 viewMask = DEFAULT_VIEWMASK) :
		OctreeQuery(result, drawableFlags, viewMask),
		point_(point)
	{
	}

	/// Intersection test for an octant.
	Intersection TestOctant(const BoundingBox& box, bool inside) override;
	/// Intersection test for drawables.
	void TestDrawables(DrawableComponent** start, DrawableComponent** end, bool inside) override;

	/// Point.
	Vector3 point_;
};

/// %Sphere octree query.
class FlagGG_API SphereOctreeQuery : public OctreeQuery
{
public:
	/// Construct with sphere and query parameters.
	SphereOctreeQuery(PODVector<DrawableComponent*>& result, const Sphere& sphere, UInt8 drawableFlags = DRAWABLE_ANY,
		UInt32 viewMask = DEFAULT_VIEWMASK) :
		OctreeQuery(result, drawableFlags, viewMask),
		sphere_(sphere)
	{
	}

	/// Intersection test for an octant.
	Intersection TestOctant(const BoundingBox& box, bool inside) override;
	/// Intersection test for drawables.
	void TestDrawables(DrawableComponent** start, DrawableComponent** end, bool inside) override;

	/// Sphere.
	Sphere sphere_;
};

/// Bounding box octree query.
class FlagGG_API BoxOctreeQuery : public OctreeQuery
{
public:
	/// Construct with bounding box and query parameters.
	BoxOctreeQuery(PODVector<DrawableComponent*>& result, const BoundingBox& box, UInt8 drawableFlags = DRAWABLE_ANY,
		UInt32 viewMask = DEFAULT_VIEWMASK) :
		OctreeQuery(result, drawableFlags, viewMask),
		box_(box)
	{
	}

	/// Intersection test for an octant.
	Intersection TestOctant(const BoundingBox& box, bool inside) override;
	/// Intersection test for drawables.
	void TestDrawables(DrawableComponent** start, DrawableComponent** end, bool inside) override;

	/// Bounding box.
	BoundingBox box_;
};

/// %Frustum octree query.
class FlagGG_API FrustumOctreeQuery : public OctreeQuery
{
public:
	/// Construct with frustum and query parameters.
	FrustumOctreeQuery(PODVector<DrawableComponent*>& result, const Frustum& frustum, UInt8 drawableFlags = DRAWABLE_ANY,
		UInt32 viewMask = DEFAULT_VIEWMASK) :
		OctreeQuery(result, drawableFlags, viewMask),
		frustum_(frustum)
	{
	}

	/// Intersection test for an octant.
	Intersection TestOctant(const BoundingBox& box, bool inside) override;
	/// Intersection test for drawables.
	void TestDrawables(DrawableComponent** start, DrawableComponent** end, bool inside) override;

	/// Frustum.
	Frustum frustum_;
};

enum RayQueryLevel
{
	RAY_QUERY_AABB = 0,
	RAY_QUERY_OBB,
	RAY_QUERY_TRIANGLE,
};

class FlagGG_API RayQueryResult
{
public:
	RayQueryResult() = default;

	~RayQueryResult() = default;
			
	Vector3 position_;
	Vector3 normal_;
	Real distance_;
	DrawableComponent* component_{ nullptr };
	Node* node_{ nullptr };
};

class FlagGG_API RayOctreeQuery
{
public:
	RayOctreeQuery(PODVector<RayQueryResult>& results, const Ray& ray,
		RayQueryLevel level = RAY_QUERY_TRIANGLE, Real maxDistance = F_INFINITY, UInt32 drawableFlags = DRAWABLE_ANY) :
		results_(results),
		ray_(ray),
		level_(level),
		maxDistance_(maxDistance),
		drawableFlags_(drawableFlags)
	{}

	PODVector<RayQueryResult>& results_;
	RayQueryLevel level_;
	Real maxDistance_;
	UInt32 drawableFlags_;
	Ray ray_;
};

}
