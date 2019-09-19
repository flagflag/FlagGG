#include "Scene/Scene.h"
#include "Core/EventDefine.h"
#include "Scene/Component.h"
#include "Scene/Light.h"

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
				// 取消注册的接口没写。。。后面补上吧
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

		void Scene::Render(Container::PODVector<Graphics::RenderContext*>& renderContexts)
		{
			Render(this, renderContexts);
		}

		void Scene::Render(Node* node, Container::PODVector<Graphics::RenderContext*>& renderContexts)
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

		void Scene::GetLights(Container::PODVector<Light*>& lights)
		{
			GetLights(this, lights);
		}

		void Scene::GetLights(Node* node, Container::PODVector<Light*>& lights)
		{
			auto* light = node->GetComponent<Light>();
			if (light)
			{
				lights.Push(light);
			}

			for (const auto& child : node->GetChildren())
			{
				GetLights(child, lights);
			}
		}
	}
}
