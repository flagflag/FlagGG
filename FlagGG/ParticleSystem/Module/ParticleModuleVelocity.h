#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "Math/Distributions/DistributionFloat.h"
#include "Math/Distributions/DistributionVector.h"

namespace FlagGG
{

struct ParticleEmitterInstance;

class FlagGG_API ParticleModuleVelocityBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleVelocityBase, ParticleModule);
public:
	ParticleModuleVelocityBase();

	/**
	 *	If true, then treat the velocity as world-space defined.
	 *	NOTE: LocalSpace emitters that are moving will see strange results...
	 */
	UInt32 inWorldSpace_ : 1;

	/** If true, then apply the particle system components scale to the velocity value. */
	UInt32 applyOwnerScale_ : 1;

};

class FlagGG_API ParticleModuleVelocity : public ParticleModuleVelocityBase
{
	OBJECT_OVERRIDE(ParticleModuleVelocity, ParticleModuleVelocityBase);
public:
	ParticleModuleVelocity();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//~ Begin UParticleModule Interface
	virtual void Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	//~ Begin UParticleModule Interface

	/**
	 *	Extended version of spawn, allows for using a random stream for distribution value retrieval
	 *
	 *	@param	Owner				The particle emitter instance that is spawning
	 *	@param	Offset				The offset to the modules payload data
	 *	@param	SpawnTime			The time of the spawn
	 *	@param	InRandomStream		The random stream to use for retrieving random values
	 */
	void SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, RandomStream* inRandomStream, BaseParticle* particleBase);


	/**
	 *	The velocity to apply to a particle when it is spawned.
	 *	Value is retrieved using the EmitterTime of the emitter.
	 */
	RawDistributionVector startVelocity_;

	/**
	 *	The velocity to apply to a particle along its radial direction.
	 *	Direction is determined by subtracting the location of the emitter from the particle location at spawn.
	 *	Value is retrieved using the EmitterTime of the emitter.
	 */
	RawDistributionFloat startVelocityRadial_;
};

}
