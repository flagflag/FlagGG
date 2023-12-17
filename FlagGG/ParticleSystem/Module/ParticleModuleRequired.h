#pragma once

#include "Core/BaseMacro.h"
#include "Core/BaseTypes.h"
#include "Math/RandomStream.h"
#include "Math/Vector3.h"
#include "Math/Distributions/DistributionFloat.h"
#include "Container/EnumAsByte.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Material.h"
#include "ParticleSystem/ParticleModule.h"
#include "ParticleSystem/SubUVAnimation.h"
#include "ParticleSystem/Emitter/ParticleSpriteEmitter.h"
#include "Core/CryAssert.h"

namespace FlagGG
{

class InterpCurveEdSetup;
class ParticleLODLevel;
struct CurveEdEntry;

enum class ParticleUVFlipMode : UInt8
{
	/** Flips UV on all particles. */
	None,
	/** Flips UV on all particles. */
	FlipUV,
	/** Flips U only on all particles. */
	FlipUOnly,
	/** Flips V only on all particles. */
	FlipVOnly,
	/** Flips UV randomly for each particle on spawn. */
	RandomFlipUV,
	/** Flips U only randomly for each particle on spawn. */
	RandomFlipUOnly,
	/** Flips V only randomly for each particle on spawn. */
	RandomFlipVOnly,
	/** Flips U and V independently at random for each particle on spawn. */
	RandomFlipUVIndependent,
};

/** Flips the sign of a particle's base size based on it's UV flip mode. */
FORCEINLINE void AdjustParticleBaseSizeForUVFlipping(Vector3& outSize, ParticleUVFlipMode flipMode, RandomStream& randomStream)
{
	static const float HalfRandMax = 0.5f;

	switch (flipMode)
	{
		case ParticleUVFlipMode::None:
			return;

		case ParticleUVFlipMode::FlipUV:
			outSize = -outSize;
			return;
		
		case ParticleUVFlipMode::FlipUOnly:
			outSize.x_ = -outSize.x_;
			return;
		
		case ParticleUVFlipMode::FlipVOnly:
			outSize.y_ = -outSize.y_;
			return;
		
		case ParticleUVFlipMode::RandomFlipUV:
			outSize = randomStream.FRand() > HalfRandMax ? -outSize : outSize;
			return;

		case ParticleUVFlipMode::RandomFlipUOnly:
			outSize.x_ = randomStream.FRand() > HalfRandMax ? -outSize.x_ : outSize.x_;
			return;

		case ParticleUVFlipMode::RandomFlipVOnly:
			outSize.y_ = randomStream.FRand() > HalfRandMax ? -outSize.y_ : outSize.y_;
			return;

		case ParticleUVFlipMode::RandomFlipUVIndependent:
			outSize.x_ = randomStream.FRand() > HalfRandMax ? -outSize.x_ : outSize.x_;
			outSize.y_ = randomStream.FRand() > HalfRandMax ? -outSize.y_ : outSize.y_;
			return;

		default:
			ASSERT(false);
			break;
	}
}

enum ParticleSortMode
{
	PSORTMODE_None,
	PSORTMODE_ViewProjDepth,
	PSORTMODE_DistanceToView,
	PSORTMODE_Age_OldestFirst,
	PSORTMODE_Age_NewestFirst,
	PSORTMODE_MAX,
};

enum EmitterNormalsMode
{
	/** Default mode, normals are based on the camera facing geometry. */
	ENM_CameraFacing,
	/** Normals are generated from a sphere centered at NormalsSphereCenter. */
	ENM_Spherical,
	/** Normals are generated from a cylinder going through NormalsSphereCenter, in the direction NormalsCylinderDirection. */
	ENM_Cylindrical,
	ENM_MAX,
};

struct ParticleRequiredModule
{
	UInt32 numFrames_;
	UInt32 numBoundingVertices_;
	UInt32 numBoundingTriangles_;
	float alphaThreshold_;
	PODVector<Vector2> frameData_;
	// FRHIShaderResourceView* BoundingGeometryBufferSRV;
	UInt8 cutoutTexureIsValid_ : 1;
};


class FlagGG_API ParticleModuleRequired : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleRequired, ParticleModule);
public:
	/** Initializes the default values for this property */
	void InitializeDefaults();

	//~ Begin UParticleModule Interface
	virtual void SetToSensibleDefaults(ParticleEmitter* Owner) override;
	virtual	bool AddModuleCurvesToEditor(InterpCurveEdSetup* EdSetup, PODVector<const CurveEdEntry*>& OutCurveEntries) override
	{
		// Overide the base implementation to prevent old SpawnRate from being added...
		return true;
	}
	virtual ModuleType	GetModuleType() const override {	return EPMT_Required;	}
	virtual bool GenerateLODModuleValues(ParticleModule* SourceModule, float Percentage, ParticleLODLevel* LODLevel) override;
	//~ End UParticleModule Interface

	inline Int32 GetNumFrames() const
	{
		return subImages_Vertical_ * subImages_Horizontal_;
	}

	inline bool IsBoundingGeometryValid() const
	{
		return cutoutTexture_ != nullptr;
	}

	//inline FRHIShaderResourceView* GetBoundingGeometrySRV() const
	//{
	//	return BoundingGeometryBuffer->ShaderResourceView;
	//}

	inline Int32 GetNumBoundingVertices() const
	{
		return boundingMode_ == BVC_FourVertices ? 4 : 8;
	}

	inline Int32 GetNumBoundingTriangles() const
	{
		return boundingMode_ == BVC_FourVertices ? 2 : 6;
	}

	inline const Vector2* GetFrameData(Int32 frameIndex) const
	{
		return &derivedData_.boundingGeometry_[frameIndex * GetNumBoundingVertices()];
	}

	ParticleRequiredModule *CreateRendererResource()
	{
		ParticleRequiredModule *FReqMod = new ParticleRequiredModule();
		FReqMod->numFrames_ = GetNumFrames();
		FReqMod->frameData_ = derivedData_.boundingGeometry_;
		FReqMod->numBoundingVertices_ = GetNumBoundingVertices();
		FReqMod->numBoundingTriangles_ = GetNumBoundingTriangles();
		ASSERT(FReqMod->numBoundingTriangles_ == 2 || FReqMod->numBoundingTriangles_ == 6);
		FReqMod->alphaThreshold_ = alphaThreshold_;
		// FReqMod->BoundingGeometryBufferSRV = GetBoundingGeometrySRV();
		return FReqMod;
	}

	//
	// General.
	// 

	/** The material to utilize for the emitter at this LOD level.						*/
	SharedPtr<Material> material_;

	/** The distance at which PSA_FacingCameraDistanceBlend	is fully PSA_Square */
	float minFacingCameraBlendDistance_;

	/** The distance at which PSA_FacingCameraDistanceBlend	is fully PSA_FacingCameraPosition */
	float maxFacingCameraBlendDistance_;

	Vector3 emitterOrigin_;

	// Å·À­½Ç
	Vector3 emitterRotation_;

	/**
	 *	The screen alignment to utilize for the emitter at this LOD level.
	 *	One of the following:
	 *	PSA_FacingCameraPosition - Faces the camera position, but is not dependent on the camera rotation.
	 *								This method produces more stable particles under camera rotation.
	 *	PSA_Square			- Uniform scale (via SizeX) facing the camera
	 *	PSA_Rectangle		- Non-uniform scale (via SizeX and SizeY) facing the camera
	 *	PSA_Velocity		- Orient the particle towards both the camera and the direction
	 *						  the particle is moving. Non-uniform scaling is allowed.
	 *	PSA_TypeSpecific	- Use the alignment method indicated in the type data module.
	 *	PSA_FacingCameraDistanceBlend - Blends between PSA_FacingCameraPosition and PSA_Square over specified distance.
	 */
	EnumAsByte<ParticleScreenAlignment> screenAlignment_;

	/** If true, update the emitter in local space										*/
	UInt8 useLocalSpace_ : 1;

	/** If true, kill the emitter when the particle system is deactivated				*/
	UInt8 killOnDeactivate_ : 1;

	/** If true, kill the emitter when it completes										*/
	UInt8 killOnCompleted_ : 1;

	/**
	 *	The sorting mode to use for this emitter.
	 *	PSORTMODE_None				- No sorting required.
	 *	PSORTMODE_ViewProjDepth		- Sort by view projected depth of the particle.
	 *	PSORTMODE_DistanceToView	- Sort by distance of particle to view in world space.
	 *	PSORTMODE_Age_OldestFirst	- Sort by age, oldest drawn first.
	 *	PSORTMODE_Age_NewestFirst	- Sort by age, newest drawn first.
	 *
	 */
	EnumAsByte<ParticleSortMode> sortMode_;

	/**
	 *	If true, the EmitterTime for the emitter will be calculated by
	 *	modulating the SecondsSinceCreation by the EmitterDuration. As
	 *	this can lead to issues w/ looping and variable duration, a new
	 *	approach has been implemented.
	 *	If false, this new approach is utilized, and the EmitterTime is
	 *	simply incremented by DeltaTime each tick. When the emitter
	 *	loops, it adjusts the EmitterTime by the current EmitterDuration
	 *	resulting in proper looping/delay behavior.
	 */
	UInt8 useLegacyEmitterTime_ : 1;

	/** If true, removes the HMD view roll (e.g. in VR) */
	UInt8 removeHMDRoll_ : 1;

	/** If true, gpu simulation positions are offset to support double precision vectors. Cpu sims always support large world coordinates. */
	UInt8 supportLargeWorldCoordinates_ : 1;

	/**
	 *	If true, select the emitter duration from the range
	 *		[EmitterDurationLow..EmitterDuration]
	 */
	UInt8 emitterDurationUseRange_ : 1;

	/**
	 *	How long, in seconds, the emitter will run before looping.
	 */
	float emitterDuration_;

	//
	// Spawn-related.
	//

	/** The rate at which to spawn particles									*/
	RawDistributionFloat spawnRate_;

	/** The array of burst entries.												*/
	Vector<ParticleBurst> burstList_;

	//
	// Delay-related.
	//

	/**
	 *	Indicates the time (in seconds) that this emitter should be delayed in the particle system.
	 */
	float emitterDelay_;

	/**
	 *	The low end of the emitter delay if using a range.
	 */
	float emitterDelayLow_;

	/**
	 *	If true, the emitter will be delayed only on the first loop.
	 */
	UInt8 delayFirstLoopOnly_ : 1;

	//
	// SubUV-related.
	//

	/**
	 *	The interpolation method to used for the SubUV image selection.
	 *	One of the following:
	 *	PSUVIM_None			- Do not apply SubUV modules to this emitter.
	 *	PSUVIM_Linear		- Smoothly transition between sub-images in the given order,
	 *						  with no blending between the current and the next
	 *	PSUVIM_Linear_Blend	- Smoothly transition between sub-images in the given order,
	 *						  blending between the current and the next
	 *	PSUVIM_Random		- Pick the next image at random, with no blending between
	 *						  the current and the next
	 *	PSUVIM_Random_Blend	- Pick the next image at random, blending between the current
	 *						  and the next
	 */
	EnumAsByte<ParticleSubUVInterpMethod> interpolationMethod_;

	/** Whether to scale the UV or not - ie, the model wasn't setup with sub uvs		*/
	UInt8 scaleUV_ : 1;

	/**
	 *	If true, select the emitter delay from the range
	 *		[EmitterDelayLow..EmitterDelay]
	 */
	UInt8 emitterDelayUseRange_ : 1;

	/** The method to utilize when burst-emitting particles						*/
	EnumAsByte<ParticleBurstMethod> particleBurstMethod_;

	/** Override the system MacroUV settings                                            */
	UInt8 overrideSystemMacroUV_ : 1;

	/**
	 *	If true, use the MaxDrawCount to limit the number of particles rendered.
	 *	NOTE: This does not limit the number spawned/updated, only what is drawn.
	 */
	UInt8 useMaxDrawCount_ : 1;

	EnumAsByte<OpacitySourceMode> opacitySourceMode_;

	/** Normal generation mode for this emitter LOD. */
	EnumAsByte<EmitterNormalsMode> emitterNormalsMode_;

	/**
	* Ensures that movement generated from the orbit module is applied to velocity-aligned particles
	*/
	UInt8 orbitModuleAffectsVelocityAlignment_ : 1;

	/** The number of sub-images horizontally in the texture							*/
	Int32 subImages_Horizontal_;

	/** The number of sub-images vertically in the texture								*/
	Int32 subImages_Vertical_;

	/**
	 *	The amount of time (particle-relative, 0.0 to 1.0) to 'lock' on a random sub image
	 *	    0.0 = change every frame
	 *      1.0 = select a random image at spawn and hold for the life of the particle
	 */
	float randomImageTime_;

	/** The number of times to change a random image over the life of the particle.		*/
	Int32 randomImageChanges_;

	/** Local space position that UVs generated with the ParticleMacroUV material node will be centered on. */
	Vector3 macroUVPosition_;

	/** World space radius that UVs generated with the ParticleMacroUV material node will tile based on. */
	float macroUVRadius_;

	/**
	 * Controls UV Flipping for this emitter.
	 */
	ParticleUVFlipMode UVFlippingMode;

	/**
	 * More bounding vertices results in reduced overdraw, but adds more triangle overhead.
	 * The eight vertex mode is best used when the SubUV texture has a lot of space to cut out that is not captured by the four vertex version,
	 * and when the particles using the texture will be few and large.
	 */
	EnumAsByte<SubUVBoundingVertexCount> boundingMode_;

	/**
	 *	If true, recalculate the emitter duration on each loop.
	 */
	UInt8 durationRecalcEachLoop_ : 1;

	/**
	 * When EmitterNormalsMode is ENM_Spherical, particle normals are created to face away from NormalsSphereCenter.
	 * NormalsSphereCenter is in local space.
	 */
	Vector3 normalsSphereCenter_;

	/**
	* Alpha channel values larger than the threshold are considered occupied and will be contained in the bounding geometry.
	* Raising this threshold slightly can reduce overdraw in particles using this animation asset.
	*/
	float alphaThreshold_;

	/** The number of times to loop the emitter.
	 *	0 indicates loop continuously
	 */
	Int32 emitterLoops_;

	/**
	* Texture to generate bounding geometry from.
	*/
	SharedPtr<Texture2D> cutoutTexture_;

	/**
	 *	The maximum number of particles to DRAW for this emitter.
	 *	If set to 0, it will use whatever number are present.
	 */
	Int32 maxDrawCount_;

	/**
	 *	The low end of the emitter duration if using a range.
	 */
	float emitterDurationLow_;

	/**
	 * When EmitterNormalsMode is ENM_Cylindrical,
	 * particle normals are created to face away from the cylinder going through NormalsSphereCenter in the direction NormalsCylinderDirection.
	 * NormalsCylinderDirection is in local space.
	 */
	Vector3 normalsCylinderDirection_;

	/**
	*	Named material overrides for this emitter.
	*	Overrides this emitter's material(s) with those in the correspondingly named slot(s) of the owning system.
	*/
	Vector<String> namedMaterialOverrides_;

protected:
	friend class FParticleModuleRequiredDetails;
	friend struct FParticleEmitterInstance;

private:
	void CacheDerivedData();
	void InitBoundingGeometryBuffer();
	void GetDefaultCutout();

	/** Derived data for this asset, generated off of SubUVTexture. */
	SubUVDerivedData derivedData_;

	/** Tracks progress of BoundingGeometryBuffer release during destruction. */
	// RenderCommandFence releaseFence_;

	/** Used on platforms that support instancing, the bounding geometry is fetched from a vertex shader instead of on the CPU. */
	// SubUVBoundingGeometryBuffer* boundingGeometryBuffer_;
};


}
