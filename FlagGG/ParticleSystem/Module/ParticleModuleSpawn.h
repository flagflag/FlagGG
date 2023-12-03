#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "Math/Distributions/DistributionFloat.h"

namespace FlagGG
{

class ParticleModuleSpawnBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleSpawnBase, ParticleModule);
public:
	ParticleModuleSpawnBase();

	//~ Begin UParticleModule Interface
	virtual ModuleType	GetModuleType() const override { return EPMT_Spawn; }
	//~ End UParticleModule Interface

	/**
	 *	Retrieve the spawn amount this module is contributing.
	 *	Note that if multiple Spawn-specific modules are present, if any one
	 *	of them ignores the SpawnRate processing it will be ignored.
	 *
	 *	@param	Owner		The particle emitter instance that is spawning.
	 *	@param	Offset		The offset into the particle payload for the module.
	 *	@param	OldLeftover	The bit of timeslice left over from the previous frame.
	 *	@param	DeltaTime	The time that has expired since the last frame.
	 *	@param	Number		The number of particles to spawn. (OUTPUT)
	 *	@param	Rate		The spawn rate of the module. (OUTPUT)
	 *
	 *	@return	bool		false if the SpawnRate should be ignored.
	 *						true if the SpawnRate should still be processed.
	 */
	virtual bool GetSpawnAmount(ParticleEmitterInstance* owner, Int32 offset, float oldLeftover,
		float deltaTime, Int32& number, float& rate)
	{
		return processSpawnRate_;
	}

	/**
	 *	Retrieve the burst count this module is contributing.
	 *	Note that if multiple Spawn-specific modules are present, if any one
	 *	of them ignores the default BurstList, it will be ignored.
	 *
	 *	@param	Owner		The particle emitter instance that is spawning.
	 *	@param	Offset		The offset into the particle payload for the module.
	 *	@param	OldLeftover	The bit of timeslice left over from the previous frame.
	 *	@param	DeltaTime	The time that has expired since the last frame.
	 *	@param	Number		The number of particles to burst. (OUTPUT)
	 *
	 *	@return	bool		false if the default BurstList should be ignored.
	 *						true if the default BurstList should still be processed.
	 */
	virtual bool GetBurstCount(ParticleEmitterInstance* owner, Int32 offset, float oldLeftover,
		float deltaTime, Int32& number)
	{
		number = 0;
		return processBurstList_;
	}

	/**
	 *	Retrieve the maximum spawn rate for this module...
	 *	Used in estimating the number of particles that could be used.
	 *
	 *	@return	float		The maximum spawn rate
	 */
	virtual float GetMaximumSpawnRate() { return 0.0f; }

	/**
	 *	Retrieve the estimated spawn rate for this module...
	 *	Used in estimating the number of particles that could be used.
	 *
	 *	@return	float			The maximum spawn rate
	 */
	virtual float GetEstimatedSpawnRate() { return 0.0f; }

	/**
	 *	Retrieve the maximum number of particles this module could burst.
	 *	Used in estimating the number of particles that could be used.
	 *
	 *	@return	Int32			The maximum burst count
	 */
	virtual Int32 GetMaximumBurstCount() { return 0; }


	/** 
	 *	If true, the SpawnRate of the SpawnModule of the emitter will be processed.
	 *	If mutliple Spawn modules are 'stacked' in an emitter, if ANY of them 
	 *	have this set to false, it will not process the SpawnModule SpawnRate.
	 */
	UInt32 processSpawnRate_ : 1;

	/** 
	 *	If true, the BurstList of the SpawnModule of the emitter will be processed.
	 *	If mutliple Spawn modules are 'stacked' in an emitter, if ANY of them 
	 *	have this set to false, it will not process the SpawnModule BurstList.
	 */
	UInt32 processBurstList_ : 1;
};

class ParticleModuleSpawn : public ParticleModuleSpawnBase
{
	OBJECT_OVERRIDE(ParticleModuleSpawn, ParticleModuleSpawnBase);
public:
	ParticleModuleSpawn();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//~ Begin UParticleModule Interface
	virtual bool	GenerateLODModuleValues(ParticleModule* sourceModule, float percentage, ParticleLODLevel* LODLevel) override;
	//~ End UParticleModule Interface

	//~ Begin UParticleModuleSpawnBase Interface
	virtual bool GetSpawnAmount(ParticleEmitterInstance* owner, Int32 offset, float oldLeftover,
		float deltaTime, Int32& number, float& rate) override;
	virtual float GetMaximumSpawnRate() override;
	virtual float GetEstimatedSpawnRate() override;
	virtual Int32 GetMaximumBurstCount() override;
	//~ End UParticleModuleSpawnBase Interface

	float GetGlobalRateScale()const;

	/** The rate at which to spawn particles. */
	RawDistributionFloat rate_;

	/** The scalar to apply to the rate. */
	RawDistributionFloat rateScale_;

	/** The method to utilize when burst-emitting particles. */
	EnumAsByte<ParticleBurstMethod> rarticleBurstMethod_;

	/** The array of burst entries. */
	Vector<ParticleBurst> burstList_;

	/** Scale all burst entries by this amount. */
	RawDistributionFloat burstScale_;

	/**	If true, the SpawnRate will be scaled by the global CVar r.EmitterSpawnRateScale */
	UInt32 applyGlobalSpawnRateScale_ : 1;
};

}
