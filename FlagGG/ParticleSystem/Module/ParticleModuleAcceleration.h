#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "Math/Distributions/DistributionFloat.h"
#include "Math/Distributions/DistributionVector.h"

namespace FlagGG
{

class ParticleEmitter;
class ParticleLODLevel;
class ParticleModuleTypeDataBase;
struct ParticleEmitterInstance;

class ParticleModuleAccelerationBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleAccelerationBase, ParticleModule);
public:

	//~ Begin UParticleModule Interface
	virtual void SetToSensibleDefaults(ParticleEmitter* owner) override;
	//~ End UParticleModule Interface

	/**
	 *	If true, then treat the acceleration as world-space
	 */
	UInt8 alwaysInWorldSpace_ : 1;
};

class ParticleModuleAcceleration : public ParticleModuleAccelerationBase
{
	OBJECT_OVERRIDE(ParticleModuleAcceleration, ParticleModuleAccelerationBase);
public:
	ParticleModuleAcceleration();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//Begin ParticleModule Interface
	virtual void CompileModule(struct ParticleEmitterBuildInfo& emitterInfo) override;
	virtual void Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	virtual void Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime) override;
	virtual UInt32 RequiredBytes(ParticleModuleTypeDataBase* typeData) override;
	//End ParticleModule Interface

	// 从XML加载
	bool LoadXML(const XMLElement& root) override;

	// 保存到XML中
	bool SaveXML(XMLElement& root) override;


	/**
	 *	The initial acceleration of the particle.
	 *	Value is obtained using the EmitterTime at particle spawn.
	 *	Each frame, the current and base velocity of the particle
	 *	is then updated using the formula
	 *		velocity += acceleration * DeltaTime
	 *	where DeltaTime is the time passed since the last frame.
	 */
	RawDistributionVector acceleration_;

	/**
	 *	If true, then apply the particle system components scale
	 *	to the acceleration value.
	 */
	UInt32 applyOwnerScale_ : 1;

protected:
	friend class ParticleModuleAccelerationDetails;
};

}

