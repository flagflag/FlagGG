#include <Scene/Component.h>
#include <Scene/StaticMeshComponent.h>
#include <Scene/SkeletonMeshComponent.h>
#include <Scene/AnimationComponent.h>
#include <Scene/OceanComponent.h>
#include <Resource/ResourceCache.h>
#include <Graphics/Model.h>
#include <Graphics/Material.h>
#include <Config/LJSONFile.h>
#include <Log.h>

#include "Unit/Unit.h"

Unit::Unit() :
	Node(),
	material_(nullptr)
{ }

bool Unit::Load(const String& path, Node* node)
{
	auto* cache = GetSubsystem<ResourceCache>();
	LJSONFile* jsonFile = cache->GetResource<LJSONFile>(path);
	if (!jsonFile)
	{
		FLAGGG_LOG_ERROR("Failed to load unit config[{}].", path.CString());
		return false;
	}

	const LJSONValue& root = jsonFile->GetRoot();
	String type = root["geometrytype"].GetString();
	String dynamicType = root["dynamicmodel"].GetString();

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

	material_ = cache->GetResource<Material>(root["material"].GetString());

	if (meshComp)
	{
		meshComp->SetModel(cache->GetResource<Model>(root["model"].GetString()));
		meshComp->SetMaterial(material_);
		if (root.Contains("cast_shadow"))
			meshComp->SetCastShadow(root["cast_shadow"].GetBool());
		else
			meshComp->SetCastShadow(true);
	}

	if (dynamicType == "Ocean")
	{
		OceanComponent* dynamicMeshComp = node->GetComponent<OceanComponent>();
		if (!dynamicMeshComp)
			dynamicMeshComp = node->CreateComponent<OceanComponent>();
		dynamicMeshComp->SetSize(500, 500);
		dynamicMeshComp->SetElementSize(1);
		dynamicMeshComp->SetMaterial(material_);
	}

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
	auto* cache = GetSubsystem<ResourceCache>();
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


CEUnit::CEUnit() :
	Unit()
{}

bool CEUnit::Load(const String& path)
{
	SharedPtr<Node> root(new Node());
	root->SetScale(Vector3(0.01, 0.01, 0.01));
	root->SetName("SKELETON");
	AddChild(root);
	return Unit::Load(path, root);
}

void CEUnit::PlayAnimation(const String& path, bool isLoop)
{
	Node* node = GetChild(String("SKELETON"));
	if (!node)
		return;
	Unit::PlayAnimation(path, isLoop, node);
}
