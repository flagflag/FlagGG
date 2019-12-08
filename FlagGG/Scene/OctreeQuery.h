#pragma once

#include "Export.h"
#include "Define.h"
#include "Math/Vector3.h"
#include "Math/Ray.h"
#include "Container/Vector.h"
#include "Math/Math.h"

namespace FlagGG
{
	namespace Scene
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
			
			Math::Vector3 position_;
			Math::Vector3 normal_;
			float distance_;
			Component* component_{ nullptr };
			Node* node_{ nullptr };
		};

		class FlagGG_API RayOctreeQuery
		{
		public:
			RayOctreeQuery(Container::PODVector<RayQueryResult>& results, const Math::Ray& ray,
				RayQueryLevel level = RAY_QUERY_TRIANGLE, float maxDistance = Math::F_INFINITY) :
				results_(results),
				ray_(ray),
				level_(level),
				maxDistance_(maxDistance)
			{}

			Container::PODVector<RayQueryResult>& results_;
			RayQueryLevel level_;
			float maxDistance_;
			Math::Ray ray_;
		};
	}
}
