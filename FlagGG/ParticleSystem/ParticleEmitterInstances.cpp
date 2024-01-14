#include "ParticleEmitterInstances.h"
#include "ParticleSystem/Module/ParticleModuleRequired.h"
#include "ParticleSystem/Module/ParticleModuleCollision.h"
#include "ParticleSystem/Module/ParticleModuleTypeData.h"
#include "ParticleSystem/Module/ParticleModuleLocation.h"
#include "ParticleSystem/Emitter/ParticleSpriteEmitter.h"
#include "ParticleSystem/ParticleLODLevel.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/ParticleSystemRenderer.h"
#include "Scene/Node.h"
#include "Scene/Camera.h"
#include "Graphics/RenderPipline.h"

namespace FlagGG
{

/*-----------------------------------------------------------------------------
	Information compiled from modules to build runtime emitter data.
-----------------------------------------------------------------------------*/

ParticleEmitterBuildInfo::ParticleEmitterBuildInfo()
	: requiredModule_(NULL)
	, spawnModule_(NULL)
	, spawnPerUnitModule_(NULL)
	, maxSize_(1.0f, 1.0f)
	, sizeScaleBySpeed_(Vector2::ZERO)
	, maxSizeScaleBySpeed_(1.0f, 1.0f)
	, enableCollision_(false)
	, collisionResponse_(ParticleCollisionResponse::Bounce)
	, collisionMode_(ParticleCollisionMode::SceneDepth)
	, collisionRadiusScale_(1.0f)
	, collisionRadiusBias_(0.0f)
	, collisionRandomSpread_(0.0f)
	, collisionRandomDistribution_(1.0f)
	, friction_(0.0f)
	, pointAttractorPosition_(Vector3::ZERO)
	, pointAttractorRadius_(0.0f)
	, globalVectorFieldScale_(0.0f)
	, globalVectorFieldTightness_(-1)
	// , localVectorField_(NULL)
	, localVectorFieldTransform_(Matrix3x4::IDENTITY)
	, localVectorFieldIntensity_(0.0f)
	, localVectorFieldTightness_(0.0f)
	, localVectorFieldMinInitialRotation_(Vector3::ZERO)
	, localVectorFieldMaxInitialRotation_(Vector3::ZERO)
	, localVectorFieldRotationRate_(Vector3::ZERO)
	, constantAcceleration_(Vector3::ZERO)
	, maxLifetime_(1.0f)
	, maxRotationRate_(1.0f)
	, estimatedMaxActiveParticleCount_(0)
	, screenAlignment_(PSA_Square)
	, pivotOffset_(-0.5,-0.5)
	, localVectorFieldIgnoreComponentTransform_(false)
	, localVectorFieldTileX_(false)
	, localVectorFieldTileY_(false)
	, localVectorFieldTileZ_(false)
	, localVectorFieldUseFixDT_(false)
	, removeHMDRoll_(0)
	, minFacingCameraBlendDistance_(0.0f)
	, maxFacingCameraBlendDistance_(0.0f)
{
	dragScale_.InitializeWithConstant(1.0f);
	vectorFieldScale_.InitializeWithConstant(1.0f);
	vectorFieldScaleOverLife_.InitializeWithConstant(1.0f);
#if WITH_EDITOR
	dynamicColorScale_.Initialize();
	dynamicAlphaScale_.Initialize();
#endif
}

/*-----------------------------------------------------------------------------
	ParticleEmitterInstance
-----------------------------------------------------------------------------*/
// Only update the PeakActiveParticles if the frame rate is 20 or better
const float ParticleEmitterInstance::peakActiveParticleUpdateDelta_ = 0.05f;

/** Constructor	*/
ParticleEmitterInstance::ParticleEmitterInstance()
	: spriteTemplate_(NULL)
	, component_(NULL)
	, currentLODLevelIndex_(0)
	, currentLODLevel_(NULL)
	, typeDataOffset_(0)
	, typeDataInstanceOffset_(-1)
	, subUVDataOffset_(0)
	, dynamicParameterDataOffset_(0)
	, lightDataOffset_(0)
	, lightVolumetricScatteringIntensity_(0)
	, orbitModuleOffset_(0)
	, cameraPayloadOffset_(0)
	, enabled_(1)
	, killOnDeactivate_(0)
	, killOnCompleted_(0)
	, haltSpawning_(0)
	, haltSpawningExternal_(0)
	, requiresLoopNotification_(0)
	, ignoreComponentScale_(0)
	, isBeam_(0)
	, axisLockEnabled_(0)
	, fakeBurstsWhenSpawningSupressed_(0)
	, lockAxisFlags_(EPAL_NONE)
	, sortMode_(PSORTMODE_None)
	, particleData_(NULL)
	, particleIndices_(NULL)
	, instanceData_(NULL)
	, instancePayloadSize_(0)
	, payloadOffset_(0)
	, particleSize_(0)
	, particleStride_(0)
	, activeParticles_(0)
	, particleCounter_(0)
	, maxActiveParticles_(0)
	, spawnFraction_(0.0f)
	, secondsSinceCreation_(0.0f)
	, emitterTime_(0.0f)
	, loopCount_(0)
	, isRenderDataDirty_(0)
	, emitterDuration_(0.0f)
	, trianglesToRender_(0)
	, maxVertexIndex_(0)
	, currentMaterial_(NULL)
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	, eventCount_(0)
	, maxEventCount_(0)
#endif	//#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	, positionOffsetThisTick_(Vector3::ZERO)
	, pivotOffset_(-0.5f, -0.5f)
	, renderContext_(nullptr)
{
}

/** Destructor	*/
ParticleEmitterInstance::~ParticleEmitterInstance()
{
	for (Int32 i = 0; i < highQualityLights_.Size(); ++i)
	{
		Light* pointLightComponent = highQualityLights_[i];
		{
			//pointLightComponent->Modify();
			//pointLightComponent->DestroyComponent(false);
		}
	}
	highQualityLights_.Clear();

	free(particleData_);
	free(particleIndices_);
	free(instanceData_);
	burstFired_.Clear();
}

void ParticleEmitterInstance::InitParameters(ParticleEmitter* inTemplate, ParticleSystemComponent* inComponent)
{
	spriteTemplate_ = inTemplate;
	component_ = inComponent;
	SetupEmitterDuration();
}

/**
 *	Initialize the instance
 */
void ParticleEmitterInstance::Init()
{
	ASSERT(spriteTemplate_ != nullptr);

	// Use highest LOD level for init'ing data, will contain all module types.
	ParticleLODLevel* highLODLevel = spriteTemplate_->LODLevels_[0];

	// Set the current material
	ASSERT(highLODLevel->requiredModule_);
	currentMaterial_ = highLODLevel->requiredModule_->material_;

	// If we already have a non-zero particleSize_, don't need to do most allocation work again
	bool bNeedsInit = (particleSize_ == 0);

	if (bNeedsInit)
	{
		// Copy pre-calculated info
		requiresLoopNotification_ = spriteTemplate_->requiresLoopNotification_;
		axisLockEnabled_ = spriteTemplate_->axisLockEnabled_;
		lockAxisFlags_ = spriteTemplate_->lockAxisFlags_;
		dynamicParameterDataOffset_ = spriteTemplate_->dynamicParameterDataOffset_;
		lightDataOffset_ = spriteTemplate_->lightDataOffset_;
		lightVolumetricScatteringIntensity_ = spriteTemplate_->lightVolumetricScatteringIntensity_;
		cameraPayloadOffset_ = spriteTemplate_->cameraPayloadOffset_;
		particleSize_ = spriteTemplate_->particleSize_;
		pivotOffset_ = spriteTemplate_->pivotOffset_;
		typeDataOffset_ = spriteTemplate_->typeDataOffset_;
		typeDataInstanceOffset_ = spriteTemplate_->typeDataInstanceOffset_;

		if ((instanceData_ == NULL) || (spriteTemplate_->reqInstanceBytes_ > instancePayloadSize_))
		{
			instanceData_ = (UInt8*)(realloc(instanceData_, spriteTemplate_->reqInstanceBytes_));
			instancePayloadSize_ = spriteTemplate_->reqInstanceBytes_;
		}

		memset(instanceData_, 0, instancePayloadSize_);

		for (ParticleModule* particleModule : spriteTemplate_->modulesNeedingInstanceData_)
		{
			ASSERT(particleModule);
			UInt8* prepInstData = GetModuleInstanceData(particleModule);
			ASSERT(prepInstData != nullptr); // Shouldn't be in the list if it doesn't have data
			particleModule->PrepPerInstanceBlock(this, (void*)prepInstData);
		}

		for (ParticleModule* particleModule : spriteTemplate_->modulesNeedingRandomSeedInstanceData_)
		{
			ASSERT(particleModule);
			ParticleRandomSeedInstancePayload* seedInstancePayload = GetModuleRandomSeedInstanceData(particleModule);
			ASSERT(seedInstancePayload != nullptr); // Shouldn't be in the list if it doesn't have data
			ParticleRandomSeedInfo* randomSeedInfo = particleModule->GetRandomSeedInfo();
			particleModule->PrepRandomSeedInstancePayload(this, seedInstancePayload, randomSeedInfo ? *randomSeedInfo : ParticleRandomSeedInfo());
		}

		// Offset into emitter specific payload (e.g. TrailComponent requires extra bytes).
		payloadOffset_ = particleSize_;

		// Update size with emitter specific size requirements.
		particleSize_ += RequiredBytes();

		// Make sure everything is at least 16 byte aligned so we can use SSE for Vector3.
		// particleSize_ = Align(particleSize_, 16);
		particleSize_ = (Int32)(((UInt64)particleSize_ + 16 - 1) & ~(16 - 1));

		// E.g. trail emitters store trailing particles directly after leading one.
		particleStride_ = CalculateParticleStride(particleSize_);
	}

	// Setup the emitter instance material array...
	SetMeshMaterials(spriteTemplate_->meshMaterials_);

	// Set initial values.
	spawnFraction_ = 0;
	secondsSinceCreation_ = 0;
	emitterTime_ = 0;
	particleCounter_ = 0;

	UpdateTransforms();
	location_ = component_->GetNode()->GetPosition();
	oldLocation_ = location_;

	trianglesToRender_ = 0;
	maxVertexIndex_ = 0;

	if (particleData_ == NULL)
	{
		maxActiveParticles_ = 0;
		activeParticles_ = 0;
	}

	particleBoundingBox_.Clear();
	if (highLODLevel->requiredModule_->randomImageChanges_ == 0)
	{
		highLODLevel->requiredModule_->randomImageTime_ = 1.0f;
	}
	else
	{
		highLODLevel->requiredModule_->randomImageTime_ = 0.99f / (highLODLevel->requiredModule_->randomImageChanges_ + 1);
	}

	// Resize to sensible default.
	if (bNeedsInit &&
		// Only presize if any particles will be spawned 
		spriteTemplate_->qualityLevelSpawnRateScale_ > 0)
	{
		if ((highLODLevel->peakActiveParticles_ > 0) || (spriteTemplate_->initialAllocationCount_ > 0))
		{
			// In-game... we assume the editor has set this properly, but still clamp at 100 to avoid wasting
			// memory.
			if (spriteTemplate_->initialAllocationCount_ > 0)
			{
				Resize(Min(spriteTemplate_->initialAllocationCount_, 100));
			}
			else
			{
				Resize(Min(highLODLevel->peakActiveParticles_, 100));
			}
		}
		else
		{
			// This is to force the editor to 'select' a value
			Resize(10);
		}
	}

	loopCount_ = 0;

	if (bNeedsInit)
	{
		// Propagate killon flags
		killOnDeactivate_ = highLODLevel->requiredModule_->killOnDeactivate_;
		killOnCompleted_ = highLODLevel->requiredModule_->killOnCompleted_;

		// Propagate sorting flag.
		sortMode_ = highLODLevel->requiredModule_->sortMode_;

		// Reset the burst lists
		if (burstFired_.Size() < spriteTemplate_->LODLevels_.Size())
		{
			burstFired_.Resize(spriteTemplate_->LODLevels_.Size());
		}

		for (Int32 LODIndex = 0; LODIndex < spriteTemplate_->LODLevels_.Size(); LODIndex++)
		{
			ParticleLODLevel* LODLevel = spriteTemplate_->LODLevels_[LODIndex];
			ASSERT(LODLevel);
			LODBurstFired& localBurstFired = burstFired_[LODIndex];
			if (localBurstFired.fired_.Size() < LODLevel->spawnModule_->burstList_.Size())
			{
				localBurstFired.fired_.Resize(LODLevel->spawnModule_->burstList_.Size());
			}
		}
	}

	ResetBurstList();

#if WITH_EDITORONLY_DATA
	//Check for SubUV module to see if it has SubUVAnimation to move data to required module
	for (auto currModule : highLODLevel->modules_)
	{
		if (currModule->IsInstanceOf(ParticleModuleSubUV::GetTypeStatic()))
		{
			ParticleModuleSubUV* subUVModule = (ParticleModuleSubUV*)currModule;

			if (subUVModule->Animation)
			{
				highLODLevel->requiredModule_->alphaThreshold_ = subUVModule->Animation->alphaThreshold_;
				highLODLevel->requiredModule_->boundingMode_ = subUVModule->Animation->boundingMode_;
				highLODLevel->requiredModule_->opacitySourceMode_ = subUVModule->Animation->opacitySourceMode_;
				highLODLevel->requiredModule_->cutoutTexture_ = subUVModule->Animation->subUVTexture_;

				subUVModule->animation_ = nullptr;

				highLODLevel->requiredModule_->CacheDerivedData();
				highLODLevel->requiredModule_->InitBoundingGeometryBuffer();
			}
		}
	}
#endif //WITH_EDITORONLY_DATA

	// Tag it as dirty w.r.t. the renderer
	isRenderDataDirty_ = 1;

	emitterIsDone_ = false;
}

void ParticleEmitterInstance::UpdateTransforms()
{
	//QUICK_SCOPE_CYCLE_COUNTER(STAT_EmitterInstance_UpdateTransforms);

	ASSERT(spriteTemplate_ != NULL);

	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();
	Matrix3x4 componentToWorld = component_ != NULL ?
		component_->GetNode()->GetWorldTransform() : Matrix3x4::IDENTITY;
	Matrix3x4 emitterToComponent = Matrix3x4(
		LODLevel->requiredModule_->emitterRotation_,
		LODLevel->requiredModule_->emitterOrigin_,
		Vector3::ONE
	);

	if (LODLevel->requiredModule_->useLocalSpace_)
	{
		emitterToSimulation_ = emitterToComponent;
		simulationToWorld_ = componentToWorld;
#if ENABLE_NAN_DIAGNOSTIC
		if (simulationToWorld_.ContainsNaN())
		{
			FLAGGG_LOG_ERROR("ParticleEmitterInstance::UpdateTransforms() - SimulationToWorld contains NaN!");
			simulationToWorld_ = Matrix3x4::IDENTITY;
		}
#endif
	}
	else
	{
		emitterToSimulation_ = emitterToComponent * componentToWorld;
		simulationToWorld_ = Matrix3x4::IDENTITY;
	}
}

/**
 * Ensures enough memory is allocated for the requested number of particles.
 *
 * @param NewMaxActiveParticles		The number of particles for which memory must be allocated.
 * @param bSetMaxActiveCount		If true, update the peak active particles for this LOD.
 * @returns bool					true if memory is allocated for at least NewMaxActiveParticles.
 */
bool ParticleEmitterInstance::Resize(Int32 newMaxActiveParticles, bool setMaxActiveCount)
{
	if (MAX_PARTICLE_RESIZE > 0)
	{
		if ((newMaxActiveParticles < 0) || (newMaxActiveParticles > MAX_PARTICLE_RESIZE))
		{
			if ((newMaxActiveParticles < 0) || (newMaxActiveParticles > MAX_PARTICLE_RESIZE_WARN))
			{
				FLAGGG_LOG_ERROR("Emitter::Resize> Invalid NewMaxActive (%d) for Emitter in PSys %s",
					newMaxActiveParticles,
					component_ ?
					component_->template_ ? component_->template_->GetName().CString()
					: "ParticleSystemComponent"
					:
					"INVALID COMPONENT");
			}

			return false;
		}
	}

	if (newMaxActiveParticles > maxActiveParticles_)
	{
		// Alloc (or realloc) the data array
		// Allocations > 16 byte are always 16 byte aligned so particleData_ can be used with SSE.
		// NOTE: We don't have to zero the memory here... It gets zeroed when grabbed later.
		{
			particleData_ = (UInt8*)realloc(particleData_, particleStride_ * newMaxActiveParticles);
			ASSERT(particleData_);

			// Allocate memory for indices.
			if (particleIndices_ == NULL)
			{
				// Make sure that we clear all when it is the first alloc
				maxActiveParticles_ = 0;
			}
			particleIndices_ = (UInt16*)realloc(particleIndices_, sizeof(UInt16) * (newMaxActiveParticles + 1));
		}

		// Fill in default 1:1 mapping.
		for (Int32 i = maxActiveParticles_; i < newMaxActiveParticles; i++)
		{
			particleIndices_[i] = i;
		}

		// Set the max count
		maxActiveParticles_ = newMaxActiveParticles;
	}

	// Set the PeakActiveParticles
	if (setMaxActiveCount)
	{
		ParticleLODLevel* LODLevel = spriteTemplate_->GetLODLevel(0);
		ASSERT(LODLevel);
		if (maxActiveParticles_ > LODLevel->peakActiveParticles_)
		{
			LODLevel->peakActiveParticles_ = maxActiveParticles_;
		}
	}

	return true;
}

/**
 *	Tick the instance.
 *
 *	@param	deltaTime			The time slice to use
 *	@param	suppressSpawning	If true, do not spawn during Tick
 */
void ParticleEmitterInstance::Tick(float deltaTime, bool suppressSpawning)
{
	ASSERT(spriteTemplate_);
	ASSERT(spriteTemplate_->LODLevels_.Size() > 0);

	// If this the FirstTime we are being ticked?
	bool firstTime = (secondsSinceCreation_ > 0.0f) ? false : true;

	// Grab the current LOD level
	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();

	// Handle emitterTime_ setup, looping, etc.
	float emitterDelay = Tick_EmitterTimeSetup(deltaTime, LODLevel);

	if (enabled_)
	{
		// Kill off any dead particles
		KillParticles();

		// Reset particle parameters.
		ResetParticleParameters(deltaTime);

		// Update the particles
		currentMaterial_ = LODLevel->requiredModule_->material_;
		Tick_ModuleUpdate(deltaTime, LODLevel);

		// Spawn new particles.
		spawnFraction_ = Tick_SpawnParticles(deltaTime, LODLevel, suppressSpawning, firstTime);

		// PostUpdate (beams only)
		Tick_ModulePostUpdate(deltaTime, LODLevel);

		if (activeParticles_ > 0)
		{
			// Update the orbit data...
			UpdateOrbitData(deltaTime);
			// Calculate bounding box and simulate velocity.
			UpdateBoundingBox(deltaTime);
		}

		Tick_ModuleFinalUpdate(deltaTime, LODLevel);

		CheckEmitterFinished();

		// Invalidate the contents of the vertex/index buffer.
		isRenderDataDirty_ = 1;
	}
	else
	{
		FakeBursts();
	}

	// 'Reset' the emitter time so that the delay functions correctly
	emitterTime_ += emitterDelay;

	// Store the last delta time.
	lastDeltaTime_ = deltaTime;

	// Reset particles position offset
	positionOffsetThisTick_ = Vector3::ZERO;
}


/**
*	Called from Tick to determine whether the emitter will no longer spawn particles
*   ASSERTs for emitters with 0 loops, infinite lifetime, and no continuous spawning (only bursts)
*	and sets bEmitterIsDone if the last burst lies in the past and there are no active particles
*	bEmitterIsDone is ASSERTed for all emitters by ParticleSystemComponent tick, and the particle
*	system is deactivated if it's true for all emitters, and if bAutoDeactivate is set on the ParticleSystem
*/
void ParticleEmitterInstance::CheckEmitterFinished()
{
	// Grab the current LOD level
	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();

	// figure out if this emitter will no longer spawn particles
	//
	if (this->activeParticles_ == 0)
	{
		ParticleModuleSpawn* spawnModule = LODLevel->spawnModule_;
		ASSERT(spawnModule);

		ParticleBurst* lastBurst = nullptr;
		if (spawnModule->burstList_.Size())
		{
			lastBurst = &spawnModule->burstList_.Back();
		}

		if (!lastBurst || lastBurst->time_ < this->emitterTime_)
		{
			const ParticleModuleRequired* RequiredModule = LODLevel->requiredModule_;
			ASSERT(RequiredModule);

			if (HasCompleted() ||
				(spawnModule->GetMaximumSpawnRate() == 0
					&& RequiredModule->emitterDuration_ == 0
					&& RequiredModule->emitterLoops_ == 0)
				)
			{
				emitterIsDone_ = true;
			}
		}
	}
}


/**
 *	Tick sub-function that handle emitterTime_ setup, looping, etc.
 *
 *	@param	deltaTime			The current time slice
 *	@param	currentLODLevel_		The current LOD level for the instance
 *
 *	@return	float				The EmitterDelay
 */
float ParticleEmitterInstance::Tick_EmitterTimeSetup(float deltaTime, ParticleLODLevel* inCurrentLODLevel)
{
	// Make sure we don't try and do any interpolation on the first frame we are attached (OldLocation is not valid in this circumstance)
	if (component_->justRegistered_)
	{
		location_ = component_->GetNode()->GetPosition();
		oldLocation_ = location_;
	}
	else
	{
		// Keep track of location for world- space interpolation and other effects.
		oldLocation_ = location_;
		location_ = component_->GetNode()->GetPosition();
	}

	UpdateTransforms();
	secondsSinceCreation_ += deltaTime;

	// Update time within emitter loop.
	bool bLooped = false;
	if (inCurrentLODLevel->requiredModule_->useLegacyEmitterTime_ == false)
	{
		emitterTime_ += deltaTime;
		bLooped = (emitterDuration_ > 0.0f) && (emitterTime_ >= emitterDuration_);
	}
	else
	{
		emitterTime_ = secondsSinceCreation_;
		if (emitterDuration_ > KINDA_SMALL_NUMBER)
		{
			emitterTime_ = Mod(secondsSinceCreation_, emitterDuration_);
			bLooped = ((secondsSinceCreation_ - (emitterDuration_ * loopCount_)) >= emitterDuration_);
		}
	}

	// Get the emitter delay time
	float emitterDelay = currentDelay_;

	// Determine if the emitter has looped
	if (bLooped)
	{
		loopCount_++;
		ResetBurstList();
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		// Reset the event count each loop...
		if (eventCount_ > maxEventCount_)
		{
			maxEventCount_ = eventCount_;
		}
		eventCount_ = 0;
#endif	//#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

		if (inCurrentLODLevel->requiredModule_->useLegacyEmitterTime_ == false)
		{
			emitterTime_ -= emitterDuration_;
		}

		if ((inCurrentLODLevel->requiredModule_->durationRecalcEachLoop_ == true)
			|| ((inCurrentLODLevel->requiredModule_->delayFirstLoopOnly_ == true) && (loopCount_ == 1))
			)
		{
			SetupEmitterDuration();
		}

		if (requiresLoopNotification_ == true)
		{
			for (Int32 ModuleIdx = -3; ModuleIdx < inCurrentLODLevel->modules_.Size(); ModuleIdx++)
			{
				Int32 ModuleFetchIdx;
				switch (ModuleIdx)
				{
				case -3:	ModuleFetchIdx = INDEX_REQUIREDMODULE;	break;
				case -2:	ModuleFetchIdx = INDEX_SPAWNMODULE;		break;
				case -1:	ModuleFetchIdx = INDEX_TYPEDATAMODULE;	break;
				default:	ModuleFetchIdx = ModuleIdx;				break;
				}

				ParticleModule* Module = inCurrentLODLevel->GetModuleAtIndex(ModuleFetchIdx);
				if (Module != NULL)
				{
					if (Module->RequiresLoopingNotification() == true)
					{
						Module->EmitterLoopingNotify(this);
					}
				}
			}
		}
	}

	// Don't delay unless required
	if ((inCurrentLODLevel->requiredModule_->delayFirstLoopOnly_ == true) && (loopCount_ > 0))
	{
		emitterDelay = 0;
	}

	// 'Reset' the emitter time so that the modules function correctly
	emitterTime_ -= emitterDelay;

	return emitterDelay;
}

/**
 *	Tick sub-function that handles spawning of particles
 *
 *	@param	deltaTime			The current time slice
 *	@param	currentLODLevel		The current LOD level for the instance
 *	@param	suppressSpawning	true if spawning has been suppressed on the owning particle system component
 *	@param	firstTime			true if this is the first time the instance has been ticked
 *
 *	@return	float				The SpawnFraction remaining
 */
float ParticleEmitterInstance::Tick_SpawnParticles(float deltaTime, ParticleLODLevel* inCurrentLODLevel, bool suppressSpawning, bool firstTime)
{
	if (!haltSpawning_ && !haltSpawningExternal_ && !suppressSpawning && (emitterTime_ >= 0.0f))
	{
		// If emitter is not done - spawn at current rate.
		// If EmitterLoops is 0, then we loop forever, so always spawn.
		if ((inCurrentLODLevel->requiredModule_->emitterLoops_ == 0) ||
			(loopCount_ < inCurrentLODLevel->requiredModule_->emitterLoops_) ||
			(secondsSinceCreation_ < (emitterDuration_ * inCurrentLODLevel->requiredModule_->emitterLoops_)) ||
			firstTime)
		{
			firstTime = false;
			spawnFraction_ = Spawn(deltaTime);
		}
	}
	else if (fakeBurstsWhenSpawningSupressed_)
	{
		FakeBursts();
	}

	return spawnFraction_;
}

/**
 *	Tick sub-function that handles module updates
 *
 *	@param	deltaTime			The current time slice
 *	@param	currentLODLevel_		The current LOD level for the instance
 */
void ParticleEmitterInstance::Tick_ModuleUpdate(float deltaTime, ParticleLODLevel* inCurrentLODLevel)
{
	ParticleLODLevel* highestLODLevel = spriteTemplate_->LODLevels_[0];
	ASSERT(highestLODLevel);
	for (Int32 moduleIndex = 0; moduleIndex < inCurrentLODLevel->updateModules_.Size(); moduleIndex++)
	{
		ParticleModule* currentModule = inCurrentLODLevel->updateModules_[moduleIndex];
		if (currentModule && currentModule->enabled_ && currentModule->updateModule_)
		{
			currentModule->Update(this, GetModuleDataOffset(highestLODLevel->updateModules_[moduleIndex]), deltaTime);
		}
	}
}

/**
 *	Tick sub-function that handles module post updates
 *
 *	@param	deltaTime			The current time slice
 *	@param	currentLODLevel_		The current LOD level for the instance
 */
void ParticleEmitterInstance::Tick_ModulePostUpdate(float deltaTime, ParticleLODLevel* inCurrentLODLevel)
{
	// Handle the TypeData module
	if (inCurrentLODLevel->typeDataModule_)
	{
		inCurrentLODLevel->typeDataModule_->Update(this, typeDataOffset_, deltaTime);
	}
}

/**
 *	Tick sub-function that handles module FINAL updates
 *
 *	@param	deltaTime			The current time slice
 *	@param	currentLODLevel_		The current LOD level for the instance
 */
void ParticleEmitterInstance::Tick_ModuleFinalUpdate(float deltaTime, ParticleLODLevel* inCurrentLODLevel)
{
	ParticleLODLevel* highestLODLevel = spriteTemplate_->LODLevels_[0];
	ASSERT(highestLODLevel);
	for (Int32 moduleIndex = 0; moduleIndex < inCurrentLODLevel->updateModules_.Size(); moduleIndex++)
	{
		ParticleModule* currentModule = inCurrentLODLevel->updateModules_[moduleIndex];
		if (currentModule && currentModule->enabled_ && currentModule->finalUpdateModule_)
		{
			currentModule->FinalUpdate(this, GetModuleDataOffset(highestLODLevel->updateModules_[moduleIndex]), deltaTime);
		}
	}


	if (inCurrentLODLevel->typeDataModule_ && inCurrentLODLevel->typeDataModule_->enabled_ && inCurrentLODLevel->typeDataModule_->finalUpdateModule_)
	{
		inCurrentLODLevel->typeDataModule_->FinalUpdate(this, GetModuleDataOffset(highestLODLevel->typeDataModule_), deltaTime);
	}
}

/**
 *	Set the LOD to the given index
 *
 *	@param	inLODIndex			The index of the LOD to set as current
 *	@param	inFullyProcess		If true, process burst lists, etc.
 */
void ParticleEmitterInstance::SetCurrentLODIndex(Int32 inLODIndex, bool inFullyProcess)
{
	if (spriteTemplate_ != NULL)
	{
		currentLODLevelIndex_ = inLODIndex;
		// ASSERT to make certain the data in the content actually represents what we are being asked to render
		if (spriteTemplate_->LODLevels_.Size() > currentLODLevelIndex_)
		{
			currentLODLevel_ = spriteTemplate_->LODLevels_[currentLODLevelIndex_];
		}
		// set to the LOD which is guaranteed to exist
		else
		{
			currentLODLevelIndex_ = 0;
			currentLODLevel_ = spriteTemplate_->LODLevels_[currentLODLevelIndex_];
		}
		emitterDuration_ = emitterDurations_[currentLODLevelIndex_];

		ASSERT(currentLODLevel_);
		ASSERT(currentLODLevel_->requiredModule_);

		if (inFullyProcess == true)
		{
			killOnCompleted_ = currentLODLevel_->requiredModule_->killOnCompleted_;
			killOnDeactivate_ = currentLODLevel_->requiredModule_->killOnDeactivate_;

			// Check for bursts that should have been fired already...
			ParticleModuleSpawn* spawnModule = currentLODLevel_->spawnModule_;
			LODBurstFired* localBurstFired = NULL;

			if (currentLODLevelIndex_ + 1 > burstFired_.Size())
			{
				// This should not happen, but catch it just in case...
				burstFired_.Resize(currentLODLevelIndex_ + 1);
			}
			localBurstFired = &(burstFired_[currentLODLevelIndex_]);

			if (localBurstFired->fired_.Size() < spawnModule->burstList_.Size())
			{
				localBurstFired->fired_.Resize(spawnModule->burstList_.Size());
			}

			for (Int32 BurstIndex = 0; BurstIndex < spawnModule->burstList_.Size(); BurstIndex++)
			{
				if (currentLODLevel_->requiredModule_->emitterDelay_ + spawnModule->burstList_[BurstIndex].time_ < emitterTime_)
				{
					localBurstFired->fired_[BurstIndex] = true;
				}
			}
		}

		if (currentLODLevel_->enabled_ == false)
		{
			// Kill active particles...
			KillParticlesForced();
		}
	}
}

/**
 *	Rewind the instance.
 */
void ParticleEmitterInstance::Rewind()
{
	secondsSinceCreation_ = 0;
	emitterTime_ = 0;
	loopCount_ = 0;
	particleCounter_ = 0;
	enabled_ = 1;
	ResetBurstList();
}

/**
 *	Retrieve the bounding box for the instance
 *
 *	@return	BoundingBox	The bounding box
 */
BoundingBox ParticleEmitterInstance::GetBoundingBox()
{
	return particleBoundingBox_;
}

Int32 ParticleEmitterInstance::GetOrbitPayloadOffset()
{
	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();

	Int32 orbitOffsetValue = -1;
	if (LODLevel->orbitModules_.Size() > 0)
	{
		ParticleLODLevel* highestLODLevel = spriteTemplate_->LODLevels_[0];
		ASSERT(highestLODLevel);

		ParticleModuleOrbit* orbitModule = highestLODLevel->orbitModules_[LODLevel->orbitModules_.Size() - 1];
		if (orbitModule)
		{
			UInt32* orbitOffsetIndex = spriteTemplate_->moduleOffsetMap_.TryGetValue(orbitModule);
			if (orbitOffsetIndex)
			{
				orbitOffsetValue = *orbitOffsetIndex;
			}
		}
	}
	return orbitOffsetValue;
}

Vector3 ParticleEmitterInstance::GetParticleLocationWithOrbitOffset(BaseParticle* particle)
{
	Int32 orbitOffsetValue = GetOrbitPayloadOffset();
	if (orbitOffsetValue == -1)
	{
		return particle->location_;
	}
	else
	{
		Int32 currentOffset = orbitOffsetValue;
		const UInt8* particleBase = (const UInt8*)particle;
		PARTICLE_ELEMENT(OrbitChainModuleInstancePayload, orbitPayload);
		return particle->location_ + Vector3(orbitPayload.offset_);
	}
}

/**
 *	Update the bounding box for the emitter
 *
 *	@param	deltaTime		The time slice to use
 */
void ParticleEmitterInstance::UpdateBoundingBox(float deltaTime)
{
	if (component_)
	{
		bool updateBox = ((component_->warmingUp_ == false) && (component_->template_ != NULL) && (component_->template_->useFixedRelativeBoundingBox_ == false));

		// Take component scale into account
		Vector3 Scale = component_->GetNode()->GetWorldScale();

		ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();

		Vector3	newLocation;
		float	newRotation;
		if (updateBox)
		{
			particleBoundingBox_.Clear();
		}
		ParticleLODLevel* highestLODLevel = spriteTemplate_->LODLevels_[0];
		ASSERT(highestLODLevel);

		Vector3 particlepivotOffset(-0.5f, -0.5f, 0.0f);
		if (updateBox)
		{
			UInt32 numModules = highestLODLevel->modules_.Size();
			for (UInt32 i = 0; i < numModules; ++i)
			{
				ParticleModulePivotOffset* module = RTTICast<ParticleModulePivotOffset>(highestLODLevel->modules_[i]);
				if (module)
				{
					Vector2 pivotOff = module->pivotOffset_;
					particlepivotOffset += Vector3(pivotOff.x_, pivotOff.y_, 0.0f);
					break;
				}
			}
		}

		// Store off the orbit offset, if there is one
		Int32 orbitOffsetValue = GetOrbitPayloadOffset();

		// For each particle, offset the box appropriately 
		Vector3 minVal(F_INFINITY * 0.5, F_INFINITY * 0.5, F_INFINITY * 0.5);
		Vector3 maxVal(-F_INFINITY * 0.5, -F_INFINITY * 0.5, -F_INFINITY * 0.5);

		const bool useLocalSpace = LODLevel->requiredModule_->useLocalSpace_;

		const Matrix3x4 componentToWorld = useLocalSpace
			? component_->GetNode()->GetWorldTransform()
			: Matrix3x4::IDENTITY;

		bool skipDoubleSpawnUpdate = !spriteTemplate_->useLegacySpawningBehavior_;
		for (Int32 i = 0; i < activeParticles_; i++)
		{
			DECLARE_PARTICLE(particle, particleData_ + particleStride_ * particleIndices_[i]);

			// Do linear integrator and update bounding box
			// Do angular integrator, and wrap result to within +/- 2 PI
			particle.oldLocation_ = particle.location_;

			bool justSpawned = (particle.flags_ & STATE_Particle_JustSpawned) != 0;
			particle.flags_ &= ~STATE_Particle_JustSpawned;

			//Don't update position for newly spawned particles. They already have a partial update applied during spawn.
			bool skipUpdate = justSpawned && skipDoubleSpawnUpdate;

			if ((particle.flags_ & STATE_Particle_Freeze) == 0 && !skipUpdate)
			{
				if ((particle.flags_ & STATE_Particle_FreezeTranslation) == 0)
				{
					newLocation = particle.location_ + Vector3(deltaTime * particle.velocity_);
				}
				else
				{
					newLocation = particle.location_;
				}
				if ((particle.flags_ & STATE_Particle_FreezeRotation) == 0)
				{
					newRotation = (deltaTime * particle.rotationRate_) + particle.rotation_;
				}
				else
				{
					newRotation = particle.rotation_;
				}
			}
			else
			{
				newLocation = particle.location_;
				newRotation = particle.rotation_;
			}

			Real localMax(0.0f);

			if (updateBox)
			{
				if (orbitOffsetValue == -1)
				{
					localMax = (Vector3(particle.size_) * Scale).GetAbsMax();
				}
				else
				{
					Int32 currentOffset = orbitOffsetValue;
					const UInt8* particleBase = (const UInt8*)&particle;
					PARTICLE_ELEMENT(OrbitChainModuleInstancePayload, orbitPayload);
					localMax = orbitPayload.offset_.GetAbsMax();
				}

				localMax += (Vector3(particle.size_) * particlepivotOffset).GetAbsMax();
			}

			newLocation += positionOffsetThisTick_;
			particle.oldLocation_ += positionOffsetThisTick_;

			particle.location_ = newLocation;
			particle.rotation_ = Mod(newRotation, 2.f * (float)PI);

			if (updateBox)
			{
				Vector3 positionForBounds = newLocation;

				if (useLocalSpace)
				{
					// Note: building the bounding box in world space as that gives tighter bounds than transforming a local space AABB into world space
					positionForBounds = componentToWorld * newLocation;
				}

				// Treat each particle as a cube whose sides are the length of the maximum component
				// This handles the particle's extents changing due to being camera facing
				minVal[0] = Min(minVal[0], positionForBounds.x_ - localMax);
				maxVal[0] = Max(maxVal[0], positionForBounds.x_ + localMax);
				minVal[1] = Min(minVal[1], positionForBounds.y_ - localMax);
				maxVal[1] = Max(maxVal[1], positionForBounds.y_ + localMax);
				minVal[2] = Min(minVal[2], positionForBounds.z_ - localMax);
				maxVal[2] = Max(maxVal[2], positionForBounds.z_ + localMax);
			}
		}

		if (updateBox)
		{
			particleBoundingBox_ = BoundingBox(minVal, maxVal);
		}
	}
}

/**
 * Force the bounding box to be updated.
 */
void ParticleEmitterInstance::ForceUpdateBoundingBox()
{
	if (component_)
	{
		// Take component scale into account
		Vector3 scale = component_->GetNode()->GetWorldScale();

		ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();
		ParticleLODLevel* highestLODLevel = spriteTemplate_->LODLevels_[0];
		ASSERT(highestLODLevel);

		particleBoundingBox_.Clear();

		// Store off the orbit offset, if there is one
		Int32 orbitOffsetValue = GetOrbitPayloadOffset();

		const bool useLocalSpace = LODLevel->requiredModule_->useLocalSpace_;

		const Matrix3x4 componentToWorld = useLocalSpace
			? component_->GetNode()->GetWorldTransform()
			: Matrix3x4::IDENTITY;

		// For each particle, offset the box appropriately 
		Vector3 minVal(F_INFINITY * 0.5, F_INFINITY * 0.5, F_INFINITY * 0.5);
		Vector3 maxVal(-F_INFINITY * 0.5, -F_INFINITY * 0.5, -F_INFINITY * 0.5);

		for (Int32 i = 0; i < activeParticles_; i++)
		{
			DECLARE_PARTICLE(particle, particleData_ + particleStride_ * particleIndices_[i]);

			Real localMax(0.0f);

			if (orbitOffsetValue == -1)
			{
				localMax = (Vector3(particle.size_) * scale).GetAbsMax();
			}
			else
			{
				Int32 currentOffset = orbitOffsetValue;
				const UInt8* particleBase = (const UInt8*)&particle;
				PARTICLE_ELEMENT(OrbitChainModuleInstancePayload, orbitPayload);
				localMax = orbitPayload.offset_.GetAbsMax();
			}

			Vector3 positionForBounds = particle.location_;

			if (useLocalSpace)
			{
				// Note: building the bounding box in world space as that gives tighter bounds than transforming a local space AABB into world space
				positionForBounds = componentToWorld * particle.location_;
			}

			// Treat each particle as a cube whose sides are the length of the maximum component
			// This handles the particle's extents changing due to being camera facing
			minVal[0] = Min(minVal[0], positionForBounds.x_ - localMax);
			maxVal[0] = Max(maxVal[0], positionForBounds.x_ + localMax);
			minVal[1] = Min(minVal[1], positionForBounds.y_ - localMax);
			maxVal[1] = Max(maxVal[1], positionForBounds.y_ + localMax);
			minVal[2] = Min(minVal[2], positionForBounds.z_ - localMax);
			maxVal[2] = Max(maxVal[2], positionForBounds.z_ + localMax);
		}

		particleBoundingBox_ = BoundingBox(minVal, maxVal);
	}
}

/**
 *	Retrieved the per-particle bytes that this emitter type requires.
 *
 *	@return	UInt32	The number of required bytes for particles in the instance
 */
UInt32 ParticleEmitterInstance::RequiredBytes()
{
	// If ANY LOD level has subUV, the size must be taken into account.
	UInt32 uiBytes = 0;
	bool hasSubUV = false;
	for (Int32 LODIndex = 0; (LODIndex < spriteTemplate_->LODLevels_.Size()) && !hasSubUV; LODIndex++)
	{
		// This code assumes that the module stacks are identical across LOD levevls...
		ParticleLODLevel* LODLevel = spriteTemplate_->GetLODLevel(LODIndex);

		if (LODLevel)
		{
			ParticleSubUVInterpMethod interpolationMethod = (ParticleSubUVInterpMethod)LODLevel->requiredModule_->interpolationMethod_;
			if (LODIndex > 0)
			{
				if ((interpolationMethod != PSUVIM_None) && (hasSubUV == false))
				{
					FLAGGG_LOG_WARN("Emitter w/ mismatched SubUV settings: %s",
						component_ ?
						component_->template_ ?
						component_->template_->GetName().CString() :
						"ParticleSystemComponent" :
						"INVALID PSYS!");
				}

				if ((interpolationMethod == PSUVIM_None) && (hasSubUV == true))
				{
					FLAGGG_LOG_WARN("Emitter w/ mismatched SubUV settings: %s",
						component_ ?
						component_->template_ ?
						component_->template_->GetName().CString() :
						"ParticleSystemComponent" :
						"INVALID PSYS!");
				}
			}
			// Check for SubUV utilization, and update the required bytes accordingly
			if (interpolationMethod != PSUVIM_None)
			{
				hasSubUV = true;
			}
		}
	}

	if (hasSubUV)
	{
		subUVDataOffset_ = payloadOffset_;
		uiBytes = sizeof(FullSubUVPayload);
	}

	return uiBytes;
}

UInt32 ParticleEmitterInstance::GetModuleDataOffset(ParticleModule* module)
{
	ASSERT(spriteTemplate_);

	UInt32* Offset = spriteTemplate_->moduleOffsetMap_.TryGetValue(module);
	return (Offset != nullptr) ? *Offset : 0;
}

UInt8* ParticleEmitterInstance::GetModuleInstanceData(ParticleModule* module)
{
	// If there is instance data present, look up the modules offset
	if (instanceData_)
	{
		UInt32* offset = spriteTemplate_->moduleInstanceOffsetMap_.TryGetValue(module);
		if (offset)
		{
			ASSERT(*offset < (UInt32)instancePayloadSize_);
			return &(instanceData_[*offset]);
		}
	}
	return NULL;
}

/** Get pointer to emitter instance random seed payload data for a particular module */
ParticleRandomSeedInstancePayload* ParticleEmitterInstance::GetModuleRandomSeedInstanceData(ParticleModule* module)
{
	// If there is instance data present, look up the modules offset
	if (instanceData_)
	{
		UInt32* Offset = spriteTemplate_->moduleRandomSeedInstanceOffsetMap_.TryGetValue(module);
		if (Offset)
		{
			ASSERT(*Offset < (UInt32)instancePayloadSize_);
			return (ParticleRandomSeedInstancePayload*)&(instanceData_[*Offset]);
		}
	}
	return NULL;
}

/**
 *	Get the pointer to the instance data allocated for type data module.
 *
 *	@return	UInt8*		The pointer to the data
 */
UInt8* ParticleEmitterInstance::GetTypeDataModuleInstanceData()
{
	if (instanceData_ && (typeDataInstanceOffset_ != -1))
	{
		return &(instanceData_[typeDataInstanceOffset_]);
	}
	return NULL;
}

/**
 *	Calculate the stride of a single particle for this instance
 *
 *	@param	particleSize_	The size of the particle
 *
 *	@return	UInt32			The stride of the particle
 */
UInt32 ParticleEmitterInstance::CalculateParticleStride(UInt32 inparticleSize)
{
	return inparticleSize;
}

/**
 *	Reset the burst list information for the instance
 */
void ParticleEmitterInstance::ResetBurstList()
{
	for (Int32 BurstIndex = 0; BurstIndex < burstFired_.Size(); BurstIndex++)
	{
		LODBurstFired& currBurstFired = burstFired_[BurstIndex];
		for (Int32 firedIndex = 0; firedIndex < currBurstFired.fired_.Size(); firedIndex++)
		{
			currBurstFired.fired_[firedIndex] = false;
		}
	}
}

/**
 *	Get the current burst rate offset (delta time is artificially increased to generate bursts)
 *
 *	@param	deltaTime	The time slice (In/Out)
 *	@param	Burst		The number of particles to burst (Output)
 *
 *	@return	float		The time slice increase to use
 */
float ParticleEmitterInstance::GetCurrentBurstRateOffset(float& deltaTime, Int32& burst)
{
	float spawnRateInc = 0.0f;

	// Grab the current LOD level
	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();
	if (LODLevel->spawnModule_->burstList_.Size() > 0)
	{
		RandomStream& randomStream = LODLevel->spawnModule_->GetRandomStream(this);

		// For each burst in the list
		for (Int32 burstIdx = 0; burstIdx < LODLevel->spawnModule_->burstList_.Size(); burstIdx++)
		{
			ParticleBurst* burstEntry = &(LODLevel->spawnModule_->burstList_[burstIdx]);
			// If it hasn't been fired
			if (burstEntry && LODLevel->level_ < burstFired_.Size())
			{
				LODBurstFired& localBurstFired = burstFired_[LODLevel->level_];
				if (burstIdx < localBurstFired.fired_.Size())
				{
					if (localBurstFired.fired_[burstIdx] == false)
					{
						// If it is time to fire it
						if (emitterTime_ >= burstEntry->time_)
						{
							// Make sure there is a valid time slice
							if (deltaTime < 0.00001f)
							{
								deltaTime = 0.00001f;
							}
							// Calculate the increase time slice
							Int32 count = burstEntry->count_;
							if (burstEntry->countLow_ > -1)
							{
								count = randomStream.RandRange(burstEntry->countLow_, burstEntry->count_);
							}
							// Take in to account scale.
							float scale = LODLevel->spawnModule_->burstScale_.GetValue(emitterTime_, component_);
							count = CeilToInt(count * scale);
							spawnRateInc += count / deltaTime;
							burst += count;
							localBurstFired.fired_[burstIdx] = true;
						}
					}
				}
			}
		}
	}

	return spawnRateInc;
}

/**
 *	Reset the particle parameters
 */
void ParticleEmitterInstance::ResetParticleParameters(float deltaTime)
{
	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();
	ParticleLODLevel* highestLODLevel = spriteTemplate_->LODLevels_[0];
	ASSERT(highestLODLevel);

	// Store off any orbit offset values
	PODVector<Int32> orbitOffsets;
	orbitOffsets.Reserve(8);
	Int32 orbitCount = LODLevel->orbitModules_.Size();
	for (Int32 orbitIndex = 0; orbitIndex < orbitCount; orbitIndex++)
	{
		ParticleModuleOrbit* orbitModule = highestLODLevel->orbitModules_[orbitIndex];
		if (orbitModule)
		{
			UInt32* orbitOffset = spriteTemplate_->moduleOffsetMap_.TryGetValue(orbitModule);
			if (orbitOffset)
			{
				orbitOffsets.Push(*orbitOffset);
			}
		}
	}

	bool skipDoubleSpawnUpdate = !spriteTemplate_->useLegacySpawningBehavior_;
	for (Int32 particleIndex = 0; particleIndex < activeParticles_; particleIndex++)
	{
		DECLARE_PARTICLE(particle, particleData_ + particleStride_ * particleIndices_[particleIndex]);
		particle.velocity_ = particle.baseVelocity_;
		particle.size_ = GetParticleBaseSize(particle);
		particle.rotationRate_ = particle.baseRotationRate_;
		particle.color_ = particle.baseColor_;

		bool bJustSpawned = (particle.flags_ & STATE_Particle_JustSpawned) != 0;

		//Don't update position for newly spawned particles. They already have a partial update applied during spawn.
		bool skipUpdate = bJustSpawned && skipDoubleSpawnUpdate;

		particle.relativeTime_ += skipUpdate ? 0.0f : particle.oneOverMaxLifetime_ * deltaTime;

		if (cameraPayloadOffset_ > 0)
		{
			Int32 currentOffset = cameraPayloadOffset_;
			const UInt8* particleBase = (const UInt8*)&particle;
			PARTICLE_ELEMENT(CameraOffsetParticlePayload, cameraOffsetPayload);
			cameraOffsetPayload.offset_ = cameraOffsetPayload.baseOffset_;
		}
		for (Int32 OrbitIndex = 0; OrbitIndex < orbitOffsets.Size(); OrbitIndex++)
		{
			Int32 currentOffset = orbitOffsets[OrbitIndex];
			const UInt8* particleBase = (const UInt8*)&particle;
			PARTICLE_ELEMENT(OrbitChainModuleInstancePayload, orbitPayload);
			orbitPayload.previousOffset_ = orbitPayload.offset_;
			orbitPayload.offset_ = orbitPayload.baseOffset_;
			orbitPayload.rotationRate_ = orbitPayload.baseRotationRate_;
		}
	}
}

/**
 *	Calculate the orbit offset data.
 */
void ParticleEmitterInstance::CalculateOrbitOffset(OrbitChainModuleInstancePayload& payload,
	Vector3& accumOffset, Vector3& accumRotation, Vector3& accumRotationRate,
	float deltaTime, Vector3& result, Matrix3x4& rotationMat)
{
	accumRotation += accumRotationRate * deltaTime;
	payload.rotation_ = Vector3(accumRotation);
	if (accumRotation.IsNearlyZero() == false)
	{
		Vector3 rotRot = rotationMat * accumRotation;
		Vector3 scaledRotation = rotRot * 360.0f;
		Quaternion rotator(scaledRotation.y_, scaledRotation.z_, scaledRotation.x_);
		Matrix3x4 rotMat(Vector3::ZERO, rotator, 1);

		rotationMat = rotationMat * rotMat;

		result = rotationMat * accumOffset;
	}
	else
	{
		result = accumOffset;
	}

	accumOffset = Vector3::ZERO;
	accumRotation = Vector3::ZERO;
	accumRotationRate = Vector3::ZERO;
}

void ParticleEmitterInstance::UpdateOrbitData(float deltaTime)
{
	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();
	Int32 moduleCount = LODLevel->orbitModules_.Size();
	if (moduleCount > 0)
	{
		ParticleLODLevel* highestLODLevel = spriteTemplate_->LODLevels_[0];
		ASSERT(highestLODLevel);

		PODVector<Vector3> offsets;
		offsets.Resize(moduleCount + 1);

		PODVector<Int32> moduleOffsets;
		moduleOffsets.Resize(moduleCount + 1);
		for (Int32 modOffIndex = 0; modOffIndex < moduleCount; modOffIndex++)
		{
			ParticleModuleOrbit* highestOrbitModule = highestLODLevel->orbitModules_[modOffIndex];
			ASSERT(highestOrbitModule);

			moduleOffsets[modOffIndex] = GetModuleDataOffset(highestOrbitModule);
		}

		for (Int32 i = activeParticles_ - 1; i >= 0; i--)
		{
			Int32 offsetIndex = 0;
			const Int32	currentIndex = particleIndices_[i];
			const UInt8* particleBase = particleData_ + currentIndex * particleStride_;
			BaseParticle& particle = *((BaseParticle*)particleBase);
			if ((particle.flags_ & STATE_Particle_Freeze) == 0)
			{
				Vector3 accumulatedOffset;
				Vector3 accumulatedRotation;
				Vector3 accumulatedRotationRate;

				OrbitChainModuleInstancePayload* localOrbitPayload = NULL;
				OrbitChainModuleInstancePayload* prevOrbitPayload = NULL;
				UInt8 prevOrbitChainMode = 0;
				Matrix3x4 accumRotMatrix;

				for (Int32 orbitIndex = 0; orbitIndex < moduleCount; orbitIndex++)
				{
					Int32 currentOffset = moduleOffsets[orbitIndex];
					ParticleModuleOrbit* orbitModule = LODLevel->orbitModules_[orbitIndex];
					ASSERT(orbitModule);

					if (currentOffset == 0)
					{
						continue;
					}

					PARTICLE_ELEMENT(OrbitChainModuleInstancePayload, orbitPayload);

					// The last orbit module holds the last final offset position
					bool calculateOffset = false;
					if (orbitIndex == (moduleCount - 1))
					{
						localOrbitPayload = &orbitPayload;
						calculateOffset = true;
					}

					// Determine the offset, rotation, rotationrate for the current particle
					if (orbitModule->chainMode_ == EOChainMode_Add)
					{
						if (orbitModule->enabled_ == true)
						{
							accumulatedOffset += Vector3(orbitPayload.offset_);
							accumulatedRotation += Vector3(orbitPayload.rotation_);
							accumulatedRotationRate += Vector3(orbitPayload.rotationRate_);
						}
					}
					else if (orbitModule->chainMode_ == EOChainMode_Scale)
					{
						if (orbitModule->enabled_ == true)
						{
							accumulatedOffset *= Vector3(orbitPayload.offset_);
							accumulatedRotation *= Vector3(orbitPayload.rotation_);
							accumulatedRotationRate *= Vector3(orbitPayload.rotationRate_);
						}
					}
					else if (orbitModule->chainMode_ == EOChainMode_Link)
					{
						if ((orbitIndex > 0) && (prevOrbitChainMode == EOChainMode_Link))
						{
							// Calculate the offset with the current accumulation
							Vector3 ResultOffset;
							CalculateOrbitOffset(*prevOrbitPayload,
								accumulatedOffset, accumulatedRotation, accumulatedRotationRate,
								deltaTime, ResultOffset, accumRotMatrix);
							if (orbitModule->enabled_ == false)
							{
								accumulatedOffset = Vector3::ZERO;
								accumulatedRotation = Vector3::ZERO;
								accumulatedRotationRate = Vector3::ZERO;
							}
							offsets[offsetIndex++] = ResultOffset;
						}

						if (orbitModule->enabled_ == true)
						{
							accumulatedOffset = Vector3(orbitPayload.offset_);
							accumulatedRotation = Vector3(orbitPayload.rotation_);
							accumulatedRotationRate = Vector3(orbitPayload.rotationRate_);
						}
					}

					if (calculateOffset == true)
					{
						// Push the current offset into the array
						Vector3 ResultOffset;
						CalculateOrbitOffset(orbitPayload,
							accumulatedOffset, accumulatedRotation, accumulatedRotationRate,
							deltaTime, ResultOffset, accumRotMatrix);
						offsets[offsetIndex++] = ResultOffset;
					}

					if (orbitModule->enabled_)
					{
						prevOrbitPayload = &orbitPayload;
						prevOrbitChainMode = orbitModule->chainMode_;
					}
				}

				if (localOrbitPayload != NULL)
				{
					localOrbitPayload->offset_ = Vector3::ZERO;
					for (Int32 accumIndex = 0; accumIndex < offsetIndex; accumIndex++)
					{
						localOrbitPayload->offset_ += Vector3(offsets[accumIndex]);
					}

					memset(&offsets[0], 0, sizeof(Vector3) * (moduleCount + 1));
				}
			}
		}
	}
}

void ParticleEmitterInstance::ParticlePrefetch()
{
	for (Int32 particleIndex = 0; particleIndex < activeParticles_; particleIndex++)
	{
		PARTICLE_INSTANCE_PREFETCH(this, particleIndex);
	}
}

void ParticleEmitterInstance::CheckSpawnCount(Int32 inNewCount, Int32 inMaxCount)
{

}

/**
 *	Spawn particles for this emitter instance
 *
 *	@param	deltaTime		The time slice to spawn over
 *
 *	@return	float			The leftover fraction of spawning
 */
float ParticleEmitterInstance::Spawn(float deltaTime)
{
	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();

	// For beams, we probably want to ignore the SpawnRate distribution,
	// and focus strictly on the BurstList...
	float spawnRate = 0.0f;
	Int32 spawnCount = 0;
	Int32 burstCount = 0;
	float spawnRateDivisor = 0.0f;
	float oldLeftover = spawnFraction_;

	ParticleLODLevel* highestLODLevel = spriteTemplate_->LODLevels_[0];

	bool processSpawnRate = true;
	bool processBurstList = true;
	Int32 detailMode = component_->GetCurrentDetailMode();

	if (spriteTemplate_->qualityLevelSpawnRateScale_ > 0.0f)
	{
		// Process all Spawning modules that are present in the emitter.
		for (Int32 spawnModIndex = 0; spawnModIndex < LODLevel->spawningModules_.Size(); spawnModIndex++)
		{
			ParticleModuleSpawnBase* spawnModule = LODLevel->spawningModules_[spawnModIndex];
			if (spawnModule && spawnModule->enabled_)
			{
				ParticleModule* offsetModule = highestLODLevel->spawningModules_[spawnModIndex];
				UInt32 offset = GetModuleDataOffset(offsetModule);

				// Update the spawn rate
				Int32 number = 0;
				float rate = 0.0f;
				if (spawnModule->GetSpawnAmount(this, offset, oldLeftover, deltaTime, number, rate) == false)
				{
					processSpawnRate = false;
				}

				number = Max<Int32>(0, number);
				rate = Max<float>(0.0f, rate);

				spawnCount += number;
				spawnRate += rate;
				// Update the burst list
				Int32 burstNumber = 0;
				if (spawnModule->GetBurstCount(this, offset, oldLeftover, deltaTime, burstNumber) == false)
				{
					processBurstList = false;
				}

				burstCount += burstNumber;
			}
		}

		// Figure out spawn rate for this tick.
		if (processSpawnRate)
		{
			float rateScale = LODLevel->spawnModule_->rateScale_.GetValue(emitterTime_, component_) * LODLevel->spawnModule_->GetGlobalRateScale();
			spawnRate += LODLevel->spawnModule_->rate_.GetValue(emitterTime_, component_) * rateScale;
			spawnRate = Max<float>(0.0f, spawnRate);
		}

		// Take Bursts into account as well...
		if (processBurstList)
		{
			Int32 burst = 0;
			float burstTime = GetCurrentBurstRateOffset(deltaTime, burst);
			burstCount += burst;
		}

		float qualityMult = spriteTemplate_->GetQualityLevelSpawnRateMult();
		spawnRate = Max<float>(0.0f, spawnRate * qualityMult);
		burstCount = CeilToInt(burstCount * qualityMult);
	}
	else
	{
		// Disable any spawning if MediumDetailSpawnRateScale is 0 and we are not in high detail mode
		spawnRate = 0.0f;
		spawnCount = 0;
		burstCount = 0;
	}

	// Spawn new particles...
	if ((spawnRate > 0.f) || (burstCount > 0))
	{
		float safetyLeftover = oldLeftover;
		// Ensure continuous spawning... lots of fiddling.
		float newLeftover = oldLeftover + deltaTime * spawnRate;
		Int32 number = FloorToInt(newLeftover);
		float increment = (spawnRate > 0.0f) ? (1.f / spawnRate) : 0.0f;
		float startTime = deltaTime + oldLeftover * increment - increment;
		newLeftover = newLeftover - number;

		// Handle growing arrays.
		bool processSpawn = true;
		Int32 newCount = activeParticles_ + number + burstCount;

		if (newCount > FXConsoleVariables::MaxCPUParticlesPerEmitter)
		{
			Int32 maxNewParticles = FXConsoleVariables::MaxCPUParticlesPerEmitter - activeParticles_;
			burstCount = Min(maxNewParticles, burstCount);
			maxNewParticles -= burstCount;
			number = Min(maxNewParticles, number);
			newCount = activeParticles_ + number + burstCount;
		}

		float burstIncrement = spriteTemplate_->useLegacySpawningBehavior_ ? (burstCount > 0.0f) ? (1.f / burstCount) : 0.0f : 0.0f;
		float burstStartTime = spriteTemplate_->useLegacySpawningBehavior_ ? deltaTime * burstIncrement : 0.0f;

		if (newCount >= maxActiveParticles_)
		{
			if (deltaTime < peakActiveParticleUpdateDelta_)
			{
				processSpawn = Resize(newCount + FloorToInt(Sqrt(Sqrt((float)newCount)) + 1));
			}
			else
			{
				processSpawn = Resize((newCount + FloorToInt(Sqrt(Sqrt((float)newCount)) + 1)), false);
			}
		}

		if (processSpawn == true)
		{
			const Vector3 initialLocation = emitterToSimulation_.Translation();

			// Spawn particles.
			SpawnParticles(number, startTime, increment, initialLocation, Vector3::ZERO, nullptr);

			// Burst particles.
			SpawnParticles(burstCount, burstStartTime, burstIncrement, initialLocation, Vector3::ZERO, nullptr);

			return newLeftover;
		}
		return safetyLeftover;
	}

	return spawnFraction_;
}

static Int32 FindAndSetFirstZeroIndex(PODVector<UInt8>& usedIndices)
{
	for (Int32 i = 0; i < usedIndices.Size(); ++i)
	{
		if (!usedIndices[i])
		{
			usedIndices[i] = 1;
			return i;
		}
	}
	return INDEX_NONE;
}

/**
* Fixup particle indices to only have valid entries.
*/
void ParticleEmitterInstance::FixupParticleIndices()
{
	// Something is wrong and particle data are be invalid. Try to fix-up things.
	PODVector<UInt8> usedIndices(maxActiveParticles_, 0);

	for (Int32 i = 0; i < activeParticles_; ++i)
	{
		const UInt16 usedIndex = particleIndices_[i];
		if (usedIndex < maxActiveParticles_ && usedIndices[usedIndex] == 0)
		{
			usedIndices[usedIndex] = 1;
		}
		else
		{
			if (i != activeParticles_ - 1)
			{
				// Remove this bad or duplicated index
				particleIndices_[i] = particleIndices_[activeParticles_ - 1];
			}
			// Decrease particle count.
			--activeParticles_;

			// Retry the new index.
			--i;
		}
	}

	for (Int32 i = usedIndices.Size() - 1; i >= 0; --i)
	{
		if (usedIndices[i])
		{
			usedIndices.Resize(i + 1);
			break;
		}
	}

	for (Int32 i = activeParticles_; i < maxActiveParticles_; ++i)
	{
		const Int32 freeIndex = FindAndSetFirstZeroIndex(usedIndices);
		if (CRY_ENSURE(freeIndex != INDEX_NONE))
		{
			particleIndices_[i] = (UInt16)freeIndex;
		}
		else // Can't really handle that.
		{
			particleIndices_[i] = (UInt16)i;
		}
	}
}
/**
 * Spawn the indicated number of particles.
 *
 * @param count The number of particles to spawn.
 * @param startTime			The local emitter time at which to begin spawning particles.
 * @param increment			The time delta between spawned particles.
 * @param initialLocation	The initial location of spawned particles.
 * @param initialVelocity	The initial velocity of spawned particles.
 * @param EventPayload		Event generator payload if events should be triggered.
 */
void ParticleEmitterInstance::SpawnParticles(Int32 count, float startTime, float increment, const Vector3& initialLocation, const Vector3& initialVelocity, ParticleEventInstancePayload* eventPayload)
{
	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();

	ASSERT(activeParticles_ <= maxActiveParticles_);
	ASSERT(eventPayload == NULL);

	// Ensure we don't access particle beyond what is allocated.
	ASSERT(activeParticles_ + count <= maxActiveParticles_);
	count = Min<Int32>(count, maxActiveParticles_ - activeParticles_);

	auto SpawnInternal = [&](bool legacySpawnBehavior)
	{
		ParticleLODLevel* highestLODLevel = spriteTemplate_->LODLevels_[0];
		float spawnTime = startTime;
		float interp = 1.0f;
		const float interpIncrement = (count > 0 && increment > 0.0f) ? (1.0f / (float)count) : 0.0f;
		for (Int32 i = 0; i < count; i++)
		{
			// Workaround to released data.
			if (!CRY_ENSURE(particleData_ && particleIndices_))
			{
				static bool errorReported = false;
				if (!errorReported)
				{
					FLAGGG_LOG_ERROR("Detected null particles. Template : %s. ", component_ && component_->template_ ? component_->template_->GetName().CString() : "UNKNOWN");
					errorReported = true;
				}
				activeParticles_ = 0;
				count = 0;
				continue;
			}

			// Workaround to corrupted indices.
			UInt16 nextFreeIndex = particleIndices_[activeParticles_];
			if (!CRY_ENSURE(nextFreeIndex < maxActiveParticles_))
			{
				FLAGGG_LOG_ERROR("Detected corrupted particle indices. Template : %s.", component_ && component_->template_ ? component_->template_->GetName().CString() : "UNKNOWN");
				FixupParticleIndices();
				nextFreeIndex = particleIndices_[activeParticles_];
			}

			DECLARE_PARTICLE_PTR(particle, particleData_ + particleStride_ * nextFreeIndex);
			const UInt32 currentParticleIndex = activeParticles_++;

			if (legacySpawnBehavior)
			{
				startTime -= increment;
				interp -= interpIncrement;
			}

			PreSpawn(particle, initialLocation, initialVelocity);
			for (Int32 moduleIndex = 0; moduleIndex < LODLevel->spawnModules_.Size(); moduleIndex++)
			{
				ParticleModule* spawnModule = LODLevel->spawnModules_[moduleIndex];
				if (spawnModule->enabled_)
				{
					ParticleModule* OffsetModule = highestLODLevel->spawnModules_[moduleIndex];
					spawnModule->Spawn(this, GetModuleDataOffset(OffsetModule), spawnTime, particle);

					ASSERT("NaN in particle Location. Template: %s.", component_ && component_->template_ ? component_->template_->GetName().CString() : "UNKNOWN");
				}
			}
			PostSpawn(particle, interp, spawnTime);

			// Spawn modules may set a relative time greater than 1.0f to indicate that a particle should not be spawned. We kill these particles.
			if (particle->relativeTime_ > 1.0f)
			{
				KillParticle(currentParticleIndex);

				// Process next particle
				continue;
			}

			if (!legacySpawnBehavior)
			{
				spawnTime -= increment;
				interp -= interpIncrement;
			}
		}
	};

	if (spriteTemplate_->useLegacySpawningBehavior_)
	{
		SpawnInternal(true);
	}
	else
	{
		SpawnInternal(false);
	}
}

ParticleLODLevel* ParticleEmitterInstance::GetCurrentLODLevelChecked()
{
	ASSERT(spriteTemplate_ != NULL);
	ParticleLODLevel* LODLevel = spriteTemplate_->GetCurrentLODLevel(this);
	ASSERT(LODLevel != NULL);
	ASSERT(LODLevel->requiredModule_ != NULL);
	return LODLevel;
}

Material* ParticleEmitterInstance::GetCurrentMaterial()
{
	Material* renderMaterial = currentMaterial_;
	if (renderMaterial == NULL)
	{
		// TODO: get default material
		// renderMaterial = Material::GetDefaultMaterial();
	}
	currentMaterial_ = renderMaterial;
	return renderMaterial;
}

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
void ParticleEmitterInstance::ForceSpawn(float deltaTime, Int32 inSpawnCount, Int32 inBurstCount,
	Vector3& inLocation, Vector3& inVelocity)
{
	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();

	// For beams, we probably want to ignore the SpawnRate distribution,
	// and focus strictly on the BurstList...
	Int32 spawnCount = inSpawnCount;
	Int32 burstCount = inBurstCount;
	float spawnRateDivisor = 0.0f;
	float oldLeftover = 0.0f;

	ParticleLODLevel* highestLODLevel = spriteTemplate_->LODLevels_[0];

	bool processSpawnRate = true;
	bool processBurstList = true;

	// Spawn new particles...
	if ((spawnCount > 0) || (burstCount > 0))
	{
		Int32 number = spawnCount;
		float increment = (spawnCount > 0) ? (deltaTime / spawnCount) : 0;
		float startTime = deltaTime;

		// Handle growing arrays.
		bool processSpawn = true;
		Int32 newCount = activeParticles_ + number + burstCount;
		if (newCount >= maxActiveParticles_)
		{
			if (deltaTime < peakActiveParticleUpdateDelta_)
			{
				processSpawn = Resize(newCount + FloorToInt(Sqrt(Sqrt((float)newCount)) + 1));
			}
			else
			{
				processSpawn = Resize((newCount + FloorToInt(Sqrt(Sqrt((float)newCount)) + 1)), false);
			}
		}

		if (processSpawn == true)
		{
			// This logic matches the existing behavior. However, I think the
			// interface for ForceSpawn should treat these values as being in
			// world space and transform them to emitter local space if necessary.
			const bool useLocalSpace = LODLevel->requiredModule_->useLocalSpace_;
			Vector3 spawnLocation = useLocalSpace ? Vector3::ZERO : inLocation;
			Vector3 spawnVelocity = useLocalSpace ? Vector3::ZERO : inVelocity;

			// Spawn particles.
			SpawnParticles(number, startTime, increment, inLocation, inVelocity, NULL /*EventPayload*/);

			// Burst particles.
			SpawnParticles(burstCount, startTime, 0.0f, inLocation, inVelocity, NULL /*EventPayload*/);
		}
	}
}

/**
 * Handle any pre-spawning actions required for particles
 *
 * @param particle			The particle being spawned.
 * @param initialLocation	The initial location of the particle.
 * @param initialVelocity	The initial velocity of the particle.
 */
void ParticleEmitterInstance::PreSpawn(BaseParticle* particle, const Vector3& initialLocation, const Vector3& initialVelocity)
{
	ASSERT(particle);
	// This isn't a problem w/ the FMemory::Memzero call - it's a problem in general!
	ASSERT(particleSize_ > 0);

	// By default, just clear out the particle
	memset(particle, 0, particleSize_);

	// Initialize the particle location.
	particle->location_ = initialLocation;
	particle->baseVelocity_ = Vector3(initialVelocity);
	particle->velocity_ = Vector3(initialVelocity);

	// New particles has already updated spawn location
	// Subtract offset here, so deferred location offset in UpdateBoundingBox will return this particle back
	particle->location_ -= positionOffsetThisTick_;
}

/**
 *	Has the instance completed it's run?
 *
 *	@return	bool	true if the instance is completed, false if not
 */
bool ParticleEmitterInstance::HasCompleted()
{
	// Validity ASSERT
	if (spriteTemplate_ == NULL)
	{
		return true;
	}

	// If it hasn't finished looping or if it loops forever, not completed.
	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();
	if ((LODLevel->requiredModule_->emitterLoops_ == 0) ||
		(secondsSinceCreation_ < (emitterDuration_ * LODLevel->requiredModule_->emitterLoops_)))
	{
		return false;
	}

	// If there are active particles, not completed
	if (activeParticles_ > 0)
	{
		return false;
	}

	return true;
}

/**
 *	Handle any post-spawning actions required by the instance
 *
 *	@param	particle					The particle that was spawned
 *	@param	InterpolationPercentage		The percentage of the time slice it was spawned at
 *	@param	SpawnTIme					The time it was spawned at
 */
void ParticleEmitterInstance::PostSpawn(BaseParticle* particle, float interpolationPercentage, float spawnTime)
{
	// Interpolate position if using world space.
	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();
	if (LODLevel->requiredModule_->useLocalSpace_ == false)
	{
		if ((oldLocation_ - location_).LengthSquared() > 1.f)
		{
			particle->location_ += interpolationPercentage * (oldLocation_ - location_);
		}
	}

	// Offset caused by any velocity
	particle->oldLocation_ = particle->location_;
	particle->location_ += spawnTime * particle->velocity_;

	// Store a sequence counter.
	particle->flags_ |= ((particleCounter_++) & STATE_CounterMask);
	particle->flags_ |= STATE_Particle_JustSpawned;
}

/**
 *	Kill off any dead particles. (Remove them from the active array)
 */
void ParticleEmitterInstance::KillParticles()
{
	if (activeParticles_ > 0)
	{
		ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();

		bool foundCorruptIndices = false;
		// Loop over the active particles... If their RelativeTime is > 1.0f (indicating they are dead),
		// move them to the 'end' of the active particle list.
		for (Int32 i = activeParticles_ - 1; i >= 0; i--)
		{
			const Int32	currentIndex = particleIndices_[i];
			if (CRY_ENSURE(currentIndex < maxActiveParticles_))
			{
				const UInt8* ParticleBase = particleData_ + currentIndex * particleStride_;
				BaseParticle& particle = *((BaseParticle*)ParticleBase);

				if (particle.relativeTime_ > 1.0f)
				{
					// Move it to the 'back' of the list
					particleIndices_[i] = particleIndices_[activeParticles_ - 1];
					particleIndices_[activeParticles_ - 1] = currentIndex;
					activeParticles_--;
				}
			}
			else
			{
				foundCorruptIndices = true;
			}
		}

		if (foundCorruptIndices)
		{
			FLAGGG_LOG_ERROR("Detected corrupted particle indices. Template : %s, component_ %s", component_ && component_->template_ ? component_->template_->GetName().CString() : "UNKNOWN");
			FixupParticleIndices();
		}
	}
}

/**
 *	Kill the particle at the given instance
 *
 *	@param	index		The index of the particle to kill.
 */
void ParticleEmitterInstance::KillParticle(Int32 index)
{
	if (index < activeParticles_)
	{
		ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();

		Int32 KillIndex = particleIndices_[index];

		// Move it to the 'back' of the list
		for (Int32 i = index; i < activeParticles_ - 1; i++)
		{
			particleIndices_[i] = particleIndices_[i + 1];
		}
		particleIndices_[activeParticles_ - 1] = KillIndex;
		activeParticles_--;
	}
}

void ParticleEmitterInstance::FakeBursts()
{
	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();
	if (LODLevel->spawnModule_->burstList_.Size() > 0)
	{
		// For each burst in the list
		for (Int32 burstIdx = 0; burstIdx < LODLevel->spawnModule_->burstList_.Size(); burstIdx++)
		{
			ParticleBurst* burstEntry = &(LODLevel->spawnModule_->burstList_[burstIdx]);
			// If it hasn't been fired
			if (LODLevel->level_ < burstFired_.Size())
			{
				LODBurstFired& LocalBurstFired = burstFired_[LODLevel->level_];
				if (burstIdx < LocalBurstFired.fired_.Size())
				{
					if (emitterTime_ >= burstEntry->time_)
					{
						LocalBurstFired.fired_[burstIdx] = true;
					}
				}
			}
		}
	}
}

/**
 *	This is used to force "kill" particles irrespective of their duration.
 *	Basically, this takes all particles and moves them to the 'end' of the
 *	particle list so we can insta kill off trailed particles in the level.
 */
void ParticleEmitterInstance::KillParticlesForced(bool fireEvents)
{
	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();

	// Loop over the active particles and kill them.
	// Move them to the 'end' of the active particle list.
	for (Int32 KillIdx = activeParticles_ - 1; KillIdx >= 0; KillIdx--)
	{
		const Int32 CurrentIndex = particleIndices_[KillIdx];
		particleIndices_[KillIdx] = particleIndices_[activeParticles_ - 1];
		particleIndices_[activeParticles_ - 1] = CurrentIndex;
		activeParticles_--;
	}

	particleCounter_ = 0;
}

/**
 *	Retrieve the particle at the given index
 *
 *	@param	index			The index of the particle of interest
 *
 *	@return	BaseParticle*	The pointer to the particle. NULL if not present/active
 */
BaseParticle* ParticleEmitterInstance::GetParticle(Int32 index)
{
	// See if the index is valid. If not, return NULL
	if ((index >= activeParticles_) || (index < 0))
	{
		return NULL;
	}

	// Grab and return the particle
	DECLARE_PARTICLE_PTR(particle, particleData_ + particleStride_ * particleIndices_[index]);
	return particle;
}

BaseParticle* ParticleEmitterInstance::GetParticleDirect(Int32 inDirectIndex)
{
	if ((activeParticles_ > 0) && (inDirectIndex < maxActiveParticles_))
	{
		DECLARE_PARTICLE_PTR(particle, particleData_ + particleStride_ * inDirectIndex);
		return particle;
	}
	return NULL;
}

/**
 *	Calculates the emitter duration for the instance.
 */
void ParticleEmitterInstance::SetupEmitterDuration()
{
	// Validity ASSERT
	if (spriteTemplate_ == NULL)
	{
		return;
	}

	// Set up the array for each LOD level
	Int32 EDCount = emitterDurations_.Size();
	if ((EDCount == 0) || (EDCount != spriteTemplate_->LODLevels_.Size()))
	{
		emitterDurations_.Clear();
		emitterDurations_.Resize(spriteTemplate_->LODLevels_.Size());
	}

	// Calculate the duration for each LOD level
	for (Int32 LODIndex = 0; LODIndex < spriteTemplate_->LODLevels_.Size(); LODIndex++)
	{
		ParticleLODLevel* TempLOD = spriteTemplate_->LODLevels_[LODIndex];
		ParticleModuleRequired* requiredModule = TempLOD->requiredModule_;

		RandomStream& randomStream = requiredModule->GetRandomStream(this);

		currentDelay_ = requiredModule->emitterDelay_ + component_->emitterDelay_;
		if (requiredModule->emitterDelayUseRange_)
		{
			const float	Rand = randomStream.FRand();
			currentDelay_ = requiredModule->emitterDelayLow_ +
				((requiredModule->emitterDelay_ - requiredModule->emitterDelayLow_) * Rand) + component_->emitterDelay_;
		}


		if (requiredModule->emitterDurationUseRange_)
		{
			const float	Rand = randomStream.FRand();
			const float	Duration = requiredModule->emitterDurationLow_ +
				((requiredModule->emitterDuration_ - requiredModule->emitterDurationLow_) * Rand);
			emitterDurations_[TempLOD->level_] = Duration + currentDelay_;
		}
		else
		{
			emitterDurations_[TempLOD->level_] = requiredModule->emitterDuration_ + currentDelay_;
		}

		if ((loopCount_ == 1) && (requiredModule->delayFirstLoopOnly_ == true) &&
			((requiredModule->emitterLoops_ == 0) || (requiredModule->emitterLoops_ > 1)))
		{
			emitterDurations_[TempLOD->level_] -= currentDelay_;
		}
	}

	// Set the current duration
	emitterDuration_ = emitterDurations_[currentLODLevelIndex_];
}

/**
 *	Checks some common values for GetDynamicData validity
 *
 *	@return	bool		true if GetDynamicData should continue, false if it should return NULL
 */
bool ParticleEmitterInstance::IsDynamicDataRequired(ParticleLODLevel* inCurrentLODLevel)
{
	if ((activeParticles_ <= 0) ||
		(spriteTemplate_ && (spriteTemplate_->emitterRenderMode_ == ERM_None)))
	{
		return false;
	}

	if ((inCurrentLODLevel == NULL) || (inCurrentLODLevel->enabled_ == false) ||
		((inCurrentLODLevel->requiredModule_->useMaxDrawCount_ == true) && (inCurrentLODLevel->requiredModule_->maxDrawCount_ == 0)))
	{
		return false;
	}

	if (component_ == NULL)
	{
		return false;
	}
	return true;
}

/**
 *	Process received events.
 */
void ParticleEmitterInstance::ProcessParticleEvents(float deltaTime, bool suppressSpawning)
{

}


// Called on world origin changes
void ParticleEmitterInstance::ApplyWorldOffset(Vector3 inOffset, bool worldShift)
{
	UpdateTransforms();

	location_ += inOffset;
	oldLocation_ += inOffset;

	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();
	if (!LODLevel->requiredModule_->useLocalSpace_)
	{
		positionOffsetThisTick_ = inOffset;
	}
}

void ParticleEmitterInstance::Tick_MaterialOverrides(Int32 EmitterIndex)
{
	ParticleLODLevel* LODLevel = spriteTemplate_->GetCurrentLODLevel(this);
	bool overridden = false;
	if (LODLevel && LODLevel->requiredModule_ && component_ && component_->template_)
	{
		Vector<String>& namedOverrides = LODLevel->requiredModule_->namedMaterialOverrides_;
		Vector<NamedEmitterMaterial>& slots = component_->template_->namedMaterialSlots_;
		Vector<SharedPtr<Material>>& emitterMaterials = component_->emitterMaterials_;
		if (namedOverrides.Size() > 0)
		{
			//If we have named material overrides then get it's index into the emitter materials array.
			//Only ASSERT for [0] in in the named overrides as most emitter types only need one material. Mesh emitters might use more but they override this function.		
			for (Int32 checkIdx = 0; checkIdx < slots.Size(); ++checkIdx)
			{
				if (namedOverrides[0] == slots[checkIdx].name_)
				{
					//Default to the default material for that slot.
					currentMaterial_ = slots[checkIdx].material_;
					if (emitterMaterials.IsValidIndex(checkIdx) && nullptr != emitterMaterials[checkIdx])
					{
						//This material has been overridden externally, e.g. from a BP so use that one.
						currentMaterial_ = emitterMaterials[checkIdx];
					}

					overridden = true;
					break;
				}
			}
		}
	}

	if (overridden == false && component_)
	{
		if (component_->emitterMaterials_.IsValidIndex(EmitterIndex))
		{
			if (component_->emitterMaterials_[EmitterIndex])
			{
				currentMaterial_ = component_->emitterMaterials_[EmitterIndex];
			}
		}
	}
}

bool ParticleEmitterInstance::UseLocalSpace()
{
	const ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();
	return LODLevel->requiredModule_->useLocalSpace_;
}

void ParticleEmitterInstance::GetScreenAlignmentAndScale(Int32& outScreenAlign, Vector3& outScale)
{
	const ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();
	outScreenAlign = (Int32)LODLevel->requiredModule_->screenAlignment_;

	outScale = Vector3(1.0f, 1.0f, 1.0f);
	if (component_)
	{
		outScale = component_->GetNode()->GetWorldScale();
	}
}


/*-----------------------------------------------------------------------------
	ParticleSpriteEmitterInstance
-----------------------------------------------------------------------------*/
/**
 *	ParticleSpriteEmitterInstance
 *	The structure for a standard sprite emitter instance.
 */
 /** Constructor	*/
ParticleSpriteEmitterInstance::ParticleSpriteEmitterInstance()
{
}

/** Destructor	*/
ParticleSpriteEmitterInstance::~ParticleSpriteEmitterInstance()
{
}

/**
 *	Retrieve the allocated size of this instance.
 *
 *	@param	OutNum			The size of this instance
 *	@param	OutMax			The maximum size of this instance
 */
void ParticleSpriteEmitterInstance::GetAllocatedSize(Int32& outNum, Int32& outMax)
{
	Int32 size = sizeof(ParticleSpriteEmitterInstance);
	Int32 activeParticleDataSize = (particleData_ != NULL) ? (activeParticles_ * particleStride_) : 0;
	Int32 maxActiveParticleDataSize = (particleData_ != NULL) ? (maxActiveParticles_ * particleStride_) : 0;
	Int32 activeParticleIndexSize = (particleIndices_ != NULL) ? (activeParticles_ * sizeof(UInt16)) : 0;
	Int32 maxActiveParticleIndexSize = (particleIndices_ != NULL) ? (maxActiveParticles_ * sizeof(UInt16)) : 0;

	outNum = activeParticleDataSize + activeParticleIndexSize + size;
	outMax = maxActiveParticleDataSize + maxActiveParticleIndexSize + size;
}

void ParticleSpriteEmitterInstance::RenderUpdate(const RenderPiplineContext* renderPiplineContext, ParticleMeshDataBuilder* particleMeshDataBuilder)
{
	if (!spriteTemplate_)
		return;

	auto* currentLODLevel = spriteTemplate_->GetCurrentLODLevel(this);

	if (!currentLODLevel)
		return;

	const Matrix3x4& localToWorld = component_->GetNode()->GetWorldTransform();

	// Put the camera origin in the appropriate coordinate space.
	Vector3 cameraPosition = renderPiplineContext->camera_->GetNode()->GetWorldPosition();
	if (currentLODLevel->requiredModule_->useLocalSpace_)
	{
		Matrix3x4 invSelf = localToWorld.Inverse();
		cameraPosition = invSelf * cameraPosition;
	}

	Vector4 dynamicParameterValue(Vector4::ONE);
	Vector3 particlePosition;
	Vector3 particleOldPosition;
	float subImageIndex = 0.0f;

	ASSERT(renderContext_);
	if (!renderContext_->vertexDesc_)
	{
		geometry_ = new Geometry();

		renderContext_->geometryType_ = GEOMETRY_BILLBOARD;
		renderContext_->geometry_ = geometry_;
		renderContext_->material_ = GetCurrentMaterial();

		PODVector<VertexElement> vertexElements;
		vertexElements.Push(VertexElement(VE_VECTOR4, SEM_POSITION, 0));
		vertexElements.Push(VertexElement(VE_VECTOR4, SEM_TEXCOORD, 0));
		vertexElements.Push(VertexElement(VE_VECTOR4, SEM_TEXCOORD, 1));
		vertexElements.Push(VertexElement(VE_VECTOR4, SEM_COLOR));
		renderContext_->vertexDesc_ = GetSubsystem<VertexDescFactory>()->Create(vertexElements);
	}

	VertexDescription* vertexDesc = renderContext_->vertexDesc_;
	ParticleMeshDataBuilder::ParticleMeshData particleMeshData = particleMeshDataBuilder->Allocate(vertexDesc->GetStrideSize() * activeParticles_, activeParticles_ * 6);
	GlobalDynamicVertexBuffer::Allocation& vertexAllocation = particleMeshData.vertexAllocation_;
	GlobalDynamicIndexBuffer::Allocation& indexAllocation = particleMeshData.indexAllocation_;

	geometry_->SetVertexBuffer(0, vertexAllocation.vertexBuffer_);
	geometry_->SetIndexBuffer(indexAllocation.indexBuffer_);
	geometry_->SetDataRange(indexAllocation.offsetInCount_, indexAllocation.sizeInCount_, 0, vertexAllocation.sizeInBytes_, vertexAllocation.offsetInBytes_);

	ASSERT_MESSAGE(sizeof(ParticleSpriteVertex) == vertexDesc->GetStrideSize(), "Particle stride error.");
	ParticleSpriteVertex* fillVertex = (ParticleSpriteVertex*)vertexAllocation.vertexData_;
	UInt32* fillIndex = indexAllocation.indexData_;
	UInt32 idx = 0;

	for (UInt32 i = 0; i < activeParticles_; ++i)	
	{
		DECLARE_PARTICLE_CONST(particle, particleData_ + particleStride_ * particleIndices_[i]);
		if (i + 1 < activeParticles_)
		{
			DECLARE_PARTICLE_CONST(nextParticle, particleData_ + particleStride_ * particleIndices_[i + 1]);
			// PlatformMisc::Prefetch(&nextParticle);
		}

		const Vector2 size = ParticleMeshDataBuilder::GetParticleSize(particle, currentLODLevel->requiredModule_);

		particlePosition = particle.location_;
		particleOldPosition = particle.oldLocation_;
		ParticleMeshDataBuilder::ApplyOrbitToPosition(orbitModuleOffset_, particle, currentLODLevel->requiredModule_, localToWorld, particlePosition, particleOldPosition);

		if (cameraPayloadOffset_ != 0)
		{
			Vector3 cameraOffset = ParticleMeshDataBuilder::GetCameraOffsetFromPayload(cameraPayloadOffset_, particle, particlePosition, cameraPosition);
			particlePosition += cameraOffset;
			particleOldPosition += cameraOffset;
		}

		if (subUVDataOffset_ > 0)
		{
			FullSubUVPayload* subUVPayload = (FullSubUVPayload*)(((UInt8*)&particle) + subUVDataOffset_);
			subImageIndex = subUVPayload->imageIndex_;
		}

		if (dynamicParameterDataOffset_ > 0)
		{
			ParticleMeshDataBuilder::GetDynamicValueFromPayload(dynamicParameterDataOffset_, particle, dynamicParameterValue);
		}

		fillVertex->position_		= particlePosition;
		fillVertex->relativeTime_	= particle.relativeTime_;
		fillVertex->oldPosition_	= particleOldPosition;
		fillVertex->particleId_		= (particle.flags_ & STATE_CounterMask) / 10000.0f;
		fillVertex->size_			= GetParticleSizeWithUVFlipInSign(particle, size);
		fillVertex->rotation_		= particle.rotation_;
		fillVertex->subImageIndex_	= subImageIndex;
		fillVertex->color_			= particle.color_;
		
		++fillVertex;

		(*fillIndex) = idx++;
		++fillIndex;
		(*fillIndex) = idx++;
		++fillIndex;
		(*fillIndex) = idx++;
		++fillIndex;
		(*fillIndex) = idx++;
		++fillIndex;
		(*fillIndex) = idx++;
		++fillIndex;
		(*fillIndex) = idx++;
		++fillIndex;
	}
}

/*-----------------------------------------------------------------------------
	ParticleMeshEmitterInstance
-----------------------------------------------------------------------------*/
/**
 *	Structure for mesh emitter instances
 */

 /** Constructor	*/
ParticleMeshEmitterInstance::ParticleMeshEmitterInstance()
	: ParticleEmitterInstance()
	, meshTypeData_(NULL)
	, meshRotationActive_(false)
	, meshRotationOffset_(0)
	, meshMotionBlurOffset_(0)
{
}


void ParticleMeshEmitterInstance::InitParameters(ParticleEmitter* InTemplate, ParticleSystemComponent* InComponent)
{
	ParticleEmitterInstance::InitParameters(InTemplate, InComponent);

	// Get the type data module
	ParticleLODLevel* LODLevel = InTemplate->GetLODLevel(0);
	ASSERT(LODLevel);
	meshTypeData_ = RTTICast<ParticleModuleTypeDataMesh>(LODLevel->typeDataModule_);
	ASSERT(meshTypeData_);

	// Grab cached mesh rotation flag from ParticleEmitter template
	meshRotationActive_ = InTemplate->meshRotationActive_;
}

/**
 *	Initialize the instance
 */
void ParticleMeshEmitterInstance::Init()
{
	ParticleEmitterInstance::Init();
}

/**
 *	Resize the particle data array
 *
 *	@param	NewMaxActiveParticles	The new size to use
 *
 *	@return	bool					true if the resize was successful
 */
bool ParticleMeshEmitterInstance::Resize(Int32 newMaxActiveParticles, bool setMaxActiveCount)
{
	Int32 OldMaxActiveParticles = maxActiveParticles_;
	if (ParticleEmitterInstance::Resize(newMaxActiveParticles, setMaxActiveCount) == true)
	{
		if (meshRotationActive_)
		{
			for (Int32 i = OldMaxActiveParticles; i < newMaxActiveParticles; i++)
			{
				DECLARE_PARTICLE(particle, particleData_ + particleStride_ * particleIndices_[i]);
				MeshRotationPayloadData* payloadData = (MeshRotationPayloadData*)((UInt8*)&particle + meshRotationOffset_);
				payloadData->rotationRateBase_ = Vector3::ZERO;
			}
		}

		return true;
	}

	return false;
}

/**
 *	Tick the instance.
 *
 *	@param	deltaTime			The time slice to use
 *	@param	bSuppressSpawning	If true, do not spawn during Tick
 */
void ParticleMeshEmitterInstance::Tick(float deltaTime, bool suppressSpawning)
{
	if (enabled_ && meshMotionBlurOffset_)
	{
		for (Int32 i = 0; i < activeParticles_; i++)
		{
			DECLARE_PARTICLE(particle, particleData_ + particleStride_ * particleIndices_[i]);

			MeshRotationPayloadData* rotationPayloadData = (MeshRotationPayloadData*)((UInt8*)&particle + meshRotationOffset_);
			MeshMotionBlurPayloadData* motionBlurPayloadData = (MeshMotionBlurPayloadData*)((UInt8*)&particle + meshMotionBlurOffset_);

			motionBlurPayloadData->baseParticlePrevRotation_ = particle.rotation_;
			motionBlurPayloadData->baseParticlePrevVelocity_ = particle.velocity_;
			motionBlurPayloadData->baseParticlePrevSize_ = particle.size_;
			motionBlurPayloadData->payloadPrevRotation_ = rotationPayloadData->rotation_;

			if (cameraPayloadOffset_)
			{
				const CameraOffsetParticlePayload* CameraPayload = (const CameraOffsetParticlePayload*)((const UInt8*)&particle + cameraPayloadOffset_);
				motionBlurPayloadData->payloadPrevCameraOffset_ = CameraPayload->offset_;
			}
			else
			{
				motionBlurPayloadData->payloadPrevCameraOffset_ = 0.0f;
			}

			if (orbitModuleOffset_)
			{
				const OrbitChainModuleInstancePayload* orbitPayload = (const OrbitChainModuleInstancePayload*)((const UInt8*)&particle + orbitModuleOffset_);
				motionBlurPayloadData->payloadPrevOrbitOffset_ = orbitPayload->offset_;
			}
			else
			{
				motionBlurPayloadData->payloadPrevOrbitOffset_ = Vector3::ZERO;
			}
		}
	}

	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();
	// See if we are handling mesh rotation
	if (meshRotationActive_ && enabled_)
	{
		// Update the rotation for each particle
		for (Int32 i = 0; i < activeParticles_; i++)
		{
			DECLARE_PARTICLE(particle, particleData_ + particleStride_ * particleIndices_[i]);
			MeshRotationPayloadData* payloadData = (MeshRotationPayloadData*)((UInt8*)&particle + meshRotationOffset_);
			payloadData->rotationRate_ = payloadData->rotationRateBase_;
			if (LODLevel->requiredModule_->screenAlignment_ == PSA_Velocity
				|| LODLevel->requiredModule_->screenAlignment_ == PSA_AwayFromCenter)
			{
				// Determine the rotation to the velocity vector and apply it to the mesh
				Vector3	newDirection = particle.velocity_;

				if (LODLevel->requiredModule_->screenAlignment_ == PSA_Velocity)
				{
					//ASSERT if an orbit module should affect the velocity...		
					if (LODLevel->requiredModule_->orbitModuleAffectsVelocityAlignment_ &&
						LODLevel->orbitModules_.Size() > 0)
					{
						ParticleModuleOrbit* LastOrbit = spriteTemplate_->LODLevels_[0]->orbitModules_[LODLevel->orbitModules_.Size() - 1];
						ASSERT(LastOrbit);

						UInt32 SpriteOrbitModuleOffset = *spriteTemplate_->moduleOffsetMap_.TryGetValue(LastOrbit);
						if (SpriteOrbitModuleOffset != 0)
						{
							const OrbitChainModuleInstancePayload& orbitPayload = *(OrbitChainModuleInstancePayload*)((UInt8*)&particle + SpriteOrbitModuleOffset);

							//this should be our current position
							const Vector3 newPos = particle.location_ + orbitPayload.offset_;
							//this should be our previous position
							const Vector3 oldPos =particle.oldLocation_ + orbitPayload.previousOffset_;

							newDirection = newPos - oldPos;
						}
					}
				}
				else if (LODLevel->requiredModule_->screenAlignment_ == PSA_AwayFromCenter)
				{
					newDirection = particle.location_;
				}

				newDirection.Normalize();
				Vector3	oldDirection(1.0f, 0.0f, 0.0f);

				Quaternion rotation(oldDirection, newDirection);
				Vector3 Euler(rotation.EulerAngles());
				payloadData->rotation_ = payloadData->initRotation_ + Euler;
				payloadData->rotation_ += payloadData->curContinuousRotation_;
			}
			else // not PSA_Velocity or PSA_AwayfromCenter, so rotation is not reset every tick
			{
				if ((particle.flags_ & STATE_Particle_FreezeRotation) == 0)
				{
					payloadData->rotation_ = payloadData->initRotation_ + payloadData->curContinuousRotation_;
				}
			}
		}
	}


	// Call the standard tick
	ParticleEmitterInstance::Tick(deltaTime, suppressSpawning);

	if (meshRotationActive_ && enabled_)
	{
		//Must do this (at least) after module update other wise the reset value of RotationRate is used.
		//Probably the other stuff before the module tick should be brought down here too and just leave the RotationRate reset before.
		//Though for the sake of not breaking existing behavior, leave things as they are for now.
		for (Int32 i = 0; i < activeParticles_; i++)
		{
			DECLARE_PARTICLE(particle, particleData_ + particleStride_ * particleIndices_[i]);
			MeshRotationPayloadData* payloadData = (MeshRotationPayloadData*)((UInt8*)&particle + meshRotationOffset_);
			payloadData->curContinuousRotation_ += deltaTime * payloadData->rotationRate_;
		}
	}
}

void ParticleMeshEmitterInstance::Tick_MaterialOverrides(Int32 emitterIndex)
{
	// we need to do this here, since CurrentMaterials is unique to mesh emitters, so this can't be done from the component. CurrentMaterials 
	// may end up in MeshMaterials, so if this doesn't get updated, rendering may access a garbage collected material

	// make sure currentmaterials are all set to the emitter material, so we don't end up with GC'd material pointers
	// in MeshMaterials when GetMeshMaterials pushes CurrentMaterials to MeshMaterials
	if (component_ && component_->emitterMaterials_.IsValidIndex(emitterIndex))
	{
		if (component_->emitterMaterials_[emitterIndex])
		{
			for (Material*& curMat : currentMaterials_)
			{
				curMat = component_->emitterMaterials_[emitterIndex];
			}
		}
	}

	ParticleLODLevel* LODLevel = spriteTemplate_->GetCurrentLODLevel(this);
	bool overridden = false;
	if (LODLevel && LODLevel->requiredModule_ && component_ && component_->template_)
	{
		Vector<String>& namedOverrides = LODLevel->requiredModule_->namedMaterialOverrides_;
		Vector<NamedEmitterMaterial>& slots = component_->template_->namedMaterialSlots_;
		Vector<SharedPtr<Material>>& emitterMaterials = component_->emitterMaterials_;
		if (namedOverrides.Size() > 0)
		{
			currentMaterials_.Resize(namedOverrides.Size());
			for (Int32 materialIdx = 0; materialIdx < namedOverrides.Size(); ++materialIdx)
			{
				//If we have named material overrides then get it's index into the emitter materials array.	
				for (Int32 checkIdx = 0; checkIdx < slots.Size(); ++checkIdx)
				{
					if (namedOverrides[materialIdx] == slots[checkIdx].name_)
					{
						//Default to the default material for that slot.
						currentMaterials_[materialIdx] = slots[checkIdx].material_;
						if (emitterMaterials.IsValidIndex(checkIdx) && nullptr != emitterMaterials[checkIdx])
						{
							//This material has been overridden externally, e.g. from a BP so use that one.
							currentMaterials_[materialIdx] = emitterMaterials[checkIdx];
						}

						overridden = true;
						break;
					}
				}
			}
		}
	}
}

/**
 *	Update the bounding box for the emitter
 *
 *	@param	deltaTime		The time slice to use
 */
void ParticleMeshEmitterInstance::UpdateBoundingBox(float deltaTime)
{
	//@todo. Implement proper bound determination for mesh emitters.
	// Currently, just 'forcing' the mesh size to be taken into account.
	if ((component_ != NULL) && (activeParticles_ > 0))
	{
		bool updateBox = ((component_->warmingUp_ == false) &&
			(component_->template_ != NULL) && (component_->template_->useFixedRelativeBoundingBox_ == false));

		// Take scale into account
		Vector3 scale = component_->GetNode()->GetWorldScale();

		// Get the static mesh bounds
		BoundingBox meshBound;
		if (component_->warmingUp_ == false)
		{
			if (meshTypeData_->mesh_)
			{
				meshBound = meshTypeData_->mesh_->GetBoundingBox();
			}
			else
			{
				//UE_LOG(LogParticles, Log, TEXT("MeshEmitter with no mesh set?? - %s"), component_->template_ ? *(component_->template_->GetPathName()) : TEXT("??????"));
				meshBound = BoundingBox(Vector3::ZERO, Vector3::ZERO);
			}
		}
		else
		{
			// This isn't used anywhere if the bWarmingUp flag is false, but GCC doesn't like it not touched.
			meshBound = BoundingBox(Vector3::ZERO, Vector3::ZERO);
		}

		ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();

		const bool useLocalSpace = LODLevel->requiredModule_->useLocalSpace_;

		const Matrix3x4 componentToWorld = useLocalSpace
			? component_->GetNode()->GetWorldTransform()
			: Matrix3x4::IDENTITY;

		Vector3	newLocation;
		float newRotation;
		if (updateBox)
		{
			particleBoundingBox_.Clear();
		}

		// For each particle, offset the box appropriately 
		Vector3 minVal(F_INFINITY * 0.5, F_INFINITY * 0.5, F_INFINITY * 0.5);
		Vector3 maxVal(-F_INFINITY * 0.5, -F_INFINITY * 0.5, -F_INFINITY * 0.5);

		// PlatformMisc::Prefetch(particleData_, particleStride_ * particleIndices_[0]);
		// PlatformMisc::Prefetch(particleData_, (particleIndices_[0] * particleStride_) + PLATFORM_CACHE_LINE_SIZE);

		bool skipDoubleSpawnUpdate = !spriteTemplate_->useLegacySpawningBehavior_;
		for (Int32 i = 0; i < activeParticles_; i++)
		{
			DECLARE_PARTICLE(particle, particleData_ + particleStride_ * particleIndices_[i]);
			// PlatformMisc::Prefetch(particleData_, particleStride_ * particleIndices_[i + 1]);
			// PlatformMisc::Prefetch(particleData_, (particleIndices_[i + 1] * particleStride_) + PLATFORM_CACHE_LINE_SIZE);

			// Do linear integrator and update bounding box
			particle.oldLocation_ = particle.location_;

			bool justSpawned = (particle.flags_ & STATE_Particle_JustSpawned) != 0;
			particle.flags_ &= ~STATE_Particle_JustSpawned;

			//Don't update position for newly spawned particles. They already have a partial update applied during spawn.
			bool skipUpdate = justSpawned && skipDoubleSpawnUpdate;

			if ((particle.flags_ & STATE_Particle_Freeze) == 0 && !skipUpdate)
			{
				if ((particle.flags_ & STATE_Particle_FreezeTranslation) == 0)
				{
					newLocation = particle.location_ + deltaTime * (Vector3)particle.velocity_;
				}
				else
				{
					newLocation = particle.location_;
				}
				if ((particle.flags_ & STATE_Particle_FreezeRotation) == 0)
				{
					newRotation = particle.rotation_ + deltaTime * particle.rotationRate_;
				}
				else
				{
					newRotation = particle.rotation_;
				}
			}
			else
			{
				// Don't move it...
				newLocation = particle.location_;
				newRotation = particle.rotation_;
			}

			Vector3 localExtent = meshBound.Size() * (Vector3)particle.size_ * scale;

			newLocation += positionOffsetThisTick_;
			particle.oldLocation_ += positionOffsetThisTick_;

			// Do angular integrator, and wrap result to within +/- 2 PI
			particle.rotation_ = Mod(newRotation, 2.f * (float)PI);
			particle.location_ = newLocation;

			if (updateBox)
			{
				Vector3 positionForBounds = newLocation;

				if (useLocalSpace)
				{
					// Note: building the bounding box in world space as that gives tighter bounds than transforming a local space AABB into world space
					positionForBounds = componentToWorld * newLocation;
				}

				minVal[0] = Min(minVal[0], positionForBounds.x_ - localExtent.x_);
				maxVal[0] = Max(maxVal[0], positionForBounds.x_ + localExtent.x_);
				minVal[1] = Min(minVal[1], positionForBounds.y_ - localExtent.y_);
				maxVal[1] = Max(maxVal[1], positionForBounds.y_ + localExtent.y_);
				minVal[2] = Min(minVal[2], positionForBounds.z_ - localExtent.z_);
				maxVal[2] = Max(maxVal[2], positionForBounds.z_ + localExtent.z_);
			}
		}

		if (updateBox)
		{
			particleBoundingBox_ = BoundingBox(minVal, maxVal);
		}
	}
}

/**
 *	Retrieved the per-particle bytes that this emitter type requires.
 *
 *	@return	UInt32	The number of required bytes for particles in the instance
 */
UInt32 ParticleMeshEmitterInstance::RequiredBytes()
{
	UInt32 uiBytes = ParticleEmitterInstance::RequiredBytes();

	meshRotationOffset_ = payloadOffset_ + uiBytes;
	uiBytes += sizeof(MeshRotationPayloadData);

	if (meshTypeData_)
	{
		const auto* MeshTD = static_cast<const ParticleModuleTypeDataMesh*>(meshTypeData_);
		if (meshTypeData_->IsMotionBlurEnabled())
		{
			meshMotionBlurOffset_ = payloadOffset_ + uiBytes;
			uiBytes += sizeof(MeshMotionBlurPayloadData);
		}
	}

	return uiBytes;
}

/**
 *	Handle any post-spawning actions required by the instance
 *
 *	@param	particle					The particle that was spawned
 *	@param	InterpolationPercentage		The percentage of the time slice it was spawned at
 *	@param	SpawnTIme					The time it was spawned at
 */
void ParticleMeshEmitterInstance::PostSpawn(BaseParticle* particle, float interpolationPercentage, float spawnTime)
{
	ParticleEmitterInstance::PostSpawn(particle, interpolationPercentage, spawnTime);
	ParticleLODLevel* LODLevel = GetCurrentLODLevelChecked();

	MeshRotationPayloadData* payloadData = (MeshRotationPayloadData*)((UInt8*)particle + meshRotationOffset_);

	if (LODLevel->requiredModule_->screenAlignment_ == PSA_Velocity
		|| LODLevel->requiredModule_->screenAlignment_ == PSA_AwayFromCenter)
	{
		// Determine the rotation to the velocity vector and apply it to the mesh
		Vector3	newDirection(particle->velocity_);
		if (LODLevel->requiredModule_->screenAlignment_ == PSA_AwayFromCenter)
		{
			newDirection = particle->location_;
		}

		newDirection.Normalize();
		Vector3	oldDirection(1.0f, 0.0f, 0.0f);

		Quaternion rotation(oldDirection, newDirection);
		Vector3 euler = rotation.EulerAngles();

		payloadData->rotation_ += euler;
	}

	Vector3 initialOrient = meshTypeData_->rollPitchYawRange_.GetValue(spawnTime, 0, 0, &meshTypeData_->randomStream_);
	payloadData->initialOrientation_ = initialOrient;

	if (meshMotionBlurOffset_)
	{
		MeshRotationPayloadData* rotationPayloadData = (MeshRotationPayloadData*)((UInt8*)particle + meshRotationOffset_);
		MeshMotionBlurPayloadData* motionBlurPayloadData = (MeshMotionBlurPayloadData*)((UInt8*)particle + meshMotionBlurOffset_);

		motionBlurPayloadData->baseParticlePrevRotation_ = particle->rotation_;
		motionBlurPayloadData->baseParticlePrevVelocity_ = particle->velocity_;
		motionBlurPayloadData->baseParticlePrevSize_ = particle->size_;
		motionBlurPayloadData->payloadPrevRotation_ = rotationPayloadData->rotation_;

		if (cameraPayloadOffset_)
		{
			const CameraOffsetParticlePayload* CameraPayload = (const CameraOffsetParticlePayload*)((const UInt8*)particle + cameraPayloadOffset_);
			motionBlurPayloadData->payloadPrevCameraOffset_ = CameraPayload->offset_;
		}
		else
		{
			motionBlurPayloadData->payloadPrevCameraOffset_ = 0.0f;
		}

		if (orbitModuleOffset_)
		{
			const OrbitChainModuleInstancePayload* OrbitPayload = (const OrbitChainModuleInstancePayload*)((const UInt8*)particle + orbitModuleOffset_);
			motionBlurPayloadData->payloadPrevOrbitOffset_ = OrbitPayload->offset_;
		}
		else
		{
			motionBlurPayloadData->payloadPrevOrbitOffset_ = Vector3::ZERO;
		}
	}
}

bool ParticleMeshEmitterInstance::IsDynamicDataRequired(ParticleLODLevel* InCurrentLODLevel)
{
	return /*meshTypeData_->mesh_ != NULL
		&& meshTypeData_->mesh_->HasValidRenderData(false)
		&& */ParticleEmitterInstance::IsDynamicDataRequired(InCurrentLODLevel);
}

/**
 *	Retrieve the allocated size of this instance.
 *
 *	@param	OutNum			The size of this instance
 *	@param	OutMax			The maximum size of this instance
 */
void ParticleMeshEmitterInstance::GetAllocatedSize(Int32& outNum, Int32& outMax)
{
	Int32 size = sizeof(ParticleMeshEmitterInstance);
	Int32 activeParticleDataSize = (particleData_ != NULL) ? (activeParticles_ * particleStride_) : 0;
	Int32 maxActiveParticleDataSize = (particleData_ != NULL) ? (maxActiveParticles_ * particleStride_) : 0;
	Int32 activeParticleIndexSize = (particleIndices_ != NULL) ? (activeParticles_ * sizeof(UInt16)) : 0;
	Int32 maxActiveParticleIndexSize = (particleIndices_ != NULL) ? (maxActiveParticles_ * sizeof(UInt16)) : 0;

	outNum = activeParticleDataSize + activeParticleIndexSize + size;
	outMax = maxActiveParticleDataSize + maxActiveParticleIndexSize + size;
}

/**
 * Sets the materials with which mesh particles should be rendered.
 */
void ParticleMeshEmitterInstance::SetMeshMaterials(const PODVector<Material*>& InMaterials)
{
	currentMaterials_ = InMaterials;
}

}
