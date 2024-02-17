#pragma once

#include "Math/Vector3.h"
#include "Math/Color.h"
#include "Math/RandomStream.h"

namespace FlagGG
{

namespace FXConsoleVariables
{
	static Int32 MaxCPUParticlesPerEmitter = 1000;
}

enum ParticleDetailMode
{
	PDM_Low,
	PDM_Medium,
	PDM_High,
	PDM_MAX,
};
const UInt32 PDM_DefaultValue = 0xFFFF;

/*-----------------------------------------------------------------------------
	Helper macros.
-----------------------------------------------------------------------------*/
//	Macro fun.
// #define _PARTICLES_USE_PREFETCH_
#if defined(_PARTICLES_USE_PREFETCH_)
#define	PARTICLE_PREFETCH(Index)					PlatformMisc::Prefetch( particleData, particleStride * particleIndices[Index] )
#define PARTICLE_INSTANCE_PREFETCH(Instance, Index)	PlatformMisc::Prefetch( Instance->particleData_, Instance->particleStride_ * Instance->particleIndices_[Index] )
#define	PARTICLE_OWNER_PREFETCH(Index)				PlatformMisc::Prefetch( owner->particleData_, owner->particleStride_ * owner->particleIndices_[Index] )
#else	//#if defined(_PARTICLES_USE_PREFETCH_)
#define	PARTICLE_PREFETCH(Index)					
#define	PARTICLE_INSTANCE_PREFETCH(Instance, Index)	
#define	PARTICLE_OWNER_PREFETCH(Index)				
#endif	//#if defined(_PARTICLES_USE_PREFETCH_)

// Special module indices...
#define INDEX_TYPEDATAMODULE	(INDEX_NONE - 1)
#define INDEX_REQUIREDMODULE	(INDEX_NONE - 2)
#define INDEX_SPAWNMODULE		(INDEX_NONE - 3)

#define MAX_PARTICLE_RESIZE 100
#define MAX_PARTICLE_RESIZE_WARN 80

#define DECLARE_PARTICLE(Name,Address)		\
	BaseParticle& Name = *((BaseParticle*) (Address));

#define DECLARE_PARTICLE_CONST(Name,Address)		\
	const BaseParticle& Name = *((const BaseParticle*) (Address));

#define PARTICLE_ELEMENT(Type,Name)																						\
	Type& Name = *((Type*)((UInt8*)particleBase + currentOffset));																\
	currentOffset += sizeof(Type);

#define DECLARE_PARTICLE_PTR(Name,Address)		\
	BaseParticle* Name = (BaseParticle*) (Address);

#define BEGIN_UPDATE_LOOP																								\
	{																													\
		ASSERT((owner != NULL) && (owner->component_ != NULL));															\
		Int32&			activeParticles = owner->activeParticles_;														\
		UInt32			currentOffset	= offset;																		\
		const UInt8*	particleData	= owner->particleData_;															\
		const UInt32	particleStride	= owner->particleStride_;														\
		UInt16*			particleIndices	= owner->particleIndices_;														\
		for(Int32 i=activeParticles-1; i>=0; i--)																			\
		{																												\
			const Int32	currentIndex	= particleIndices[i];															\
			const UInt8* particleBase	= particleData + currentIndex * particleStride;									\
			BaseParticle& particle		= *((BaseParticle*)particleBase);												\
			if ((particle.flags_ & STATE_Particle_Freeze) == 0)															\
			{																											\

#define END_UPDATE_LOOP																									\
			}																											\
			currentOffset				= offset;																		\
		}																												\
	}

#define CONTINUE_UPDATE_LOOP																							\
		currentOffset = offset;																							\
		continue;

#define SPAWN_INIT																										\
	ASSERT((owner != NULL) && (owner->component_ != NULL));																\
	const Int32		activeParticles	= owner->activeParticles_;															\
	const UInt32	particleStride	= owner->particleStride_;														    \
	UInt32			currentOffset	= offset;																			\
	BaseParticle&	particle		= *(particleBase);

/*-----------------------------------------------------------------------------
	FBaseParticle
-----------------------------------------------------------------------------*/
// Mappings for 'standard' particle data
// Only used when required.
struct BaseParticle
{
	// 24 bytes
	Vector3			oldLocation_;			// Last frame's location, used for collision
	Vector3			location_;				// Current location

	// 16 bytes
	Vector3			baseVelocity_;			// Velocity = BaseVelocity at the start of each frame.
	float			rotation_;				// Rotation of particle (in Radians)

	// 16 bytes
	Vector3			velocity_;				// Current velocity, gets reset to BaseVelocity each frame to allow 
	float			baseRotationRate_;		// Initial angular velocity of particle (in Radians per second)

	// 16 bytes
	Vector3			baseSize_;				// Size = BaseSize at the start of each frame
	float			rotationRate_;			// Current rotation rate, gets reset to BaseRotationRate each frame

	// 16 bytes
	Vector3			size_;					// Current size, gets reset to BaseSize each frame
	Int32			flags_;					// Flags indicating various particle states

	// 16 bytes
	Color			color_;					// Current color of particle.

	// 16 bytes
	Color			baseColor_;				// Base color of the particle

	// 16 bytes
	float			relativeTime_;			// Relative time, range is 0 (==spawn) to 1 (==death)
	float			oneOverMaxLifetime_;	// Reciprocal of lifetime
	float			placeholder0_;
	float			placeholder1_;
};

/** Mesh rotation data payload										*/
struct MeshRotationPayloadData
{
	Vector3	 initialOrientation_;		// from mesh data module
	Vector3  initRotation_;				// from init rotation module
	Vector3  rotation_;
	Vector3	 curContinuousRotation_;
	Vector3  rotationRate_;
	Vector3  rotationRateBase_;
};

struct MeshMotionBlurPayloadData
{
	Vector3 baseParticlePrevVelocity_;
	Vector3 baseParticlePrevSize_;
	Vector3 payloadPrevRotation_;
	Vector3 payloadPrevOrbitOffset_;
	float   baseParticlePrevRotation_;
	float   payloadPrevCameraOffset_;
};

/**
 *	Chain-able Orbit module instance payload
 */
struct OrbitChainModuleInstancePayload
{
	/** The base offset of the particle from it's tracked location	*/
	Vector3	baseOffset_;
	/** The offset of the particle from it's tracked location		*/
	Vector3	offset_;
	/** The rotation of the particle at it's offset location		*/
	Vector3	rotation_;
	/** The base rotation rate of the particle offset				*/
	Vector3	baseRotationRate_;
	/** The rotation rate of the particle offset					*/
	Vector3	rotationRate_;
	/** The offset of the particle from the last frame				*/
	Vector3	previousOffset_;
};

/** Camera offset particle payload */
struct CameraOffsetParticlePayload
{
	/** The base amount to offset the particle towards the camera */
	float	baseOffset_;
	/** The amount to offset the particle towards the camera */
	float	offset_;
};

/** Random-seed instance payload */
struct ParticleRandomSeedInstancePayload
{
	RandomStream randomStream_;
};

/** A level of significance for a particle system. Used by game code to enable/disable emitters progressively as they move away from the camera or are occluded/off screen. */
enum class ParticleSignificanceLevel : UInt8
{
	/** Low significance emitter. Culled first. */
	Low,
	/** Medium significance emitter. */
	Medium,
	/** High significance emitter. Culled last. */
	High,
	/** Critical emitter. Never culled. */
	Critical,

	Num,
};

enum class ParticleSystemInsignificanceReaction : UInt8
{
	/** Looping systems will DisableTick. Non-looping systems will Complete.*/
	Auto,
	/** The system will be considered complete and will auto destroy if desired etc.*/
	Complete,
	/** The system will simply stop ticking. Tick will be re-enabled when any emitters become significant again. This is useful for persistent fx such as environmental fx.  */
	DisableTick,
	/** As DisableTick but will also kill all particles. */
	DisableTickAndKill, //Hidden for now until I make it useful i.e. Killing particles saves memory.

	Num,
};

/*-----------------------------------------------------------------------------
	Particle State Flags
-----------------------------------------------------------------------------*/
enum ParticleStates
{
	/** Ignore updates to the particle						*/
	STATE_Particle_JustSpawned = 0x02000000,
	/** Ignore updates to the particle						*/
	STATE_Particle_Freeze = 0x04000000,
	/** Ignore collision updates to the particle			*/
	STATE_Particle_IgnoreCollisions = 0x08000000,
	/**	Stop translations of the particle					*/
	STATE_Particle_FreezeTranslation = 0x10000000,
	/**	Stop rotations of the particle						*/
	STATE_Particle_FreezeRotation = 0x20000000,
	/** Combination for a single check of 'ignore' flags	*/
	STATE_Particle_CollisionIgnoreCheck = STATE_Particle_Freeze | STATE_Particle_IgnoreCollisions | STATE_Particle_FreezeTranslation | STATE_Particle_FreezeRotation,
	/** Delay collision updates to the particle				*/
	STATE_Particle_DelayCollisions = 0x40000000,
	/** Flag indicating the particle has had at least one collision	*/
	STATE_Particle_CollisionHasOccurred = 0x80000000,
	/** State mask. */
	STATE_Mask = 0xFE000000,
	/** Counter mask. */
	STATE_CounterMask = (~STATE_Mask)
};

/**
 * Per-particle data sent to the GPU.
 */
struct ParticleSpriteVertex
{
	/** The position of the particle. */
	Vector3 position_;
	/** The relative time of the particle. */
	float relativeTime_;
	/** The previous position of the particle. */
	Vector3	oldPosition_;
	/** Value that remains constant over the lifetime of a particle. */
	float particleId_;
	/** The size of the particle. */
	Vector2 size_;
	/** The rotation of the particle. */
	float rotation_;
	/** The sub-image index for the particle. */
	float subImageIndex_;
	/** The color of the particle. */
	Color color_;
	/** The Texcoord of the particle. */
	Vector2 texcoord_;
};

//
//	SubUV-related payloads
//
struct FullSubUVPayload
{
	// The integer portion indicates the sub-image index.
	// The fractional portion indicates the lerp factor.
	float imageIndex_;
	float randomImageTime_;
};

FORCEINLINE Vector3 GetParticleBaseSize(const BaseParticle& particle, bool keepFlipScale = false)
{
	return keepFlipScale ? particle.baseSize_ : Vector3(Abs(particle.baseSize_.x_), Abs(particle.baseSize_.y_), Abs(particle.baseSize_.z_));
}

FORCEINLINE Vector2 GetParticleSizeWithUVFlipInSign(const BaseParticle& particle, const Vector2& scaledSize)
{
	return Vector2(
		particle.baseSize_.x_ >= 0.0f ? scaledSize.x_ : -scaledSize.x_,
		particle.baseSize_.y_ >= 0.0f ? scaledSize.y_ : -scaledSize.y_);
}

}
