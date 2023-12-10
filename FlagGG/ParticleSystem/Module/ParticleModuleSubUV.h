#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "Math/Distributions/DistributionFloat.h"
#include "Container/Ptr.h"

namespace FlagGG
{

class ParticleLODLevel;
class SubUVAnimation;

class FlagGG_API ParticleModuleSubUVBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleSubUVBase, ParticleModule);
public:
	// Begin UParticleModule Interface
	virtual ModuleType	GetModuleType() const override { return EPMT_SubUV; }
	//End UParticleModule Interface

};

class FlagGG_API ParticleModuleSubUV : public ParticleModuleSubUVBase
{
	OBJECT_OVERRIDE(ParticleModuleSubUV, ParticleModuleSubUVBase);
public:
	ParticleModuleSubUV();

	~ParticleModuleSubUV() override;

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//~ Begin UParticleModule Interface
	virtual void CompileModule(struct ParticleEmitterBuildInfo& emitterInfo) override;
	virtual void Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	virtual void Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime) override;
	virtual void SetToSensibleDefaults(ParticleEmitter* owner) override;
	//~ End UParticleModule Interface

	/**
	 *	Determine the current image index to use...
	 *
	 *	@param	Owner					The emitter instance being updated.
	 *	@param	Offset					The offset to the particle payload for this module.
	 *	@param	Particle				The particle that the image index is being determined for.
	 *	@param	InterpMethod			The ParticleSubUVInterpMethod method used to update the subUV.
	 *	@param	SubUVPayload			The FullSubUVPayload for this particle.
	 *
	 *	@return	float					The image index with interpolation amount as the fractional portion.
	 */
	virtual float DetermineImageIndex(ParticleEmitterInstance* owner, Int32 offset, BaseParticle* particle,
		ParticleSubUVInterpMethod interpMethod, FullSubUVPayload& subUVPayload, float deltaTime);

#if WITH_EDITOR
	virtual bool IsValidForLODLevel(ParticleLODLevel* LODLevel, String& outErrorString) override;
#endif

	/**
	 * SubUV animation asset to use.
	 * When specified, optimal bounding geometry for each SubUV frame will be used when rendering the sprites for this emitter instead of full quads.
	 * This reduction in overdraw can reduce the GPU cost of rendering the emitter by 2x or 3x, depending on how much unused space was in the texture.
	 * The bounding geometry is generated off of the texture alpha setup in the SubUV Animation asset, so that has to match what the material is using for opacity, or clipping will occur.
	 * When specified, SubImages_Horizontal and SubImages_Vertical will come from the asset instead of the Required Module.
	 */
	SharedPtr<SubUVAnimation> animation_;

	/**
	 *	The index of the sub-image that should be used for the particle.
	 *	The value is retrieved using the RelativeTime of the particles.
	 */
	RawDistributionFloat subImageIndex_;

	/**
	 *	If true, use *real* time when updating the image index.
	 *	The movie will update regardless of the slomo settings of the game.
	 */
	UInt8 useRealTime_ : 1;

protected:
	friend class ParticleModuleSubUVDetails;
};


}

