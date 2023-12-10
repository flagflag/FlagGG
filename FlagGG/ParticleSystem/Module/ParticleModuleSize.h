#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "Math/Distributions/DistributionVector.h"

namespace FlagGG
{

struct ParticleEmitterInstance;

class FlagGG_API ParticleModuleSizeBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleSizeBase, ParticleModule);
public:
	ParticleModuleSizeBase();
};

class FlagGG_API ParticleModuleSize : public ParticleModuleSizeBase
{
	OBJECT_OVERRIDE(ParticleModuleSize, ParticleModuleSizeBase);
public:
	ParticleModuleSize();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//~ Begin UParticleModule Interface
	virtual void CompileModule(struct ParticleEmitterBuildInfo& emitterInfo) override;
	virtual void Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	//~ End UParticleModule Interface

	/**
	 *	Extended version of spawn, allows for using a random stream for distribution value retrieval
	 *
	 *	@param	Owner				The particle emitter instance that is spawning
	 *	@param	Offset				The offset to the modules payload data
	 *	@param	SpawnTime			The time of the spawn
	 *	@param	InRandomStream		The random stream to use for retrieving random values
	 */
	void SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, struct RandomStream* inRandomStream, BaseParticle* particleBase);

	/**
	 *	The initial size that should be used for a particle.
	 *	The value is retrieved using the EmitterTime during the spawn of a particle.
	 *	It is added to the Size and BaseSize fields of the spawning particle.
	 */
	RawDistributionVector startSize_;
};

}
