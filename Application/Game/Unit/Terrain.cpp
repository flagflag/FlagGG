#include <Scene/Component.h>
#include <Scene/TerrainComponent.h>
#include <Resource/ResourceCache.h>
#include <Graphics/Material.h>
#include <Container/Ptr.h>
#include <Math/Vector3.h>

#include "Unit/Terrain.h"

Terrain::Terrain()
{ }

void Terrain::Create(UInt32 pathSize, const Vector3& quadSize, const String& heightMap, const String& materialPath)
{
	RemoveAllChild();

	auto* cache = GetSubsystem<ResourceCache>();

	auto* comp = GetComponent<TerrainComponent>();
	if (!comp)
	{
		comp = CreateComponent<TerrainComponent>();
	}
	comp->SetPatchSize(pathSize);
	comp->SetQuadSize(quadSize);
	comp->SetHeightMap(cache->GetResource<Image>(heightMap));
	comp->SetMaterial(cache->GetResource<Material>(materialPath));
	comp->CreateGeometry();
}
