#pragma once

#include "Export.h"
#include "Container/RefCounted.h"
#include "Container/Vector.h"
#include "Container/Ptr.h"
#include "Scene/Component.h"
#include "Graphics/RenderContext.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"

namespace FlagGG
{
	namespace Scene
	{
		class FlagGG_API Node : public Container::RefCounted
		{
		public:
			virtual void Update(float timeStep);

			virtual void Render(Container::Vector<Graphics::RenderContext>& renderContexts);

			void AddComponent(Component* component);

			void RemoveComponent(Component* component);

			void RemoveAllComponent();

			void AddChild(Node* node);

			void RemoveChild(Node* node);

			Container::Vector<Container::SharedPtr<Node>>& GetChildren();

		private:
			Container::Vector<Container::SharedPtr<Component>> components_;

			Container::Vector<Container::SharedPtr<Node>> children_;
		};
	}
}
