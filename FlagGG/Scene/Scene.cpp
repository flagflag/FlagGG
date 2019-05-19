#include "Scene/Scene.h"

namespace FlagGG
{
	namespace Scene
	{
		void Scene::Update(float timeStep)
		{
			Node::Update(timeStep);
		}

		void Scene::Render(Container::Vector<Graphics::RenderContext>& renderContexts)
		{
			Render(this, renderContexts);
		}

		void Scene::Render(Node* node, Container::Vector<Graphics::RenderContext>& renderContexts)
		{
			auto& children = node->GetChildren();
			
			for (const auto& child : children)
			{
				child->Render(renderContexts);
			}

			for (const auto& child : children)
			{
				Render(child, renderContexts);
			}
		}
	}
}
