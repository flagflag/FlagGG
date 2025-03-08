#include "PrefabLoader.h"
#include "Config/LJSONFile.h"
#include "Scene/StaticMeshComponent.h"
#include "Scene/SkeletonMeshComponent.h"
#include "Scene/Light.h"
#include "Resource/ResourceCache.h"
#include "FileSystem/FileManager.h"
#include "Graphics/Model.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Shader.h"
#include "Graphics/SCEMaterial.h"
#include "ResourceTranslation/SCE/SCEResourceTranslation.h"

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

static void LoadArchetypeMaterial(const LJSONValue& source, StaticMeshComponent* meshComponent)
{
	auto* cache = GetSubsystem<ResourceCache>();
	const LJSONValue& jsonMaterials = source["materials"];
	for (UInt32 i = 0; i < jsonMaterials.Size(); ++i)
	{
		const String materialPath = jsonMaterials[i].GetString();
		if (materialPath.ToLower() == "editor/materials/engine/gridmaterial.material")
		{
			auto material = cache->GetResource<Material>("Materials/StaticModel.ljson");
			meshComponent->SetMaterial(i, material);
			return;
		}

		SharedPtr<Material> material(cache->GetExistingResource<SCEMaterial>(materialPath));
		if (!material)
		{
			auto stream = GetSubsystem<AssetFileManager>()->OpenFileReader(materialPath);
			if (stream)
			{
				material = new SCEMaterial(meshComponent->IsInstanceOf<SkeletonMeshComponent>());
				if (material->LoadStream(stream))
				{
					material->SetName(ResourceCache::FormatReousrcePath(materialPath));
					cache->AddManualResource(material);
				}
				else
				{
					material = cache->GetResource<Material>("Materials/StaticModel.ljson");
				}
			}
		}

		meshComponent->SetMaterial(i, material);
	}
}

static SharedPtr<Node> LoadPrefab_StaticMeshArchetype(const LJSONValue& source)
{
	auto* cache = GetSubsystem<ResourceCache>();
	SharedPtr<Node> instance = LoadPrefab_Transform(source);
	auto* meshComponent = instance->CreateComponent<StaticMeshComponent>();
	meshComponent->SetCastShadow(true);
	{
		const String meshPath = source["staticMesh"].GetString();
		auto meshResource = cache->GetResource<Model>(meshPath);
		meshComponent->SetModel(meshResource);
	}
	LoadArchetypeMaterial(source, meshComponent);
	return instance;
}

static SharedPtr<Node> LoadPrefab_SkeletonMeshArchetype(const LJSONValue& source)
{
	auto* cache = GetSubsystem<ResourceCache>();
	SharedPtr<Node> instance = LoadPrefab_Transform(source);
	auto* meshComponent = instance->CreateComponent<SkeletonMeshComponent>();
	meshComponent->SetCastShadow(true);
	{
		const String meshPath = source["skeletalMesh"].GetString();
		auto meshResource = cache->GetResource<Model>(meshPath);
		meshComponent->SetModel(meshResource);
	}
	LoadArchetypeMaterial(source, meshComponent);
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

static bool LoadPrefab(const String& path, Node* prefabInstance);

static SharedPtr<Node> LoadPrefab_PrefabArchetype(const LJSONValue& source)
{
	SharedPtr<Node> instance = LoadPrefab_Transform(source);
	const String prefabPath = source["prefabPath"].GetString();
	LoadPrefab(prefabPath, instance);
	return instance;
}

static HashMap<String, std::function<SharedPtr<Node>(const LJSONValue&)>> ArchetypeLoaderMap =
{
	{ "staticMesh",   LoadPrefab_StaticMeshArchetype },
	{ "skeletalMesh", LoadPrefab_SkeletonMeshArchetype },
	{ "lightMesh",    LoadPrefab_LightArchetype },
	{ "prefabMesh",   LoadPrefab_PrefabArchetype },
};

static bool LoadPrefab(const String& path, Node* prefabInstance)
{
	LJSONFile* jsonFile = GetSubsystem<ResourceCache>()->GetResource<LJSONFile>(path);
	if (!jsonFile)
		return false;
	const auto& source = jsonFile->GetRoot();
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
	return true;
}

SharedPtr<Node> LoadPrefab(const String& path)
{
	SharedPtr<Node> prefabInstance = MakeShared<Node>();
	if (LoadPrefab(path, prefabInstance))
		return prefabInstance;
	return nullptr;
}

}
