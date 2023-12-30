#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "Math/Distributions/DistributionFloat.h"

namespace FlagGG
{

struct ParticleEmitterInstance;

class ParticleModuleRotationBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleRotationBase, ParticleModule);
public:
};

class ParticleModuleRotation : public ParticleModuleRotationBase
{
	OBJECT_OVERRIDE(ParticleModuleRotation, ParticleModuleRotationBase);
public:
	ParticleModuleRotation();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//Begin ParticleModule Interface
	virtual void	Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	//End ParticleModule Interface

	/**
	 *	Extended version of spawn, allows for using a random stream for distribution value retrieval
	 *
	 *	@param	Owner				The particle emitter instance that is spawning
	 *	@param	Offset				The offset to the modules payload data
	 *	@param	SpawnTime			The time of the spawn
	 *	@param	InRandomStream		The random stream to use for retrieving random values
	 */
	void SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, struct RandomStream* inRandomStream, BaseParticle* particleBase);

	// 从XML加载
	bool LoadXML(const XMLElement& root) override;

	// 保存到XML中
	bool SaveXML(XMLElement& root) override;


	/**
	 *	Initial rotation of the particle (1 = 360 degrees).
	 *	The value is retrieved using the EmitterTime.
	 */
	RawDistributionFloat startRotation_;
};

}
