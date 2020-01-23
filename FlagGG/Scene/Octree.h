#pragma once

#include "Export.h"
#include "Define.h"
#include "Container/Vector.h"
#include "Container/Allocator.h"
#include "Scene/Component.h"
#include "Scene/OctreeQuery.h"
#include "Math/BoundingBox.h"

namespace FlagGG
{
	namespace Scene
	{
		struct FlagGG_API OctreeNode
		{
			OctreeNode(const Math::BoundingBox& box, UInt32 level);

			~OctreeNode();

			void AddElement(Component* component);

			void RemoveElement(Component* component);

			OctreeNode* children_[8]{ nullptr };

			UInt32 level_;
			Math::BoundingBox box_;
			Math::BoundingBox cullingBox_;
			Container::PODVector<Component*> components_;
		};

		class FlagGG_API Octree : public Component
		{
		public:
			explicit Octree();

			~Octree() override = default;

			void Raycast(RayOctreeQuery& query);

			void InsertElement(Component* component);

		protected:
			bool CheckInsert(OctreeNode* node, const Math::BoundingBox& box);

			void InsertElement(OctreeNode* node, Component* component);

			OctreeNode* GetOrCreateChild(OctreeNode* node, UInt32 index);

			void RaycastImpl(OctreeNode* node, RayOctreeQuery& query);

		private:
			OctreeNode root_;

			UInt32 maxLevel_;

			Container::Allocator<OctreeNode> nodeAllocator_;
		};
	}
}
