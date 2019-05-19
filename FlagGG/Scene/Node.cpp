#include "Scene/Node.h"

namespace FlagGG
{
	namespace Scene
	{
		void Node::Update(float timeStep)
		{
			for (const auto& compoment : components_)
			{
				compoment->Update(timeStep);
			}
		}

		void Node::Render(Container::Vector<Graphics::RenderContext>& renderContexts)
		{
			for (const auto& compoment : components_)
			{
				Graphics::RenderContext renderContext;
				compoment->Render(renderContext);
				renderContexts.Push(renderContext);
			}
		}

		void Node::AddComponent(Component* component)
		{
			Container::SharedPtr<Component> sharedComponent(component);
			for (const auto& comp : components_)
			{
				if (comp == sharedComponent)
				{
					return;
				}
			}
			components_.Push(sharedComponent);
		}

		void Node::RemoveComponent(Component* component)
		{
			components_.Remove(Container::SharedPtr<Component>(component));
		}

		void Node::RemoveAllComponent()
		{
			components_.Clear();
		}

		void Node::AddChild(Node* node)
		{
			Container::SharedPtr<Node> sharedNode(node);
			for (const auto& child : children_)
			{
				if (child == node)
				{
					return;
				}
			}
			children_.Push(sharedNode);
		}

		void Node::RemoveChild(Node* node)
		{
			children_.Remove(Container::SharedPtr<Node>(node));
		}


		Container::Vector<Container::SharedPtr<Node>>& Node::GetChildren()
		{
			return children_;
		}
	}
}
