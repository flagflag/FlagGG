#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "Math/Distributions/DistributionFloat.h"
#include "Math/Distributions/DistributionFloatConstant.h"

namespace FlagGG
{

class InterpCurveEdSetup;
class ParticleEmitter;
class ParticleLODLevel;
class ParticleModuleTypeDataBase;
class Material;

class FlagGG_API ParticleModuleParameterBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleParameterBase, ParticleModule);
public:
	ParticleModuleParameterBase();

	virtual bool CanTickInAnyThread() override
	{
		return false;
	}

};

/**
 *	EmitterDynamicParameterValue
 *	Enumeration indicating the way a dynamic parameter should be set.
 */
enum EmitterDynamicParameterValue
{
	/** UserSet - use the user set values in the distribution (the default) */
	EDPV_UserSet,
	/** AutoSet - ignore values set in the distribution, another module will handle this data */
	EDPV_AutoSet,
	/** VelocityX - pass the particle velocity along the X-axis thru */
	EDPV_VelocityX,
	/** VelocityY - pass the particle velocity along the Y-axis thru */
	EDPV_VelocityY,
	/** VelocityZ - pass the particle velocity along the Z-axis thru */
	EDPV_VelocityZ,
	/** VelocityMag - pass the particle velocity magnitude thru */
	EDPV_VelocityMag,
	EDPV_MAX,
};

/** Helper structure for displaying the parameter. */
struct EmitterDynamicParameter
{
	EmitterDynamicParameter()
		: useEmitterTime_(false)
		, spawnTimeOnly_(false)
		, valueMethod_(0)
		, scaleVelocityByParamValue_(false)
	{
	}
	EmitterDynamicParameter(const String& inParamName, UInt32 inUseEmitterTime, EnumAsByte<EmitterDynamicParameterValue> inValueMethod, DistributionFloatConstant* inDistribution)
		: paramName_(inParamName)
		, useEmitterTime_(inUseEmitterTime)
		, spawnTimeOnly_(false)
		, valueMethod_(inValueMethod)
		, scaleVelocityByParamValue_(false)
	{
		paramValue_.distribution_ = inDistribution;
	}

	/** The parameter name - from the material DynamicParameter expression. READ-ONLY */
	String paramName_;

	/** If true, use the EmitterTime to retrieve the value, otherwise use Particle RelativeTime. */
	UInt32 useEmitterTime_ : 1;

	/** If true, only set the value at spawn time of the particle, otherwise update each frame. */
	UInt32 spawnTimeOnly_ : 1;

	/** Where to get the parameter value from. */
	EnumAsByte<EmitterDynamicParameterValue> valueMethod_;

	/** If true, scale the velocity value selected in ValueMethod by the evaluated ParamValue. */
	UInt32 scaleVelocityByParamValue_ : 1;

	/** The distriubtion for the parameter value. */
	RawDistributionFloat paramValue_;
};

class FlagGG_API ParticleModuleParameterDynamic : public ParticleModuleParameterBase
{
	OBJECT_OVERRIDE(ParticleModuleParameterDynamic, ParticleModuleParameterBase);
public:
	ParticleModuleParameterDynamic();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//~ Begin UParticleModule Interface
	virtual void	Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	virtual void	Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime) override;
	virtual UInt32	RequiredBytes(ParticleModuleTypeDataBase* typeData) override;
	virtual void SetToSensibleDefaults(ParticleEmitter* owner) override;
	virtual void	GetCurveObjects(Vector<ParticleCurvePair>& outCurves) override;
	virtual bool WillGeneratedModuleBeIdentical(ParticleLODLevel* sourceLODLevel, ParticleLODLevel* destLODLevel, float percentage) override
	{
		// The assumption is that at 100%, ANY module will be identical...
		// (Although this is virtual to allow over-riding that assumption on a case-by-case basis!)
		return true;
	}
	virtual void GetParticleSysParamsUtilized(Vector<String>& particleSysParamList) override;
	virtual void GetParticleParametersUtilized(Vector<String>& particleParameterList) override;
	virtual void RefreshModule(InterpCurveEdSetup* edSetup, ParticleEmitter* inEmitter, Int32 inLODLevel) override;
	//~ End UParticleModule Interface

	/**
	 *	Extended version of spawn, allows for using a random stream for distribution value retrieval
	 *
	 *	@param	owner				The particle emitter instance that is spawning
	 *	@param	offset				The offset to the modules payload data
	 *	@param	spawnTime			The time of the spawn
	 *	@param	InRandomStream		The random stream to use for retrieving random values
	 */
	void SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, RandomStream* inRandomStream, BaseParticle* particleBase);

	/**
	 *	Update the parameter names with the given material...
	 *
	 *	@param	InMaterialInterface	Pointer to the material interface
	 *	@param	bIsMeshEmitter		true if the emitter is a mesh emitter...
	 *
	 */
	virtual void UpdateParameterNames(Material* inMaterialInterface);


	/**
	 *	Retrieve the value for the parameter at the given index.
	 *
	 *	@param	InDynParams		The FEmitterDynamicParameter to fetch the value for
	 *	@param	Particle		The particle we are getting the value for.
	 *	@param	owner			The ParticleEmitterInstance owner of the particle.
	 *	@param	InRandomStream	The random stream to use when retrieving the value
	 *
	 *	@return	float			The value for the parameter.
	 */
	float GetParameterValue(EmitterDynamicParameter& inDynParams, BaseParticle& particle, ParticleEmitterInstance* owner, RandomStream* inRandomStream);

	/**
	 *	Retrieve the value for the parameter at the given index.
	 *
	 *	@param	InDynParams		The FEmitterDynamicParameter to fetch the value for
	 *	@param	Particle		The particle we are getting the value for.
	 *	@param	owner			The ParticleEmitterInstance owner of the particle.
	 *	@param	InRandomStream	The random stream to use when retrieving the value
	 *
	 *	@return	float			The value for the parameter.
	 */
	float GetParameterValue_UserSet(EmitterDynamicParameter& inDynParams, BaseParticle& particle, ParticleEmitterInstance* owner, RandomStream* inRandomStream);

	/**
	 *	Set the UpdatesFlags and bUsesVelocity
	 */
	virtual	void UpdateUsageFlags();

	virtual bool CanTickInAnyThread() override;


	/** The dynamic parameters this module uses. */
	Vector<EmitterDynamicParameter> dynamicParams_;

	/** Flags for optimizing update */
	Int32 updateFlags_;

	UInt32 usesVelocity_ : 1;
};


}
