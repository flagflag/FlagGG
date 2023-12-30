#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "Math/Distributions/DistributionVector.h"

namespace FlagGG
{

class ParticleLODLevel;

class FlagGG_API ParticleModuleLocationBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleLocationBase, ParticleModule);
public:
};

class FlagGG_API ParticleModulePivotOffset : public ParticleModuleLocationBase
{
	OBJECT_OVERRIDE(ParticleModulePivotOffset, ParticleModuleLocationBase);
public:
	ParticleModulePivotOffset();

	/** offset applied in UV space to the particle vertex positions. Defaults to (0.5,0.5) putting the pivot in the centre of the partilce. */
	Vector2 pivotOffset_;

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//Begin ParticleModule Interface
	virtual void CompileModule(struct ParticleEmitterBuildInfo& EmitterInfo) override;
	//End ParticleModule Interface

	// 从XML加载
	bool LoadXML(const XMLElement& root) override;

	// 保存到XML中
	bool SaveXML(XMLElement& root) override;
};

class FlagGG_API ParticleModuleLocation : public ParticleModuleLocationBase
{
	OBJECT_OVERRIDE(ParticleModuleLocation, ParticleModuleLocationBase);
public:
	ParticleModuleLocation();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//Begin ParticleModule Interface
	virtual void Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	//End ParticleModule Interface

	// 从XML加载
	bool LoadXML(const XMLElement& root) override;

	// 保存到XML中
	bool SaveXML(XMLElement& root) override;

	/**
	 *	The location the particle should be emitted.
	 *	Relative in local space to the emitter by default.
	 *	Relative in world space as a WorldOffset module or when the emitter's UseLocalSpace is off.
	 *	Retrieved using the EmitterTime at the spawn of the particle.
	 */
	RawDistributionVector startLocation_;

	/**
	 *  When set to a non-zero value this will force the particles to only spawn on evenly distributed
	 *  positions between the two points specified.
	 */
	float distributeOverNPoints_;

	/**
	 *  When DistributeOverNPoints is set to a non-zero value, this specifies the ratio of particles spawned
	 *  that should use the distribution.  (For example setting this to 1 will cause all the particles to
	 *  be distributed evenly whereas .75 would cause 1/4 of the particles to be randomly placed).
	 */
	float distributeThreshold_;

protected:
	/**
	 *	Extended version of spawn, allows for using a random stream for distribution value retrieval
	 *
	 *	@param	owner				The particle emitter instance that is spawning
	 *	@param	offset				The offset to the modules payload data
	 *	@param	spawnTime			The time of the spawn
	 *	@param	InRandomStream		The random stream to use for retrieving random values
	 */
	virtual void SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, struct RandomStream* inRandomStream, BaseParticle* particleBase);
};


}
