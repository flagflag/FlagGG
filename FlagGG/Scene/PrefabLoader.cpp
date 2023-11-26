#include "PrefabLoader.h"
#include "Config/LJSONFile.h"
#include "Scene/StaticMeshComponent.h"
#include "Scene/Light.h"
#include "Resource/ResourceCache.h"
#include "Graphics/Model.h"

namespace FlagGG
{

static SharedPtr<Node> LoadPrefab_Transform(const LJSONValue& source)
{
	SharedPtr<Node> instance = MakeShared<Node>();
	{
		const LJSONValue& jsonPosition = source["position"];
		instance->SetPosition(Vector3(jsonPosition["x"].GetDouble(), jsonPosition["y"].GetDouble(), jsonPosition["z"].GetDouble()));
	}
	{
		const LJSONValue& jsonRotation = source["rotation"];
		instance->SetRotation(Quaternion(jsonRotation["w_"].GetDouble(), jsonRotation["x_"].GetDouble(), jsonRotation["y_"].GetDouble(), jsonRotation["z_"].GetDouble()));
	}
	{
		const LJSONValue& jsonScale = source["scale"];
		instance->SetScale(Vector3(jsonScale["x"].GetDouble(), jsonScale["y"].GetDouble(), jsonScale["z"].GetDouble()));
	}
	return instance;
}

static SharedPtr<Node> LoadPrefab_StaticMeshArchetype(const LJSONValue& source)
{
	auto* cache = GetSubsystem<ResourceCache>();
	SharedPtr<Node> instance = LoadPrefab_Transform(source);
	auto* meshComponent = instance->CreateComponent<StaticMeshComponent>();
	{
		const String meshPath = source["staticMesh"].GetString();
		auto meshResource = cache->GetResource<Model>(meshPath);
		meshComponent->SetModel(meshResource);
	}
	{
		const LJSONValue& jsonMaterials = source["materials"];
		for (UInt32 i = 0; i < jsonMaterials.Size(); ++i)
		{
			const String materialPath = jsonMaterials[i].GetString();
			auto material = cache->GetResource<Material>(materialPath);
			meshComponent->SetMaterial(i, material);
		}
	}
	return instance;
}

static SharedPtr<Node> LoadPrefab_SkeletonMeshArchetype(const LJSONValue& source)
{
	SharedPtr<Node> instance = LoadPrefab_Transform(source);
	return instance;
}

static SharedPtr<Node> LoadPrefab_LightArchetype(const LJSONValue& source)
{
	SharedPtr<Node> instance = LoadPrefab_Transform(source);
	auto* light = instance->CreateComponent<Light>();
	light->SetLightType((LightType)source["lightType"].GetUInt());
	const LJSONValue& jsonColor = source["color"];
	light->SetColor(Color(jsonColor["r"].GetDouble(), jsonColor["g"].GetDouble(), jsonColor["b"].GetDouble(), jsonColor["a"].GetDouble()));
	light->SetBrightness(source["brightness"].GetDouble());
	light->SetRange(source["range"].GetDouble());
	return instance;
}

static HashMap<String, std::function<SharedPtr<Node>(const LJSONValue&)>> ArchetypeLoaderMap =
{
	{ "staticMesh", LoadPrefab_StaticMeshArchetype },
	{ "skeletalMesh", LoadPrefab_SkeletonMeshArchetype },
	{ "lightMesh", LoadPrefab_LightArchetype },
};

static SharedPtr<Node> LoadPrefab(const LJSONValue& source)
{
	SharedPtr<Node> prefabInstance = MakeShared<Node>();
	for (unsigned i = 0; i < source.Size(); ++i)
	{
		const LJSONValue& item = source[i];
		const String type = item["type"].GetString();
		auto it = ArchetypeLoaderMap.Find(type);
		if (it != ArchetypeLoaderMap.End())
		{
			SharedPtr<Node> archetypeInstance = it->second_(item);
			prefabInstance->AddChild(archetypeInstance);
		}
	}
	return prefabInstance;
}

SharedPtr<Node> LoadPrefab(const String& path)
{
	LJSONFile jsonFile;
	if (!jsonFile.LoadFile(path))
		return nullptr;
	return LoadPrefab(jsonFile.GetRoot());
}

}
