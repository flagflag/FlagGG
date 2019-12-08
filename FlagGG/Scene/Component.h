#pragma once

#include "Export.h"
#include "Core/Object.h"
#include "Graphics/RenderContext.h"
#include "Math/BoundingBox.h"
#include "Scene/OctreeQuery.h"

namespace FlagGG
{
	namespace Scene
	{
		class Node;
		class OctreeNode;

		class FlagGG_API Component : public Core::Object
		{
			OBJECT_OVERRIDE(Component);
		public:
			~Component() override;

			virtual void Update(float timeStep) {}

			virtual bool IsDrawable() { return false; }

			virtual Graphics::RenderContext* GetRenderContext() { return nullptr; }

			virtual void UpdateTreeDirty();

			virtual void OnUpdateWorldBoundingBox() {}

			virtual void ProcessRayQuery(const RayOctreeQuery& query, Container::PODVector<RayQueryResult>& results);

			const Math::BoundingBox& GetWorldBoundingBox();

			void SetNode(Node* node);

			Node* GetNode() const;

			void SetOcNode(OctreeNode* ocnode);

			OctreeNode* GetOcNode();

		protected:
			Container::WeakPtr<Node> node_;

			Math::BoundingBox worldBoundingBox_;

			bool worldBoundingBoxDirty_{ true };

			OctreeNode* ocnode_{ nullptr };
		};
	}
}
