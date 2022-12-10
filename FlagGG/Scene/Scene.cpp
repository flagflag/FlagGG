#include "Scene/Scene.h"
#include "Core/EventDefine.h"
#include "Scene/Component.h"
#include "Scene/Light.h"
#include "Scene/Octree.h"

namespace FlagGG
{

Scene::Scene(Context* context) :
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
		context_->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, Scene::HandleUpdate, this));
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

void Scene::HandleUpdate(Real timeStep)
{
	NodeUpdateContext updateContext;
	updateContext.timeStep_ = timeStep;
	updateContext.scene_ = this;
	updateContext.octree_ = this->GetComponent<Octree>();
	Update(updateContext);
}

void Scene::Update(const NodeUpdateContext& updateContext)
{
	Update(this, updateContext);
}

void Scene::Update(Node* node, const NodeUpdateContext& updateContext)
{
	auto& children = node->GetChildren();

	for (const auto& child : children)
	{
		child->Update(updateContext);
	}

	for (const auto& child : children)
	{
		Update(child, updateContext);
	}
}

void Scene::Render(PODVector<RenderContext*>& renderContexts)
{
	Render(this, renderContexts);
}

void Scene::Render(Node* node, PODVector<RenderContext*>& renderContexts)
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

void Scene::GetLights(PODVector<Light*>& lights)
{
	GetLights(this, lights);
}

void Scene::GetLights(Node* node, PODVector<Light*>& lights)
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
