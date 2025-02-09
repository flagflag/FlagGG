#include <Scene/Component.h>
#include <Scene/TerrainComponent.h>
#include <Resource/ResourceCache.h>
#include <Graphics/Material.h>
#include <Container/Ptr.h>
#include <Math/Vector3.h>

#include "Unit/Terrain.h"

Terrain::Terrain()
{ }

void Terrain::Create(UInt32 pathSize)
{
	RemoveAllChild();

	auto* cache = GetSubsystem<ResourceCache>();

	auto* comp = GetComponent<TerrainComponent>();
	if (!comp)
	{
		comp = CreateComponent<TerrainComponent>();
	}
	comp->SetPathSize(pathSize);
	comp->SetHeightMap(cache->GetResource<Image>("Textures/HeightMap.png"));
	comp->SetMaterial(cache->GetResource<Material>("Materials/Terrain.ljson"));
	// 目前地形没有分Patch，一整个一公里的mesh，暂时先将地形遮挡裁剪关了
	comp->SetOcclusionCulling(false);
}
