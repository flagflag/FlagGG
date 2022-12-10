#include <Scene/Component.h>
#include <Scene/TerrainComponent.h>
#include <Resource/ResourceCache.h>
#include <Graphics/Material.h>
#include <Container/Ptr.h>
#include <Math/Vector3.h>

#include "Unit/Terrain.h"

Terrain::Terrain(Context* context) :
	context_(context)
{ }

void Terrain::Create(UInt32 pathSize)
{
	RemoveAllChild();

	auto* cache = context_->GetVariable<ResourceCache>("ResourceCache");

	auto* comp = GetComponent<TerrainComponent>();
	if (!comp)
	{
		comp = CreateComponent<TerrainComponent>();
	}
	comp->SetPathSize(pathSize);
	comp->SetHeightMap(cache->GetResource<Image>("Textures/HeightMap.png"));
	comp->SetMaterial(cache->GetResource<Material>("Materials/Terrain.ljson"));
}
