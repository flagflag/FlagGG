#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "Math/Distributions/DistributionFloat.h"
#include "Math/Distributions/DistributionVector.h"

namespace FlagGG
{

class ParticleEmitter;
class ParticleModuleTypeDataBase;
class PointLightComponent;
struct ParticleEmitterInstance;

class FlagGG_API ParticleModuleLightBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleLightBase, ParticleModule);
public:
	ParticleModuleLightBase();

	virtual bool CanTickInAnyThread() override
	{
		return false;
	}
};

class FlagGG_API ParticleModuleLight : public ParticleModuleLightBase
{
	OBJECT_OVERRIDE(ParticleModuleLight, ParticleModuleLightBase);
public:
	ParticleModuleLight();

	/** Initializes the default values for this property */
	void InitializeDefaults();


	//Begin UParticleModule Interface
	virtual void Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	virtual void Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime) override;
	virtual UInt32 RequiredBytes(ParticleModuleTypeDataBase* typeData) override;
	virtual void SetToSensibleDefaults(ParticleEmitter* owner) override;
	virtual ModuleType	GetModuleType() const override { return EPMT_Light; }
	//End UParticleModule Interface

	void SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, RandomStream* inRandomStream, BaseParticle* particleBase);

	virtual bool CanTickInAnyThread() override;


	/** Whether to use physically based inverse squared falloff from the light.  If unchecked, the LightExponent distribution will be used instead. */
	bool useInverseSquaredFalloff_;

	/**
	 * Whether lights from this module should affect translucency.
	 * Use with caution.  Modules enabling this should only make a few particle lights at most, and the smaller they are, the less they will cost.
	 */
	bool affectsTranslucency_;

	/**
	 * Will draw wireframe spheres to preview the light radius if enabled.
	 * Note: this is intended for previewing and the value will not be saved, it will always revert to disabled.
	 */
	bool previewLightRadius_;

	/** Fraction of particles in this emitter to create lights on. */
	float spawnFraction_;

	/** Scale that is applied to the particle's color to calculate the light's color, and can be setup as a curve over the particle's lifetime. */
	RawDistributionVector colorScaleOverLife_;

	/** Brightness scale for the light, which can be setup as a curve over the particle's lifetime. */
	RawDistributionFloat brightnessOverLife_;

	/** Scales the particle's radius, to calculate the light's radius. */
	RawDistributionFloat radiusScale_;

	/** Provides the light's exponent when inverse squared falloff is disabled. */
	RawDistributionFloat lightExponent_;

	/**
	* Channels that this light should affect.
	* Only affect high quality lights
	* These channels only apply to opaque materials, direct lighting, and dynamic lighting and shadowing.
	*/
	// LightingChannels lightingChannels_;

	/** Intensity of the volumetric scattering from this light.  This scales Intensity and LightColor. */
	float volumetricScatteringIntensity_;

	/** Converts the particle lights into high quality lights as if they came from a PointLightComponent.  High quality lights cost significantly more on both CPU and GPU. */
	bool highQualityLights_;

	/**
	 * Whether to cast shadows from the particle lights.  Requires High Quality Lights to be enabled.
	 * Warning: This can be incredibly expensive on the GPU - use with caution.
	 */
	bool shadowCastingLights_;

private:

	//UInt64 SpawnHQLight(const LightParticlePayload& Payload, const BaseParticle& Particle, ParticleEmitterInstance* Owner);
	//void UpdateHQLight(Light* PointLightComponent, const LightParticlePayload& Payload, const BaseParticle& Particle, Int32 ScreenAlignment, Vector3 ComponentScale, bool bLocalSpace, SceneInterface* OwnerScene, bool bDoRTUpdate);
};


}
