#include <Scene/Component.h>
#include <Scene/TerrainComponent.h>
#include <Resource/ResourceCache.h>
#include <Graphics/Material.h>
#include <Container/Ptr.h>
#include <Math/Vector3.h>

#include "Unit/Terrain.h"

using namespace FlagGG::Container;
using namespace FlagGG::Resource;
using namespace FlagGG::Graphics;
using namespace FlagGG::Math;

Terrain::Terrain(Context* context) :
	context_(context)
{ }

void Terrain::SetRange(uint32_t xCount, uint32_t yCount)
{
	RemoveAllChild();

	auto* cache = context_->GetVariable<ResourceCache>("ResourceCache");

	//for (uint32_t x = 0; x < xCount; ++x)
	//{
	//	for (uint32_t y = 0; y < yCount; ++y)
	//	{
	//		SharedPtr<Node> node(new Node());
	//		StaticMeshComponent* meshComp = node->CreateComponent<StaticMeshComponent>();
	//		meshComp->SetModel(cache->GetResource<Model>("Model/Plane.mdl"));
	//		meshComp->SetMaterial(cache->GetResource<Material>("Materials/Plane.ljson"));
	//		// 设置相对位置
	//		node->SetPosition(Vector3(x, 0, y));
	//		AddChild(node);
	//	}
	//}

	auto* comp = GetComponent<TerrainComponent>();
	if (!comp)
	{
		comp = CreateComponent<TerrainComponent>();
	}
	comp->SetPathSize(64);
	comp->SetHeightMap(cache->GetResource<Image>("Textures/HeightMap.png"));
	comp->SetMaterial(cache->GetResource<Material>("Materials/Terrain.ljson"));
}