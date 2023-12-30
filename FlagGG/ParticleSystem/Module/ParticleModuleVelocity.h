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

	// 从XML加载
	bool LoadXML(const XMLElement& root) override;

	// 保存到XML中
	bool SaveXML(XMLElement& root) override;

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

	// 从XML加载
	bool LoadXML(const XMLElement& root) override;

	// 保存到XML中
	bool SaveXML(XMLElement& root) override;


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

class ParticleModuleVelocityOverLifetime : public ParticleModuleVelocityBase
{
	OBJECT_OVERRIDE(ParticleModuleVelocityOverLifetime, ParticleModuleVelocityBase);
public:
	ParticleModuleVelocityOverLifetime();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//~ Begin ParticleModule Interface
	virtual void Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	virtual void Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime) override;
	//~ Begin ParticleModule Interface

	// 从XML加载
	bool LoadXML(const XMLElement& root) override;

	// 保存到XML中
	bool SaveXML(XMLElement& root) override;


	/**
	 *	The scaling  value applied to the velocity.
	 *	Value is retrieved using the RelativeTime of the particle.
	 */
	RawDistributionVector velOverLife_;

	/**
	 *	If true, the velocity will be SET to the value from the above dist.
	 *	If false, the velocity will be scaled by the above dist.
	 */
	UInt32 absolute_ : 1;
};

}
