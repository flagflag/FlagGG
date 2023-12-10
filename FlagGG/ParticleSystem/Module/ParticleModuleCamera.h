#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "Math/Distributions/DistributionFloat.h"

namespace FlagGG
{

class ParticleModuleTypeDataBase;
struct ParticleEmitterInstance;

class FlagGG_API ParticleModuleCameraBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleCameraBase, ParticleModule);
public:
	ParticleModuleCameraBase();

	virtual bool CanTickInAnyThread() override
	{
		return false;
	}
};

/**
 *	The update method for the offset
 */
enum ParticleCameraOffsetUpdateMethod
{
	EPCOUM_DirectSet,
	EPCOUM_Additive,
	EPCOUM_Scalar,
	EPCOUM_MAX,
};

class FlagGG_API ParticleModuleCameraOffset : public ParticleModuleCameraBase
{
	OBJECT_OVERRIDE(ParticleModuleCameraOffset, ParticleModuleCameraBase);
public:
	ParticleModuleCameraOffset();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//Begin UParticleModule Interface
	virtual void Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	virtual void Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime) override;
	virtual UInt32 RequiredBytes(ParticleModuleTypeDataBase* typeData) override;
	//End UParticleModule Interface

	virtual bool CanTickInAnyThread() override;

	/**
	 *	The camera-relative offset to apply to sprite location
	 */
	RawDistributionFloat cameraOffset_;

	/** If true, the offset will only be processed at spawn time */
	UInt32 spawnTimeOnly_ : 1;

	/**
	 * How to update the offset for this module.
	 * DirectSet - Set the value directly (overwrite any previous setting)
	 * Additive  - Add the offset of this module to the existing offset
	 * Scalar    - Scale the existing offset by the value of this module
	 */
	EnumAsByte<ParticleCameraOffsetUpdateMethod> updateMethod_;
};


}
