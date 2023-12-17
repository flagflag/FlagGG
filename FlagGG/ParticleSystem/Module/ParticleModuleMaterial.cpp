#include "ParticleModuleMaterial.h"

namespace FlagGG
{

/*-----------------------------------------------------------------------------
	ParticleModuleMeshMaterial
-----------------------------------------------------------------------------*/
ParticleModuleMeshMaterial::ParticleModuleMeshMaterial()
{
}

//## BEGIN PROPS ParticleModuleMeshMaterial
//	TArray<class UMaterialInstance*> MeshMaterials;
	//## END PROPS ParticleModuleMeshMaterial

void ParticleModuleMeshMaterial::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{

}

UInt32 ParticleModuleMeshMaterial::RequiredBytesPerInstance()
{
	return 0;
}


}
