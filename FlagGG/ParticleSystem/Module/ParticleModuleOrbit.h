#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "Math/Distributions/DistributionVector.h"

namespace FlagGG
{

class ParticleLODLevel;
class ParticleModuleTypeDataBase;
struct ParticleEmitterInstance;

class FlagGG_API ParticleModuleOrbitBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleOrbitBase, ParticleModule);
public:

	/** 
	 *	If true, distribution values will be retrieved using the EmitterTime.
	 *	If false (default), they will be retrieved using the Particle.RelativeTime.
	 */
	UInt32 useEmitterTime_ : 1;

};

enum OrbitChainMode
{
	/** Add the module values to the previous results						*/
	EOChainMode_Add,
	/**	Multiply the module values by the previous results					*/
	EOChainMode_Scale,
	/**	'Break' the chain and apply the values from the	previous results	*/
	EOChainMode_Link,
	EOChainMode_MAX,
};

/**
 *	Container struct for holding options on the data updating for the module.
 */
struct OrbitOptions
{
	OrbitOptions()
		: processDuringSpawn_(true)
		, processDuringUpdate_(false)
		, useEmitterTime_(false)
	{
	}

	/**
	 *	Whether to process the data during spawning.
	 */
	UInt32 processDuringSpawn_ : 1;

	/**
	 *	Whether to process the data during updating.
	 */
	UInt32 processDuringUpdate_ : 1;

	/**
	 *	Whether to use emitter time during data retrieval.
	 */
	UInt32 useEmitterTime_ : 1;
};

class FlagGG_API ParticleModuleOrbit : public ParticleModuleOrbitBase
{
	OBJECT_OVERRIDE(ParticleModuleOrbit, ParticleModuleOrbitBase);
public:
	ParticleModuleOrbit();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//Begin ParticleModule Interface
	virtual void	CompileModule(ParticleEmitterBuildInfo& emitterInfo) override;
	virtual void	Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	virtual void	Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime) override;
	virtual UInt32	RequiredBytes(ParticleModuleTypeDataBase* typeData) override;
	virtual UInt32	RequiredBytesPerInstance() override;
	//End ParticleModule Interface

	// 从XML加载
	bool LoadXML(const XMLElement& root) override;

	// 保存到XML中
	bool SaveXML(XMLElement& root) override;


	/**
	 *	Orbit modules will chain together in the order they appear in the module stack.
	 *	The combination of a module with the one prior to it is defined by using one
	 *	of the following enumerations:
	 *		EOChainMode_Add		Add the values to the previous results
	 *		EOChainMode_Scale	Multiply the values by the previous results
	 *		EOChainMode_Link	'Break' the chain and apply the values from the	previous results
	 */
	EnumAsByte<OrbitChainMode> chainMode_;

	/** The amount to offset the sprite from the particle position. */
	RawDistributionVector offsetAmount_;

	/** The options associated with the OffsetAmount look-up. */
	OrbitOptions offsetOptions_;

	/**
	 *	The amount (in 'turns') to rotate the offset about the particle position.
	 *		0.0 = no rotation
	 *		0.5	= 180 degree rotation
	 *		1.0 = 360 degree rotation
	 */
	RawDistributionVector rotationAmount_;

	/** The options associated with the RotationAmount look-up. */
	OrbitOptions rotationOptions_;

	/**
	 *	The rate (in 'turns') at which to rotate the offset about the particle positon.
	 *		0.0 = no rotation
	 *		0.5	= 180 degree rotation
	 *		1.0 = 360 degree rotation
	 */
	RawDistributionVector rotationRateAmount_;

	/** The options associated with the RotationRateAmount look-up. */
	OrbitOptions rotationRateOptions_;

protected:
	friend class ParticleModuleOrbitDetails;
};

}
