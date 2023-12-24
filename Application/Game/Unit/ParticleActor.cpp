#include "ParticleActor.h"

#include <Config/LJSONFile.h>
#include <Resource/ResourceCache.h>
#include <ParticleSystem/ParticleSystem.h>
#include <ParticleSystem/ParticleSystemComponent.h>

ParticleActor::ParticleActor()
{

}

ParticleActor::~ParticleActor()
{

}

bool ParticleActor::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
	LJSONFile jsonFile;
	if (!jsonFile.LoadStream(stream))
	{
		return false;
	}

	const LJSONValue& root = jsonFile.GetRoot();
	if (root.Contains("asset"))
	{
		const String assetPath = root["asset"].GetString();

		RemoveAllChild();

		sceneObject_ = new Node();
		auto* pSysComp = sceneObject_->CreateComponent<ParticleSystemComponent>();
		pSysComp->template_ = GetSubsystem<ResourceCache>()->GetResource<ParticleSystem>(assetPath);
		pSysComp->InitializeSystem();

		AddChild(sceneObject_);
	}

	return true;
}

bool ParticleActor::EndLoad()
{
	return true;
}

bool ParticleActor::BeginSave(IOFrame::Buffer::IOBuffer* stream)
{
	// TODO
	return false;
}

bool ParticleActor::EndSave()
{
	// TODO
	return false;
}

