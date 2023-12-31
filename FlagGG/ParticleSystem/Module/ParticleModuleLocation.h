#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "Math/Distributions/DistributionFloat.h"
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

class FlagGG_API ParticleModuleLocationPrimitiveBase : public ParticleModuleLocationBase
{
	OBJECT_OVERRIDE(ParticleModuleLocationPrimitiveBase, ParticleModuleLocationBase);
public:
	ParticleModuleLocationPrimitiveBase();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//@todo document
	virtual void DetermineUnitDirection(ParticleEmitterInstance* owner, Vector3& vUnitDir, struct RandomStream* inRandomStream);

	// 从XML加载
	bool LoadXML(const XMLElement& root) override;

	// 保存到XML中
	bool SaveXML(XMLElement& root) override;


	/** Whether the positive X axis is valid for spawning. */
	UInt32 positive_X_ : 1;

	/** Whether the positive Y axis is valid for spawning. */
	UInt32 positive_Y_ : 1;

	/** Whether the positive Z axis is valid for spawning. */
	UInt32 positive_Z_ : 1;

	/** Whether the negative X axis is valid for spawning. */
	UInt32 negative_X_ : 1;

	/** Whether the negative Y axis is valid for spawning. */
	UInt32 negative_Y_ : 1;

	/** Whether the negative Zaxis is valid for spawning. */
	UInt32 negative_Z_ : 1;

	/** Whether particles will only spawn on the surface of the primitive. */
	UInt32 surfaceOnly_ : 1;

	/** Whether the particle should get its velocity from the position within the primitive. */
	UInt32 velocity_ : 1;

	/** The scale applied to the velocity. (Only used if 'Velocity' is checked). */
	RawDistributionFloat velocityScale_;

	/** The location of the bounding primitive relative to the position of the emitter. */
	RawDistributionVector startLocation_;
};

enum CylinderHeightAxis
{
	PMLPC_HEIGHTAXIS_X,
	PMLPC_HEIGHTAXIS_Y,
	PMLPC_HEIGHTAXIS_Z,
	PMLPC_HEIGHTAXIS_MAX,
};

class FlagGG_API ParticleModuleLocationPrimitiveCylinder : public ParticleModuleLocationPrimitiveBase
{
	OBJECT_OVERRIDE(ParticleModuleLocationPrimitiveCylinder, ParticleModuleLocationPrimitiveBase);
public:
	ParticleModuleLocationPrimitiveCylinder();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//~ Begin ParticleModule Interface
	virtual void Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	//~ End ParticleModule Interface

	/**
	 *	Extended version of spawn, allows for using a random stream for distribution value retrieval
	 *
	 *	@param	Owner				The particle emitter instance that is spawning
	 *	@param	Offset				The offset to the modules payload data
	 *	@param	SpawnTime			The time of the spawn
	 *	@param	InRandomStream		The random stream to use for retrieving random values
	 */
	void SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, RandomStream* inRandomStream, BaseParticle* particleBase);

	// 从XML加载
	bool LoadXML(const XMLElement& root) override;

	// 保存到XML中
	bool SaveXML(XMLElement& root) override;


	/** If true, get the particle velocity form the radial distance inside the primitive. */
	UInt32 radialVelocity_ : 1;

	/** The radius of the cylinder. */
	RawDistributionFloat startRadius_;

	/** The height of the cylinder, centered about the location. */
	RawDistributionFloat startHeight_;

	/**
	 * Determine particle particle system axis that should represent the height of the cylinder.
	 * Can be one of the following:
	 *   PMLPC_HEIGHTAXIS_X - Orient the height along the particle system X-axis.
	 *   PMLPC_HEIGHTAXIS_Y - Orient the height along the particle system Y-axis.
	 *   PMLPC_HEIGHTAXIS_Z - Orient the height along the particle system Z-axis.
	 */
	EnumAsByte<CylinderHeightAxis> heightAxis_;
};

class FlagGG_API ParticleModuleLocationPrimitiveSphere : public ParticleModuleLocationPrimitiveBase
{
	OBJECT_OVERRIDE(ParticleModuleLocationPrimitiveSphere, ParticleModuleLocationPrimitiveBase);
public:
	ParticleModuleLocationPrimitiveSphere();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//~ Begin ParticleModule Interface
	virtual void Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	//~ End ParticleModule Interface

	/**
	 *	Extended version of spawn, allows for using a random stream for distribution value retrieval
	 *
	 *	@param	Owner				The particle emitter instance that is spawning
	 *	@param	Offset				The offset to the modules payload data
	 *	@param	SpawnTime			The time of the spawn
	 *	@param	InRandomStream		The random stream to use for retrieving random values
	 */
	void SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, RandomStream* inRandomStream, BaseParticle* particleBase);

	// 从XML加载
	bool LoadXML(const XMLElement& root) override;

	// 保存到XML中
	bool SaveXML(XMLElement& root) override;


	/** The radius of the sphere. Retrieved using EmitterTime. */
	RawDistributionFloat startRadius_;
};

}
