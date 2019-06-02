#include <Scene/Component.h>
#include <Scene/StaticMeshComponent.h>
#include <Scene/SkeletonMeshComponent.h>
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
	context_(context)
{ }

bool Unit::Load(const String& path)
{
	auto* cache = context_->GetVariable<ResourceCache>("ResourceCache");
	LJSONFile* jsonFile = cache->GetResource<LJSONFile>(path);
	if (!jsonFile)
	{
		FLAGGG_LOG_ERROR("Failed to load unit config[%s].", path.CString());
		return false;
	}

	const LJSONValue& root = jsonFile->GetRoot();
	String type = root["geometrytype"].GetString();

	SharedPtr<StaticMeshComponent> meshComp;
	if (type == "STATIC")
		meshComp = new StaticMeshComponent();
	else if (type == "SKINNED")
		meshComp = new SkeletonMeshComponent();

	AddComponent(meshComp);
	meshComp->SetModel(cache->GetResource<Model>(root["model"].GetString()));
	meshComp->SetMaterial(cache->GetResource<Material>(root["material"].GetString()));

	return true;
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

