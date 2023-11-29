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
	ParticleModuleRequired* RequiredModule;
	/** The spawn module. */
	ParticleModuleSpawn* SpawnModule;
	/** The spawn-per-unit module. */
	ParticleModuleSpawnPerUnit* SpawnPerUnitModule;
	/** List of spawn modules that need to be invoked at runtime. */
	PODVector<ParticleModule*> SpawnModules;

	/** The accumulated orbit offset. */
	ComposableVectorDistribution OrbitOffset;
	/** The accumulated orbit initial rotation. */
	ComposableVectorDistribution OrbitInitialRotation;
	/** The accumulated orbit rotation rate. */
	ComposableVectorDistribution OrbitRotationRate;

	/** The color scale of a particle over time. */
	ComposableVectorDistribution ColorScale;
	/** The alpha scale of a particle over time. */
	ComposableFloatDistribution AlphaScale;

	/** An additional color scale for allowing parameters to be used for ColorOverLife modules. */
	RawDistributionVector DynamicColor;
	/** An additional alpha scale for allowing parameters to be used for ColorOverLife modules. */
	RawDistributionFloat DynamicAlpha;

	/** An additional color scale for allowing parameters to be used for ColorScaleOverLife modules. */
	RawDistributionVector DynamicColorScale;
	/** An additional alpha scale for allowing parameters to be used for ColorScaleOverLife modules. */
	RawDistributionFloat DynamicAlphaScale;
	
	/** How to scale a particle's size over time. */
	ComposableVectorDistribution SizeScale;
	/** The maximum size of a particle. */
	Vector2 MaxSize;
	/** How much to scale a particle's size based on its speed. */
	Vector2 SizeScaleBySpeed;
	/** The maximum amount by which to scale a particle based on its speed. */
	Vector2 MaxSizeScaleBySpeed;

	/** The sub-image index over the particle's life time. */
	ComposableFloatDistribution SubImageIndex;

	/** Drag coefficient. */
	ComposableFloatDistribution DragCoefficient;
	/** Drag scale over life. */
	ComposableFloatDistribution DragScale;

	/** Enable collision? */
	bool bEnableCollision;
	/** How particles respond to collision. */
	UInt8 CollisionResponse;
	UInt8 CollisionMode;
	/** Radius scale applied to friction. */
	float CollisionRadiusScale;
	/** Bias applied to the collision radius. */
	float CollisionRadiusBias;
	/** Factor reflection spreading cone when colliding. */
	float CollisionRandomSpread;
	/** Random distribution across the reflection spreading cone when colliding. */
	float CollisionRandomDistribution;
	/** Friction. */
	float Friction;
	/** Collision damping factor. */
	ComposableFloatDistribution Resilience;
	/** Collision damping factor scale over life. */
	ComposableFloatDistribution ResilienceScaleOverLife;

	/** Location of a point source attractor. */
	Vector3 PointAttractorPosition;
	/** Radius of the point source attractor. */
	float PointAttractorRadius;
	/** Strength of the point attractor. */
	ComposableFloatDistribution PointAttractorStrength;

	/** The per-particle vector field scale. */
	ComposableFloatDistribution VectorFieldScale;
	/** The per-particle vector field scale-over-life. */
	ComposableFloatDistribution VectorFieldScaleOverLife;
	/** Global vector field scale. */
	float GlobalVectorFieldScale;
	/** Global vector field tightness. */
	float GlobalVectorFieldTightness;

	/** Local vector field. */
	class UVectorField* LocalVectorField;
	/** Local vector field transform. */
	FTransform LocalVectorFieldTransform;
	/** Local vector field intensity. */
	float LocalVectorFieldIntensity;
	/** Tightness tweak for local vector fields. */
	float LocalVectorFieldTightness;
	/** Minimum initial rotation applied to local vector fields. */
	Vector3 LocalVectorFieldMinInitialRotation;
	/** Maximum initial rotation applied to local vector fields. */
	Vector3 LocalVectorFieldMaxInitialRotation;
	/** Local vector field rotation rate. */
	Vector3 LocalVectorFieldRotationRate;

	/** Constant acceleration to apply to particles. */
	Vector3 ConstantAcceleration;

	/** The maximum lifetime of any particle that will spawn. */
	float MaxLifetime;
	/** The maximum rotation rate of particles. */
	float MaxRotationRate;
	/** The estimated maximum number of particles for this emitter. */
	Int32 EstimatedMaxActiveParticleCount;

	Int32 ScreenAlignment;

	/** An offset in UV space for the positioning of a sprites verticies. */
	Vector2 PivotOffset;

	/** If true, local vector fields ignore the component transform. */
	UInt32 bLocalVectorFieldIgnoreComponentTransform : 1;
	/** Tile vector field in x axis? */
	UInt32 bLocalVectorFieldTileX : 1;
	/** Tile vector field in y axis? */
	UInt32 bLocalVectorFieldTileY : 1;
	/** Tile vector field in z axis? */
	UInt32 bLocalVectorFieldTileZ : 1;
	/** Use fix delta time in the simulation? */
	UInt32 bLocalVectorFieldUseFixDT : 1;
	
	/** Particle alignment overrides */
	UInt32 bRemoveHMDRoll : 1;
	float MinFacingCameraBlendDistance;
	float MaxFacingCameraBlendDistance;
	
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
