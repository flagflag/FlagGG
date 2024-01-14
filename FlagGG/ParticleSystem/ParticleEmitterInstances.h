#pragma once

#include "Container/Vector.h"
#include "Math/Vector3.h"
#include "Math/Matrix3x4.h"
#include "Math/Distributions/Distributions.h"
#include "Math/Distributions/DistributionFloat.h"
#include "Math/Distributions/DistributionVector.h"
#include "ParticleSystem/ParticleHelper.h"
#include "ParticleSystem/Module/ParticleModuleOrientation.h"
#include "ParticleSystem/Module/ParticleModuleTrail.h"

namespace FlagGG
{

/*-----------------------------------------------------------------------------
	Forward declarations
-----------------------------------------------------------------------------*/
//	Emitter
class ParticleEmitter;
class ParticleSpriteEmitter;

// Module data
class ParticleModuleTypeDataMesh;
class ParticleModuleTypeDataBeam2;
class ParticleModuleTypeDataRibbon;
class ParticleModuleTypeDataAnimTrail;

// particle module
class ParticleModule;
class ParticleModuleRequired;
class ParticleModuleBeamSource;
class ParticleModuleBeamTarget;
class ParticleModuleBeamNoise;
class ParticleModuleTrailSource;
class ParticleModuleSpawn;
class ParticleModuleSpawnPerUnit;
class ParticleModuleOrientationAxisLock;
class ParticleLODLevel;

// Scene component
class StaticMeshComponent;
class ParticleSystemComponent;
class Light;

class Material;

struct ParticleEventInstancePayload;

class ParticleMeshDataBuilder;
class Geometry;
struct RenderPiplineContext;
struct RenderContext;

struct LODBurstFired
{
	PODVector<bool> fired_;
};

/*-----------------------------------------------------------------------------
	Information compiled from modules to build runtime emitter data.
-----------------------------------------------------------------------------*/

struct FlagGG_API ParticleEmitterBuildInfo
{
	/** Default constructor. */
	ParticleEmitterBuildInfo();

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
	// VectorField* localVectorField_;
	/** Local vector field transform. */
	Matrix3x4 localVectorFieldTransform_;
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
	
	/** particle alignment overrides */
	UInt32 removeHMDRoll_ : 1;
	float minFacingCameraBlendDistance_;
	float maxFacingCameraBlendDistance_;
};

/*-----------------------------------------------------------------------------
	ParticleEmitterInstance
-----------------------------------------------------------------------------*/
struct FlagGG_API ParticleEmitterInstance
{
public:
	/** Constructor	*/
	ParticleEmitterInstance();

	/** Destructor	*/
	virtual ~ParticleEmitterInstance();

	//
	virtual void InitParameters(ParticleEmitter* inTemplate, ParticleSystemComponent* inComponent);
	virtual void Init();

	/** @return The world that the component that owns this instance is in */
	// World* GetWorld() const;

	/**
	 * Ensures enough memory is allocated for the requested number of particles.
	 *
	 * @param newMaxActiveParticles		The number of particles for which memory must be allocated.
	 * @param setMaxActiveCount		If true, update the peak active particles for this LOD.
	 * @returns bool					true if memory is allocated for at least newMaxActiveParticles.
	 */
	virtual bool Resize(Int32 newMaxActiveParticles, bool setMaxActiveCount = true);

	virtual void Tick(float deltaTime, bool suppressSpawning);
	void CheckEmitterFinished();

	/** Advances the bursts as though they were fired with out actually firing them. */
	void FakeBursts();

	/**
	 *	Tick sub-function that handles EmitterTime setup, looping, etc.
	 *
	 *	@param	deltaTime			The current time slice
	 *	@param	currentLODLevel		The current LOD level for the instance
	 *
	 *	@return	float				The EmitterDelay
	 */
	virtual float Tick_EmitterTimeSetup(float deltaTime, ParticleLODLevel* currentLODLevel);
	/**
	 *	Tick sub-function that handles spawning of particles
	 *
	 *	@param	deltaTime			The current time slice
	 *	@param	currentLODLevel		The current LOD level for the instance
	 *	@param	suppressSpawning	true if spawning has been suppressed on the owning particle system component
	 *	@param	bFirstTime			true if this is the first time the instance has been ticked
	 *
	 *	@return	float				The SpawnFraction remaining
	 */
	virtual float Tick_SpawnParticles(float deltaTime, ParticleLODLevel* currentLODLevel, bool suppressSpawning, bool firstTime);
	/**
	 *	Tick sub-function that handles module updates
	 *
	 *	@param	deltaTime			The current time slice
	 *	@param	currentLODLevel		The current LOD level for the instance
	 */
	virtual void Tick_ModuleUpdate(float deltaTime, ParticleLODLevel* currentLODLevel);
	/**
	 *	Tick sub-function that handles module post updates
	 *
	 *	@param	deltaTime			The current time slice
	 *	@param	currentLODLevel		The current LOD level for the instance
	 */
	virtual void Tick_ModulePostUpdate(float deltaTime, ParticleLODLevel* currentLODLevel);
	/**
	 *	Tick sub-function that handles module FINAL updates
	 *
	 *	@param	deltaTime			The current time slice
	 *	@param	currentLODLevel		The current LOD level for the instance
	 */
	virtual void Tick_ModuleFinalUpdate(float deltaTime, ParticleLODLevel* currentLODLevel);

	/**
	 *	Set the LOD to the given index
	 *
	 *	@param	InLODIndex			The index of the LOD to set as current
	 *	@param	bInFullyProcess		If true, process burst lists, etc.
	 */
	virtual void SetCurrentLODIndex(Int32 inLODIndex, bool inFullyProcess);

	virtual void Rewind();
	virtual BoundingBox GetBoundingBox();
	virtual void UpdateBoundingBox(float deltaTime);
	virtual void ForceUpdateBoundingBox();
	virtual UInt32 RequiredBytes();
	/** Get offset for particle payload data for a particular module */
	UInt32 GetModuleDataOffset(ParticleModule* module);
	/** Get pointer to emitter instance payload data for a particular module */
	UInt8* GetModuleInstanceData(ParticleModule* module);
	/** Get pointer to emitter instance random seed payload data for a particular module */
	ParticleRandomSeedInstancePayload* GetModuleRandomSeedInstanceData(ParticleModule* module);
	virtual UInt8* GetTypeDataModuleInstanceData();
	virtual UInt32 CalculateParticleStride(UInt32 particleSize);
	virtual void ResetBurstList();
	virtual float GetCurrentBurstRateOffset(float& deltaTime, Int32& burst);
	virtual void ResetParticleParameters(float deltaTime);
	void CalculateOrbitOffset(OrbitChainModuleInstancePayload& payload,
		Vector3& accumOffset, Vector3& accumRotation, Vector3& accumRotationRate,
		float deltaTime, Vector3& result, Matrix3x4& rotationMat);
	virtual void UpdateOrbitData(float deltaTime);
	virtual void ParticlePrefetch();

	/**
	 *	Spawn particles for this emitter instance
	 *	@param	deltaTime		The time slice to spawn over
	 *	@return	float			The leftover fraction of spawning
	 */
	virtual float Spawn(float deltaTime);

	/**
	 * Spawn the indicated number of particles.
	 *
	 * @param Count The number of particles to spawn.
	 * @param StartTime			The local emitter time at which to begin spawning particles.
	 * @param Increment			The time delta between spawned particles.
	 * @param InitialLocation	The initial location of spawned particles.
	 * @param InitialVelocity	The initial velocity of spawned particles.
	 * @param EventPayload		Event generator payload if events should be triggered.
	 */
	void SpawnParticles(Int32 count, float startTime, float increment, const Vector3& initialLocation, const Vector3& initialVelocity, ParticleEventInstancePayload* eventPayload);

	/**
	 *	Spawn/burst the given particles...
	 *
	 *	@param	deltaTime		The time slice to spawn over.
	 *	@param	InSpawnCount	The number of particles to forcibly spawn.
	 *	@param	InBurstCount	The number of particles to forcibly burst.
	 *	@param	InLocation		The location to spawn at.
	 *	@param	InVelocity		OPTIONAL velocity to have the particle inherit.
	 *
	 */
	virtual void ForceSpawn(float deltaTime, Int32 inSpawnCount, Int32 inBurstCount, Vector3& inLocation, Vector3& inVelocity);
	void CheckSpawnCount(Int32 inNewCount, Int32 inMaxCount);

	/**
	 * Handle any pre-spawning actions required for particles
	 *
	 * @param particle			The particle being spawned.
	 * @param InitialLocation	The initial location of the particle.
	 * @param InitialVelocity	The initial velocity of the particle.
	 */
	virtual void PreSpawn(BaseParticle* particle, const Vector3& initialLocation, const Vector3& initialVelocity);

	/**
	 * Handle any post-spawning actions required by the instance
	 *
	 * @param	particle					The particle that was spawned
	 * @param	interpolationPercentage		The percentage of the time slice it was spawned at
	 * @param	SpawnTIme					The time it was spawned at
	 */
	virtual void PostSpawn(BaseParticle* particle, float interpolationPercentage, float spawnTime);

	virtual bool HasCompleted();
	virtual void KillParticles();
	/**
	 *	Kill the particle at the given instance
	 *
	 *	@param	Index		The index of the particle to kill.
	 */
	virtual void KillParticle(Int32 index);

	/**
	 *	Force kill all particles in the emitter.
	 *
	 *	@param	bFireEvents		If true, fire events for the particles being killed.
	 */
	virtual void KillParticlesForced(bool fireEvents = false);

	/** Set the HaltSpawning flag */
	virtual void SetHaltSpawning(bool inHaltSpawning)
	{
		haltSpawning_ = inHaltSpawning;
	}

	/** Set the bHaltSpawningExternal flag */
	virtual void SetHaltSpawningExternal(bool inHaltSpawning)
	{
		haltSpawningExternal_ = inHaltSpawning;
	}

	FORCEINLINE void SetFakeBurstWhenSpawningSupressed(bool inFakeBurstsWhenSpawningSupressed)
	{
		fakeBurstsWhenSpawningSupressed_ = inFakeBurstsWhenSpawningSupressed;
	}

	/** Get the offset of the orbit payload. */
	Int32 GetOrbitPayloadOffset();

	/** Get the position of the particle taking orbit in to account. */
	Vector3 GetParticleLocationWithOrbitOffset(BaseParticle* particle);

	virtual BaseParticle* GetParticle(Int32 index);
	/**
	 *	Get the physical index of the particle at the given index
	 *	(ie, the contents of ParticleIndices[InIndex])
	 *
	 *	@param	InIndex			The index of the particle of interest
	 *
	 *	@return	Int32				The direct index of the particle
	 */
	FORCEINLINE Int32 GetParticleDirectIndex(Int32 inIndex)
	{
		if (inIndex < maxActiveParticles_)
		{
			return particleIndices_[inIndex];
		}
		return -1;
	}
	/**
	 *	Get the particle at the given direct index
	 *
	 *	@param	InDirectIndex		The direct index of the particle of interest
	 *
	 *	@return	BaseParticle*		The particle, if valid index was given; NULL otherwise
	 */
	virtual BaseParticle* GetParticleDirect(Int32 inDirectIndex);

	/**
	 *	Calculates the emitter duration for the instance.
	 */
	void SetupEmitterDuration();

	/**
	 * Returns whether the system has any active particles.
	 *
	 * @return true if there are active particles, false otherwise.
	 */
	bool HasActiveParticles()
	{
		return activeParticles_ > 0;
	}

	/**
	 *	Checks some common values for GetDynamicData validity
	 *
	 *	@return	bool		true if GetDynamicData should continue, false if it should return NULL
	 */
	virtual bool IsDynamicDataRequired(ParticleLODLevel* currentLODLevel);

	/**
	 *	Retrieves the dynamic data for the emitter
	 */
	//virtual DynamicEmitterDataBase* GetDynamicData(bool bSelected, ERHIFeatureLevel::Type InFeatureLevel)
	//{
	//	return NULL;
	//}

	/**
	 *	Retrieves replay data for the emitter
	 *
	 *	@return	The replay data, or NULL on failure
	 */
	//virtual DynamicEmitterReplayDataBase* GetReplayData()
	//{
	//	return NULL;
	//}

	/**
	 *	Retrieve the allocated size of this instance.
	 *
	 *	@param	OutNum			The size of this instance
	 *	@param	OutMax			The maximum size of this instance
	 */
	virtual void GetAllocatedSize(Int32& outNum, Int32& outMax)
	{
		outNum = 0;
		outMax = 0;
	}

	/** Returns resource size, similar to UObject function */
	//virtual void GetResourceSizeEx(ResourceSizeEx& CumulativeResourceSize)
	//{
	//}

	/**
	 *	Process received events.
	 */
	virtual void ProcessParticleEvents(float deltaTime, bool suppressSpawning);

	/**
	 *	Called when the particle system is deactivating...
	 */
	virtual void OnDeactivateSystem()
	{
	}

	/**
	 * Returns the offset to the mesh rotation payload, if any.
	 */
	virtual Int32 GetMeshRotationOffset() const
	{
		return 0;
	}

	/**
	 * Returns true if mesh rotation is active.
	 */
	virtual bool IsMeshRotationActive() const
	{
		return false;
	}

	/**
	 * Sets the materials with which mesh particles should be rendered.
	 * @param inMaterials - The materials.
	 */
	virtual void SetMeshMaterials(const PODVector<Material*>& inMaterials)
	{
	}

	/**
	 * Gathers material relevance flags for this emitter instance.
	 * @param OutMaterialRelevance - Pointer to where material relevance flags will be stored.
	 * @param LODLevel - The LOD level for which to compute material relevance flags.
	 */
	// virtual void GatherMaterialRelevance(Material* OutMaterialRelevance, const ParticleLODLevel* LODLevel, ERHIFeatureLevel::Type InFeatureLevel) const;

	/**
	 * When an emitter is killed, this will check other emitters and clean up anything pointing to this one
	 */
	virtual void OnEmitterInstanceKilled(ParticleEmitterInstance* instance)
	{

	}

	// Beam interface
	virtual void SetBeamEndPoint(Vector3 newEndPoint) {};
	virtual void SetBeamSourcePoint(Vector3 newSourcePoint, Int32 sourceIndex) {};
	virtual void SetBeamSourceTangent(Vector3 newTangentPoint, Int32 sourceIndex) {};
	virtual void SetBeamSourceStrength(float newSourceStrength, Int32 sourceIndex) {};
	virtual void SetBeamTargetPoint(Vector3 NewTargetPoint, Int32 targetIndex) {};
	virtual void SetBeamTargetTangent(Vector3 newTangentPoint, Int32 targetIndex) {};
	virtual void SetBeamTargetStrength(float newTargetStrength, Int32 targetIndex) {};

	//Beam get interface
	virtual bool GetBeamEndPoint(Vector3& outEndPoint) const { return false; }
	virtual bool GetBeamSourcePoint(Int32 sourceIndex, Vector3& outSourcePoint) const { return false; }
	virtual bool GetBeamSourceTangent(Int32 sourceIndex, Vector3& outTangentPoint) const { return false; }
	virtual bool GetBeamSourceStrength(Int32 sourceIndex, float& outSourceStrength) const { return false; }
	virtual bool GetBeamTargetPoint(Int32 targetIndex, Vector3& outTargetPoint) const { return false; }
	virtual bool GetBeamTargetTangent(Int32 targetIndex, Vector3& outTangentPoint) const { return false; }
	virtual bool GetBeamTargetStrength(Int32 targetIndex, float& outTargetStrength) const { return false; }

	// Called on world origin changes
	virtual void ApplyWorldOffset(Vector3 inOffset, bool worldShift);

	virtual bool IsTrailEmitter()const { return false; }

	/**
	* Begins the trail.
	*/
	virtual void BeginTrail() {}

	/**
	* Ends the trail.
	*/
	virtual void EndTrail() {}

	/**
	* Sets the data that defines this trail.
	*
	* @param	InFirstSocketName	The name of the first socket for the trail.
	* @param	InSecondSocketName	The name of the second socket for the trail.
	* @param	InWidthMode			How the width value is applied to the trail.
	* @param	InWidth				The width of the trail.
	*/
	virtual void SetTrailSourceData(const String& inFirstSocketName, const String& inSecondSocketName, TrailWidthMode inWidthMode, float inWidth) {}

	/**
	Ticks the emitter's material overrides.
	@return True if there were material overrides. Otherwise revert to default behaviour.
	*/
	virtual void Tick_MaterialOverrides(Int32 emitterIndex);

	/**
	* True if this emitter emits in local space
	*/
	bool UseLocalSpace();

	/**
	* returns the screen alignment and scale of the component.
	*/
	void GetScreenAlignmentAndScale(Int32& outScreenAlign, Vector3& outScale);

	// 渲染更新
	virtual void RenderUpdate(const RenderPiplineContext* renderPiplineContext, ParticleMeshDataBuilder* particleMeshDataBuilder) {}


	/** The maximum deltaTime allowed for updating PeakActiveParticle tracking.
	 *	Any delta time > this value will not impact active particle tracking.
	 */
	static const float peakActiveParticleUpdateDelta_;

	/** The template this instance is based on.							*/
	ParticleEmitter* spriteTemplate_;
	/** The component who owns it.										*/
	ParticleSystemComponent* component_;
	/** The index of the currently set LOD level.						*/
	Int32 currentLODLevelIndex_;
	/** The currently set LOD level.									*/
	ParticleLODLevel* currentLODLevel_;
	/** The offset to the TypeData payload in the particle data.		*/
	Int32 typeDataOffset_;
	/** The offset to the TypeData instance payload.					*/
	Int32 typeDataInstanceOffset_;
	/** The offset to the SubUV payload in the particle data.			*/
	Int32 subUVDataOffset_;
	/** The offset to the dynamic parameter payload in the particle data*/
	Int32 dynamicParameterDataOffset_;
	/** Offset to the light module data payload.						*/
	Int32 lightDataOffset_;
	float lightVolumetricScatteringIntensity_;
	/** The offset to the Orbit module payload in the particle data.	*/
	Int32 orbitModuleOffset_;
	/** The offset to the Camera payload in the particle data.			*/
	Int32 cameraPayloadOffset_;
	/** The location of the emitter instance							*/
	Vector3 location_;
	/** Transform from emitter local space to simulation space.			*/
	Matrix3x4 emitterToSimulation_;
	/** Transform from simulation space to world space.					*/
	Matrix3x4 simulationToWorld_;
	/** Component can disable Tick and Rendering of this emitter. */
	UInt32 enabled_ : 1;
	/** If true, kill this emitter instance when it is deactivated.		*/
	UInt32 killOnDeactivate_ : 1;
	/** if true, kill this emitter instance when it has completed.		*/
	UInt32 killOnCompleted_ : 1;
	/** Whether this emitter requires sorting as specified by artist.	*/
	UInt32 requiresSorting_ : 1;
	/** If true, halt spawning for this instance.						*/
	UInt32 haltSpawning_ : 1;
	/** If true, this emitter has been disabled by game code and some systems to re-enable are not allowed. */
	UInt32 haltSpawningExternal_ : 1;
	/** If true, the emitter has modules that require loop notification.*/
	UInt32 requiresLoopNotification_ : 1;
	/** If true, the emitter ignores the component's scale. (Mesh emitters only). */
	UInt32 ignoreComponentScale_ : 1;
	/** Hack: Make sure this is a Beam type to avoid casting from/to wrong types. */
	UInt32 isBeam_ : 1;
	/** Whether axis lock is enabled, cached here to avoid finding it from the module each frame */
	UInt32 axisLockEnabled_ : 1;
	/** When true and spawning is supressed, the bursts will be faked so that when spawning is enabled again, the bursts don't fire late. */
	UInt32 fakeBurstsWhenSpawningSupressed_ : 1;
	/** Axis lock flags, cached here to avoid finding it from the module each frame */
	EnumAsByte<ParticleAxisLock> lockAxisFlags_;
	/** The sort mode to use for this emitter as specified by artist.	*/
	Int32 sortMode_;
	/** Pointer to the particle data array.								*/
	UInt8* particleData_;
	/** Pointer to the particle index array.							*/
	UInt16* particleIndices_;
	/** Pointer to the instance data array.								*/
	UInt8* instanceData_;
	/** The size of the Instance data array.							*/
	Int32 instancePayloadSize_;
	/** The offset to the particle data.								*/
	Int32 payloadOffset_;
	/** The total size of a particle (in bytes).						*/
	Int32 particleSize_;
	/** The stride between particles in the ParticleData array.			*/
	Int32 particleStride_;
	/** The number of particles currently active in the emitter.		*/
	Int32 activeParticles_;
	/** Monotonically increasing counter. */
	UInt32 particleCounter_;
	/** The maximum number of active particles that can be held in
	 *	the particle data array.
	 */
	Int32 maxActiveParticles_;
	/** The fraction of time left over from spawning.					*/
	float spawnFraction_;
	/** The number of seconds that have passed since the instance was
	 *	created.
	 */
	float secondsSinceCreation_;
	/** */
	float emitterTime_;
	/** The amount of time simulated in the previous time step. */
	float lastDeltaTime_;
	/** how long did the last tick take? */
#if WITH_EDITOR
	float lastTickDurationMs_;
#endif
	/** The previous location of the instance.							*/
	Vector3 oldLocation_;
	/** The bounding box for the particles.								*/
	BoundingBox particleBoundingBox_;
	/** The BurstFire information.										*/
	Vector<LODBurstFired> burstFired_;
	/** The number of loops completed by the instance.					*/
	Int32 loopCount_;
	/** Flag indicating if the render data is dirty.					*/
	Int32 isRenderDataDirty_;
	/** The current duration fo the emitter instance.					*/
	float emitterDuration_;
	/** The emitter duration at each LOD level for the instance.		*/
	PODVector<float> emitterDurations_;
	/** The emitter's delay for the current loop		*/
	float currentDelay_;
	/** true if the emitter has no active particles and will no longer spawn any in the future */
	bool emitterIsDone_;

	/** The number of triangles to render								*/
	Int32 trianglesToRender_;
	Int32 maxVertexIndex_;

	/** The material to render this instance with.						*/
	Material* currentMaterial_;
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	/** Number of events this emitter has generated... */
	Int32 eventCount_;
	Int32 maxEventCount_;
#endif	//#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

	/** Position offset for each particle. Will be reset to zero at the end of the tick	*/
	Vector3 positionOffsetThisTick_;

	/** The PivotOffset applied to the vertex positions 			*/
	Vector2 pivotOffset_;

	PODVector<Light*> highQualityLights_;

	// 粒子网格图形数据
	SharedPtr<Geometry> geometry_;

	RenderContext* renderContext_;

protected:

	/**
	 * Captures dynamic replay data for this particle system.
	 *
	 * @param	OutData		[Out] Data will be copied here
	 *
	 * @return Returns true if successful
	 */
	// virtual bool FillReplayData(DynamicEmitterReplayDataBase& OutData);

	/**
	 * Updates all internal transforms.
	 */
	void UpdateTransforms();

	/**
	* Retrieves the current LOD level and asserts that it is valid.
	*/
	class ParticleLODLevel* GetCurrentLODLevelChecked();

	/**
	 * Get the current material to render with.
	 */
	Material* GetCurrentMaterial();


	/**
	 * Fixup particle indices to only have valid entries.
	 */
	void FixupParticleIndices();
};

/*-----------------------------------------------------------------------------
	ParticleSpriteEmitterInstance
-----------------------------------------------------------------------------*/
struct FlagGG_API ParticleSpriteEmitterInstance : public ParticleEmitterInstance
{
	/** Constructor	*/
	ParticleSpriteEmitterInstance();

	/** Destructor	*/
	virtual ~ParticleSpriteEmitterInstance();

	/**
	 *	Retrieves the dynamic data for the emitter
	 */
	// virtual DynamicEmitterDataBase* GetDynamicData(bool bSelected, ERHIFeatureLevel::Type InFeatureLevel) override;

	/**
	 *	Retrieves replay data for the emitter
	 *
	 *	@return	The replay data, or NULL on failure
	 */
	// virtual DynamicEmitterReplayDataBase* GetReplayData() override;

	/**
	 *	Retrieve the allocated size of this instance.
	 *
	 *	@param	OutNum			The size of this instance
	 *	@param	OutMax			The maximum size of this instance
	 */
	virtual void GetAllocatedSize(Int32& outNum, Int32& outMax) override;

	/**
	 * Returns the size of the object/ resource for display to artists/ LDs in the Editor.
	 *
	 * @param	Mode	Specifies which resource size should be displayed. ( see EResourceSizeMode::Type )
	 * @return  Size of resource as to be displayed to artists/ LDs in the Editor.
	 */
	// virtual void GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize) override;

protected:

	/**
	 * Captures dynamic replay data for this particle system.
	 *
	 * @param	OutData		[Out] Data will be copied here
	 *
	 * @return Returns true if successful
	 */
	// virtual bool FillReplayData(DynamicEmitterReplayDataBase& OutData) override;

	// 渲染更新
	void RenderUpdate(const RenderPiplineContext* renderPiplineContext, ParticleMeshDataBuilder* particleMeshDataBuilder) override;
};

/*-----------------------------------------------------------------------------
	ParticleMeshEmitterInstance
-----------------------------------------------------------------------------*/
struct FlagGG_API ParticleMeshEmitterInstance : public ParticleEmitterInstance
{
	/** Constructor	*/
	ParticleMeshEmitterInstance();

	virtual void InitParameters(ParticleEmitter* inTemplate, ParticleSystemComponent* inComponent) override;
	virtual void Init() override;
	virtual bool Resize(Int32 newMaxActiveParticles, bool setMaxActiveCount = true) override;
	virtual void Tick(float deltaTime, bool suppressSpawning) override;
	virtual void UpdateBoundingBox(float deltaTime) override;
	virtual UInt32 RequiredBytes() override;
	virtual void PostSpawn(BaseParticle* particle, float interpolationPercentage, float spawnTime) override;
	// virtual DynamicEmitterDataBase* GetDynamicData(bool bSelected, ERHIFeatureLevel::Type InFeatureLevel) override;
	virtual bool IsDynamicDataRequired(ParticleLODLevel* currentLODLevel) override;

	virtual void Tick_MaterialOverrides(Int32 emitterIndex) override;

	/**
	 *	Retrieves replay data for the emitter
	 *
	 *	@return	The replay data, or NULL on failure
	 */
	// virtual DynamicEmitterReplayDataBase* GetReplayData() override;

	/**
	 *	Retrieve the allocated size of this instance.
	 *
	 *	@param	OutNum			The size of this instance
	 *	@param	OutMax			The maximum size of this instance
	 */
	virtual void GetAllocatedSize(Int32& outNum, Int32& outMax) override;

	/**
	 * Returns the size of the object/ resource for display to artists/ LDs in the Editor.
	 *
	 * @param	Mode	Specifies which resource size should be displayed. ( see EResourceSizeMode::Type )
	 * @return  Size of resource as to be displayed to artists/ LDs in the Editor.
	 */
	// virtual void GetResourceSizeEx(ResourceSizeEx& CumulativeResourceSize) override;

	/**
	 * Returns the offset to the mesh rotation payload, if any.
	 */
	virtual Int32 GetMeshRotationOffset() const override
	{
		return meshRotationOffset_;
	}

	/**
	 * Returns true if mesh rotation is active.
	 */
	virtual bool IsMeshRotationActive() const override
	{
		return meshRotationActive_;
	}

	/**
	 * Sets the materials with which mesh particles should be rendered.
	 * @param inMaterials - The materials.
	 */
	virtual void SetMeshMaterials(const PODVector<Material*>& inMaterials) override;

	/**
	 * Gathers material relevance flags for this emitter instance.
	 * @param OutMaterialRelevance - Pointer to where material relevance flags will be stored.
	 * @param LODLevel - The LOD level for which to compute material relevance flags.
	 */
	// virtual void GatherMaterialRelevance(Material* OutMaterialRelevance, const ParticleLODLevel* LODLevel, RHIFeatureLevel::Type InFeatureLevel) const override;

	/**
	 * Gets the materials applied to each section of a mesh.
	 */
	// void GetMeshMaterials(PODVector<Material*, TInlineAllocator<2> >& OutMaterials, const UParticleLODLevel* LODLevel, RHIFeatureLevel::Type InFeatureLevel, bool bLogWarnings = false) const;


	ParticleModuleTypeDataMesh* meshTypeData_;
	bool meshRotationActive_;
	Int32 meshRotationOffset_;
	Int32 meshMotionBlurOffset_;

	/** The materials to render this instance with.	*/
	PODVector<Material*> currentMaterials_;

protected:

	/**
	 * Captures dynamic replay data for this particle system.
	 *
	 * @param	OutData		[Out] Data will be copied here
	 *
	 * @return Returns true if successful
	 */
	// virtual bool FillReplayData(FDynamicEmitterReplayDataBase& OutData) override;
};

}
