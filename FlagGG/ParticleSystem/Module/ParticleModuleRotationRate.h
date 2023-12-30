#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "Math/Distributions/DistributionFloat.h"

namespace FlagGG
{

class ParticleEmitter;
struct ParticleEmitterInstance;

class ParticleModuleRotationRateBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleRotationRateBase, ParticleModule);
public:
};

class ParticleModuleRotationRate : public ParticleModuleRotationRateBase
{
	OBJECT_OVERRIDE(ParticleModuleRotationRate, ParticleModuleRotationRateBase);
public:
	ParticleModuleRotationRate();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//Begin ParticleModule Interface
	virtual void CompileModule(struct ParticleEmitterBuildInfo& emitterInfo) override;
	virtual void Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	virtual void SetToSensibleDefaults(ParticleEmitter* owner) override;
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
	 *	Initial rotation rate, in rotations per second.
	 *	The value is retrieved using the EmitterTime.
	 */
	RawDistributionFloat startRotationRate_;
};

}
