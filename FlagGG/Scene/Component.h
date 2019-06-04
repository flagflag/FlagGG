#pragma once

#include "Export.h"
#include "Core/Object.h"
#include "Graphics/RenderContext.h"

namespace FlagGG
{
	namespace Scene
	{
		class Node;

		class FlagGG_API Component : public Core::Object
		{
			OBJECT_OVERRIDE(Component);
		public:
			virtual void Update(float timeStep) {};

			virtual bool IsDrawable() { return false; };

			virtual Graphics::RenderContext* GetRenderContext() { return nullptr; };

			virtual void UpdateTreeDirty() {}

			void SetNode(Node* node);

			Node* GetNode() const;

		protected:
			Container::WeakPtr<Node> node_;
		};
	}
}
