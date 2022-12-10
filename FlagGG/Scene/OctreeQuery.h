#pragma once

#include "Export.h"
#include "Define.h"
#include "Math/Vector3.h"
#include "Math/Ray.h"
#include "Container/Vector.h"
#include "Math/Math.h"

namespace FlagGG
{

class Node;
class Component;

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
	Component* component_{ nullptr };
	Node* node_{ nullptr };
};

class FlagGG_API RayOctreeQuery
{
public:
	RayOctreeQuery(PODVector<RayQueryResult>& results, const Ray& ray,
		RayQueryLevel level = RAY_QUERY_TRIANGLE, Real maxDistance = F_INFINITY) :
		results_(results),
		ray_(ray),
		level_(level),
		maxDistance_(maxDistance)
	{}

	PODVector<RayQueryResult>& results_;
	RayQueryLevel level_;
	Real maxDistance_;
	Ray ray_;
};

}
