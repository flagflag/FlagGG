#pragma once

#include "Container/Vector.h"
#include "Math/Vector2.h"
#include "Math/Distributions/Distributions.h"

namespace FlagGG
{

class ParticleModule;
class ParticleModuleRequired;
class ParticleModuleSpawn;
class ParticleModuleSpawnPerUnit;

/*-----------------------------------------------------------------------------
	Information compiled from modules to build runtime emitter data.
-----------------------------------------------------------------------------*/

struct FlagGG_API ParticleEmitterBuildInfo
{
	/** The required module. */
	ParticleModuleRequired* requiredModule_;
	/** The spawn module. */
	ParticleModuleSpawn* spawnModule_;
	/** The spawn-per-unit module. */
	ParticleModuleSpawnPerUnit* spawnPerUnitModule_;
	/** List of spawn modules that need to be invoked at runtime. */
	PODVector<ParticleModule*> spawnModules_;

	/** The accumulated orbit offset. */
	ComposableVectorDistribution orbitOffset_;
	/** The accumulated orbit initial rotation. */
	ComposableVectorDistribution orbitInitialRotation_;
	/** The accumulated orbit rotation rate. */
	ComposableVectorDistribution orbitRotationRate_;

	/** The color scale of a particle over time. */
	ComposableVectorDistribution colorScale_;
	/** The alpha scale of a particle over time. */
	ComposableFloatDistribution alphaScale_;

	/** An additional color scale for allowing parameters to be used for ColorOverLife modules. */
	RawDistributionVector dynamicColor_;
	/** An additional alpha scale for allowing parameters to be used for ColorOverLife modules. */
	RawDistributionFloat dynamicAlpha_;

	/** An additional color scale for allowing parameters to be used for ColorScaleOverLife modules. */
	RawDistributionVector dynamicColorScale_;
	/** An additional alpha scale for allowing parameters to be used for ColorScaleOverLife modules. */
	RawDistributionFloat dynamicAlphaScale_;
	
	/** How to scale a particle's size over time. */
	ComposableVectorDistribution sizeScale_;
	/** The maximum size of a particle. */
	Vector2 maxSize_;
	/** How much to scale a particle's size based on its speed. */
	Vector2 sizeScaleBySpeed_;
	/** The maximum amount by which to scale a particle based on its speed. */
	Vector2 maxSizeScaleBySpeed_;

	/** The sub-image index over the particle's life time. */
	ComposableFloatDistribution subImageIndex_;

	/** Drag coefficient. */
	ComposableFloatDistribution dragCoefficient_;
	/** Drag scale over life. */
	ComposableFloatDistribution dragScale_;

	/** Enable collision? */
	bool enableCollision_;
	/** How particles respond to collision. */
	UInt8 collisionResponse_;
	UInt8 collisionMode_;
	/** Radius scale applied to friction. */
	float collisionRadiusScale_;
	/** Bias applied to the collision radius. */
	float collisionRadiusBias_;
	/** Factor reflection spreading cone when colliding. */
	float collisionRandomSpread_;
	/** Random distribution across the reflection spreading cone when colliding. */
	float collisionRandomDistribution_;
	/** Friction. */
	float friction_;
	/** Collision damping factor. */
	ComposableFloatDistribution resilience_;
	/** Collision damping factor scale over life. */
	ComposableFloatDistribution resilienceScaleOverLife_;

	/** Location of a point source attractor. */
	Vector3 pointAttractorPosition_;
	/** Radius of the point source attractor. */
	float pointAttractorRadius_;
	/** Strength of the point attractor. */
	ComposableFloatDistribution pointAttractorStrength_;

	/** The per-particle vector field scale. */
	ComposableFloatDistribution vectorFieldScale_;
	/** The per-particle vector field scale-over-life. */
	ComposableFloatDistribution vectorFieldScaleOverLife_;
	/** Global vector field scale. */
	float globalVectorFieldScale_;
	/** Global vector field tightness. */
	float globalVectorFieldTightness_;

	/** Local vector field. */
	class UVectorField* localVectorField_;
	/** Local vector field transform. */
	FTransform localVectorFieldTransform_;
	/** Local vector field intensity. */
	float localVectorFieldIntensity_;
	/** Tightness tweak for local vector fields. */
	float localVectorFieldTightness_;
	/** Minimum initial rotation applied to local vector fields. */
	Vector3 localVectorFieldMinInitialRotation_;
	/** Maximum initial rotation applied to local vector fields. */
	Vector3 localVectorFieldMaxInitialRotation_;
	/** Local vector field rotation rate. */
	Vector3 localVectorFieldRotationRate_;

	/** Constant acceleration to apply to particles. */
	Vector3 constantAcceleration_;

	/** The maximum lifetime of any particle that will spawn. */
	float maxLifetime_;
	/** The maximum rotation rate of particles. */
	float maxRotationRate_;
	/** The estimated maximum number of particles for this emitter. */
	Int32 estimatedMaxActiveParticleCount_;

	Int32 screenAlignment_;

	/** An offset in UV space for the positioning of a sprites verticies. */
	Vector2 pivotOffset_;

	/** If true, local vector fields ignore the component transform. */
	UInt32 localVectorFieldIgnoreComponentTransform_ : 1;
	/** Tile vector field in x axis? */
	UInt32 localVectorFieldTileX_ : 1;
	/** Tile vector field in y axis? */
	UInt32 localVectorFieldTileY_ : 1;
	/** Tile vector field in z axis? */
	UInt32 localVectorFieldTileZ_ : 1;
	/** Use fix delta time in the simulation? */
	UInt32 localVectorFieldUseFixDT_ : 1;
	
	/** Particle alignment overrides */
	UInt32 removeHMDRoll_ : 1;
	float minFacingCameraBlendDistance_;
	float maxFacingCameraBlendDistance_;
	
	/** Default constructor. */
	ParticleEmitterBuildInfo();
};

/*-----------------------------------------------------------------------------
	FParticleEmitterInstance
-----------------------------------------------------------------------------*/
struct FlagGG_API ParticleEmitterInstance
{

};

}
