#pragma once

#include "Math/Vector3.h"
#include "Math/Color.h"
#include "Math/RandomStream.h"

namespace FlagGG
{

// Special module indices...
#define INDEX_TYPEDATAMODULE	(INDEX_NONE - 1)
#define INDEX_REQUIREDMODULE	(INDEX_NONE - 2)
#define INDEX_SPAWNMODULE		(INDEX_NONE - 3)

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
	float			rotation;				// Rotation of particle (in Radians)

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

}
