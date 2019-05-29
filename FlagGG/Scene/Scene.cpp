#include "Scene/Scene.h"
#include "Core/EventDefine.h"
#include "Scene/Component.h"

namespace FlagGG
{
	namespace Scene
	{
		Scene::Scene(Core::Context* context) :
			context_(context),
			isRunning_(false)
		{

		}

		Scene::~Scene()
		{
			Stop();
		}

		void Scene::Start()
		{
			if (!isRunning_)
			{
				context_->RegisterEvent(EVENT_HANDLER(Core::Frame::LOGIC_UPDATE, Scene::HandleUpdate, this));
				isRunning_ = true;
			}
		}

		void Scene::Stop()
		{
			if (isRunning_)
			{
				// ȡ��ע��Ľӿ�ûд���������油�ϰ�
			}
		}

		void Scene::HandleUpdate(float timeStep)
		{
			Update(timeStep);
		}

		void Scene::Update(float timeStep)
		{
			Update(this, timeStep);
		}

		void Scene::Update(Node* node, float timeStep)
		{
			auto& children = node->GetChildren();

			for (const auto& child : children)
			{
				child->Update(timeStep);
			}

			for (const auto& child : children)
			{
				Update(child, timeStep);
			}
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
