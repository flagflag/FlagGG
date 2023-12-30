#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "Graphics/Material.h"

namespace FlagGG
{

class ParticleLODLevel;
struct ParticleEmitterInstance;

class FlagGG_API ParticleModuleMaterialBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleMaterialBase, ParticleModule);
public:
	virtual bool CanTickInAnyThread() override
	{
		return true;
	}
};

class FlagGG_API ParticleModuleMeshMaterial : public ParticleModuleMaterialBase
{
	OBJECT_OVERRIDE(ParticleModuleMeshMaterial, ParticleModuleMaterialBase);
public:
	ParticleModuleMeshMaterial();

	//Begin ParticleModule Interface
	virtual void	Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	virtual UInt32	RequiredBytesPerInstance() override;
	//End ParticleModule Interface

	/** The array of materials to apply to the mesh particles. */
	Vector<SharedPtr<Material>> meshMaterials_;
};

}

