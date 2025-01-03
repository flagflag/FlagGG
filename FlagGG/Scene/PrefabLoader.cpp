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
		auto stream = GetSubsystem<AssetFileManager>()->OpenFileReader(materialPath);
		if (stream)
		{
			GenericMaterialDescription materialDesc;
			if (GetSubsystem<SCEResourceTranslation>()->LoadMaterial(stream, materialDesc))
			{
				SharedPtr<Material> material = MakeShared<Material>();
				// Set shader parameters
				{
					material->CreateShaderParameters();
					auto param = material->GetShaderParameters();
					param->AddParametersDefine<Color>("colorFactor");
					param->AddParametersDefine<float>("metallicFactor");
					param->AddParametersDefine<float>("roughnessFactor");
					param->AddParametersDefine<float>("metalMul0or1");
					param->AddParametersDefine<float>("emissiveMul");
					param->AddParametersDefine<Color>("emissiveColor");
					for (auto& it : materialDesc.shaderParameters_)
					{
						if (it.first_ == "ColorFactor")
						{
							param->SetValue<Color>("colorFactor", ToColor(it.second_));
						}
						else if (it.first_ == "MetallicFactor")
						{
							param->SetValue<float>("metallicFactor", ToFloat(it.second_));
						}
						else if (it.first_ == "RoughnessFactor")
						{
							param->SetValue<float>("roughnessFactor", ToFloat(it.second_));
						}
						else if (it.first_ == "MetalMul0or1")
						{
							param->SetValue<float>("metalMul0or1", ToFloat(it.second_));
						}
						else if (it.first_ == "Emissive_Mul")
						{
							param->SetValue<float>("emissiveMul", ToFloat(it.second_));
						}
						else if (it.first_ == "Color_Emissive")
						{
							param->SetValue<Color>("emissiveColor", ToColor(it.second_));
						}
					}
				}
				// Set textures
				for (auto& it : materialDesc.textureDescs_)
				{
					if (it.first_ < MAX_TEXTURE_CLASS)
					{
						auto& textureDesc = it.second_;
						if (auto* existingTexture = cache->GetExistingResource<Texture2D>(textureDesc.image_->GetName()))
						{
							material->SetTexture((TextureClass)it.first_, existingTexture);
						}
						else
						{
							SharedPtr<Texture2D> texture = MakeShared<Texture2D>();
							texture->SetSRGB(textureDesc.srgb_);
							texture->SetAddressMode(TEXTURE_COORDINATE_U, textureDesc.addresMode_[TEXTURE_COORDINATE_U]);
							texture->SetAddressMode(TEXTURE_COORDINATE_V, textureDesc.addresMode_[TEXTURE_COORDINATE_V]);
							texture->SetAddressMode(TEXTURE_COORDINATE_W, textureDesc.addresMode_[TEXTURE_COORDINATE_W]);
							texture->SetFilterMode(textureDesc.filterMode_);
							texture->SetData(textureDesc.image_);
							texture->SetName(textureDesc.image_->GetName());
							cache->AddManualResource(texture);
							material->SetTexture((TextureClass)it.first_, texture);
						}
					}
				}
				// Create pass
				for (auto& it : materialDesc.passDescs_)
				{
					if (it.name_ == "litbase" && it.pixelShaderName_.ToLower() == "shaderlab/model/defaultps")
					{
						const String meshTypeDefine = meshComponent->IsInstanceOf<SkeletonMeshComponent>() ? "SKINNED" : "STATIC";
						{
							auto& pass = material->CreateOrGetRenderPass(RENDER_PASS_TYPE_FORWARD_LIT);
							pass.SetRasterizerState(it.rasterizerState_);
							pass.SetDepthStencilState(it.depthStencilState_);
							auto shaderCode = cache->GetResource<ShaderCode>("Shader/SCE/Default.hlsl");
							pass.SetVertexShader(shaderCode->GetShader(VS, { meshTypeDefine, "SHADOW" }));
							Vector<String> pixelDefines = it.pixelDefines_.Split(' ');
							pixelDefines.Push("SHADOW");
							pixelDefines.Push("DIRLIGHT");
							pass.SetPixelShader(shaderCode->GetShader(PS, pixelDefines));
							pass.SetDepthTestMode(COMPARISON_LESS_EQUAL);
						}
						{
							auto& pass = material->CreateOrGetRenderPass(RENDER_PASS_TYPE_DEFERRED_BASE);
							pass.SetRasterizerState(it.rasterizerState_);
							pass.SetDepthStencilState(it.depthStencilState_);
							auto shaderCode = cache->GetResource<ShaderCode>("Shader/SCE/Default.hlsl");
							pass.SetVertexShader(shaderCode->GetShader(VS, { meshTypeDefine }));
							Vector<String> pixelDefines = it.pixelDefines_.Split(' ');
							pixelDefines.Push("DEFERRED_BASEPASS");
							pass.SetPixelShader(shaderCode->GetShader(PS, pixelDefines));
							pass.SetDepthTestMode(COMPARISON_EQUAL);
							pass.SetDepthWrite(false);
						}
						{
							auto& pass = material->CreateOrGetRenderPass(RENDER_PASS_TYPE_SHADOW);
							auto shaderCode = cache->GetResource<ShaderCode>("Shader/Shadow.hlsl");
							pass.SetVertexShader(shaderCode->GetShader(VS, { meshTypeDefine }));
							pass.SetPixelShader(shaderCode->GetShader(PS, { "MODEL_SHADOW" }));
						}
						{
							auto& pass = material->CreateOrGetRenderPass(RENDER_PASS_TYPE_DEPTH);
							auto shaderCode = cache->GetResource<ShaderCode>("Shader/Depth.hlsl");
							pass.SetVertexShader(shaderCode->GetShader(VS, { meshTypeDefine }));
							pass.SetPixelShader(shaderCode->GetShader(PS, {}));
							pass.SetDepthTestMode(COMPARISON_LESS_EQUAL);
						}
						break;
					}
				}
				//
				meshComponent->SetMaterial(i, material);
			}
			else
			{
				auto material = cache->GetResource<Material>("Materials/StaticModel.ljson");
				meshComponent->SetMaterial(i, material);
			}
		} // if (stream)
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
