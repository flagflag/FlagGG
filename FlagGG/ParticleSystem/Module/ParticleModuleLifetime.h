#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "Math/Distributions/DistributionFloat.h"

namespace FlagGG
{

class ParticleEmitter;
struct ParticleEmitterInstance;

class FlagGG_API ParticleModuleLifetimeBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleLifetimeBase, ParticleModule);
public:
	ParticleModuleLifetimeBase();

	/** Return the maximum lifetime this module would return. */
	virtual float GetMaxLifetime()
	{
		return 0.0f;
	}

	/**
		*	Return the lifetime value at the given time.
		*
		*	@param	Owner		The emitter instance that owns this module
		*	@param	InTime		The time input for retrieving the lifetime value
		*	@param	Data		The data associated with the distribution
		*
		*	@return	float		The Lifetime value
		*/
	virtual float GetLifetimeValue(ParticleEmitterInstance* owner, float inTime, Object* data = NULL)
		PURE_VIRTUAL(ParticleModuleLifetimeBase::GetLifetimeValue, return 0.0f;);
};


class FlagGG_API ParticleModuleLifetime : public ParticleModuleLifetimeBase
{
	OBJECT_OVERRIDE(ParticleModuleLifetime, ParticleModuleLifetimeBase);
public:
	ParticleModuleLifetime();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//Begin UParticleModule Interface
	virtual void CompileModule(struct ParticleEmitterBuildInfo& emitterInfo) override;
	virtual void Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	virtual void SetToSensibleDefaults(ParticleEmitter* owner) override;
	//End UParticleModule Interface

	//~ Begin UParticleModuleLifetimeBase Interface
	virtual float GetMaxLifetime() override;
	virtual float GetLifetimeValue(ParticleEmitterInstance* owner, float inTime, Object* data = NULL) override;
	//~ End UParticleModuleLifetimeBase Interface

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


	/** The lifetime of the particle, in seconds. Retrieved using the EmitterTime at the spawn of the particle. */
	RawDistributionFloat lifetime_;
};

}
