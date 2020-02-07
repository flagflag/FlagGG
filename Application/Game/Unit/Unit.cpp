#include <Scene/Component.h>
#include <Scene/StaticMeshComponent.h>
#include <Scene/SkeletonMeshComponent.h>
#include <Scene/AnimationComponent.h>
#include <Resource/ResourceCache.h>
#include <Graphics/Model.h>
#include <Graphics/Material.h>
#include <Config/LJSONFile.h>
#include <Log.h>

#include "Unit/Unit.h"

using namespace FlagGG::Graphics;
using namespace FlagGG::Resource;
using namespace FlagGG::Config;

Unit::Unit(Context* context) :
	Node(),
	context_(context),
	material_(nullptr)
{ }

bool Unit::Load(const String& path, Node* node)
{
	auto* cache = context_->GetVariable<ResourceCache>("ResourceCache");
	LJSONFile* jsonFile = cache->GetResource<LJSONFile>(path);
	if (!jsonFile)
	{
		FLAGGG_LOG_ERROR("Failed to load unit config[{}].", path.CString());
		return false;
	}

	const LJSONValue& root = jsonFile->GetRoot();
	String type = root["geometrytype"].GetString();

	StaticMeshComponent* meshComp = nullptr;
	if (type == "STATIC")
	{
		meshComp = node->GetComponent<StaticMeshComponent>();
		if (!meshComp)
			meshComp = node->CreateComponent<StaticMeshComponent>();
	}
	else if (type == "SKINNED")
	{
		meshComp = node->GetComponent<SkeletonMeshComponent>();
		if (!meshComp)
			meshComp = node->CreateComponent<SkeletonMeshComponent>();
	}
	else
	{
		FLAGGG_LOG_WARN("known model type.");
		return false;
	}

	meshComp->SetModel(cache->GetResource<Model>(root["model"].GetString()));
	material_ = cache->GetResource<Material>(root["material"].GetString());
	meshComp->SetMaterial(material_);

	return true;
}

bool Unit::Load(const String& path)
{
	return Load(path, this);
}

// 移动到某个点，和SetPosition不同的是：会根据速度慢慢往目标方向移动
void Unit::MoveTo(const Vector3& position)
{

}

void Unit::SetSpeed(float speed)
{
	speed_ = speed;
}

float Unit::GetSpeed() const
{
	return speed_;
}

void Unit::PlayAnimation(const String& path, bool isLoop, Node* node)
{
	AnimationComponent* animComp = node->GetComponent<AnimationComponent>();
	if (!animComp)
	{
		animComp = node->CreateComponent<AnimationComponent>();
	}
	auto* cache = context_->GetVariable<ResourceCache>("ResourceCache");
	animComp->SetAnimation(cache->GetResource<Animation>(path));
	animComp->Play(isLoop);
}

void Unit::PlayAnimation(const String& path, bool isLoop)
{
	PlayAnimation(path, isLoop, this);
}

void Unit::StopAnimation()
{
	AnimationComponent* animComp = GetComponent<AnimationComponent>();
	if (animComp)
	{
		animComp->Stop();
	}
}

Material* Unit::GetMaterial() const
{
	return material_;
}

void Unit::SetUnitId(Int32 unitId)
{
	unitId_ = unitId;
}

Int32 Unit::GetUnitId() const
{
	return unitId_;
}

void Unit::OnUpdate(float timeStep)
{

}


CEUnit::CEUnit(Context* context) :
	Unit(context)
{}

bool CEUnit::Load(const String& path)
{
	SharedPtr<Node> rotNode1(new Node());
	rotNode1->SetRotation(Quaternion(90, Vector3(0.0f, -1.0f, 0.0f)));
	rotNode1->SetScale(Vector3(0.01, 0.01, 0.01));
	rotNode1->SetName("CE_ROTATION");
	AddChild(rotNode1);

	SharedPtr<Node> rotNode2(new Node());
	rotNode2->SetRotation(Quaternion(90, Vector3(-1.0f, 0.0f, 0.0f)));
	rotNode2->SetName("CE_ROTATION");
	rotNode1->AddChild(rotNode2);

	return Unit::Load(path, rotNode2);
}

void CEUnit::PlayAnimation(const String& path, bool isLoop)
{
	Node* node = GetChild(String("CE_ROTATION"));
	if (!node)
		return;
	node = node->GetChild(String("CE_ROTATION"));
	if (!node)
		return;
	Unit::PlayAnimation(path, isLoop, node);
}
