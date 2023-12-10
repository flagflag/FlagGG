#include "ParticleEmitter.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleLODLevel.h"
#include "ParticleSystem/Module/ParticleModuleTypeData.h"
#include "ParticleSystem/Module/ParticleModuleParameter.h"
#include "ParticleSystem/Module/ParticleModuleLocation.h"
#include "ParticleSystem/Module/ParticleModuleLight.h"
#include "ParticleSystem/Module/ParticleModuleCamera.h"
#include "ParticleSystem/Module/ParticleModuleSubUV.h"
#include "ParticleSystem/Module/ParticleModuleMaterial.h"
#include "Math/Distributions/DistributionFloatConstant.h"
#include "Log.h"

namespace FlagGG
{

/*-----------------------------------------------------------------------------
	ParticleEmitter implementation.
-----------------------------------------------------------------------------*/
ParticleEmitter::ParticleEmitter()
	: significanceLevel_(ParticleSignificanceLevel::Critical)
	, useLegacySpawningBehavior_(false)
	, disabledLODsKeepEmitterAlive_(false)
	, disableWhenInsignficant_(0)
	, qualityLevelSpawnRateScale_(1.0f)
	, detailModeBitmask_(PDM_DefaultValue)
{
	// Structure to hold one-time initialization
	struct ConstructorStatics
	{
		String NAME_Particle_Emitter_;
		ConstructorStatics()
			: NAME_Particle_Emitter_("Particle Emitter")
		{
		}
	};
	static ConstructorStatics ConstructorStatics;

	emitterName_ = ConstructorStatics.NAME_Particle_Emitter_;
	convertedModules_ = true;
	peakActiveParticles_ = 0;
#if WITH_EDITORONLY_DATA
	emitterEditorColor_ = Color(0, 150, 150, 255);
#endif // WITH_EDITORONLY_DATA
}

ParticleEmitterInstance* ParticleEmitter::CreateInstance(ParticleSystemComponent* inComponent)
{
	FLAGGG_LOG_ERROR("ParticleEmitter::CreateInstance is pure virtual"); 
	return NULL; 
}

void ParticleEmitter::UpdateModuleLists()
{
	for (Int32 LODIndex = 0; LODIndex < LODLevels_.Size(); LODIndex++)
	{
		ParticleLODLevel* LODLevel = LODLevels_[LODIndex];
		if (LODLevel)
		{
			LODLevel->UpdateModuleLists();
		}
	}
	Build();
}

void ParticleEmitter::SetEmitterName(const String& name)
{
	emitterName_ = name;
}

const String& ParticleEmitter::GetEmitterName()
{
	return emitterName_;
}

void ParticleEmitter::SetLODCount(Int32 LODCount)
{
	// 
}

void ParticleEmitter::AddEmitterCurvesToEditor(InterpCurveEdSetup* edSetup)
{
	FLAGGG_LOG_WARN("ParticleEmitter::AddEmitterCurvesToEditor> Should no longer be called...");
	return;
}

void ParticleEmitter::RemoveEmitterCurvesFromEditor(InterpCurveEdSetup* edSetup)
{
	for (Int32 LODIndex = 0; LODIndex < LODLevels_.Size(); LODIndex++)
	{
		ParticleLODLevel* LODLevel = LODLevels_[LODIndex];
		// Remove the typedata curves...
		if (LODLevel->typeDataModule_ && LODLevel->typeDataModule_->IsDisplayedInCurveEd(edSetup))
		{
			LODLevel->typeDataModule_->RemoveModuleCurvesFromEditor(edSetup);
		}

		// Remove the spawn module curves...
		if (LODLevel->spawnModule_ && LODLevel->spawnModule_->IsDisplayedInCurveEd(edSetup))
		{
			LODLevel->spawnModule_->RemoveModuleCurvesFromEditor(edSetup);
		}

		// Remove each modules curves as well.
		for (Int32 ii = 0; ii < LODLevel->modules_.Size(); ii++)
		{
			if (LODLevel->modules_[ii]->IsDisplayedInCurveEd(edSetup))
			{
				// Remove it from the curve editor!
				LODLevel->modules_[ii]->RemoveModuleCurvesFromEditor(edSetup);
			}
		}
	}
}

void ParticleEmitter::ChangeEditorColor(Color& color, InterpCurveEdSetup* edSetup)
{
#if WITH_EDITORONLY_DATA
	ParticleLODLevel* LODLevel = LODLevels_[0];
	EmitterEditorColor = Color;
	for (Int32 TabIndex = 0; TabIndex < edSetup->Tabs.Size(); TabIndex++)
	{
		CurveEdTab*	Tab = &(edSetup->Tabs[TabIndex]);
		for (Int32 CurveIndex = 0; CurveIndex < Tab->Curves.Size(); CurveIndex++)
		{
			CurveEdEntry* Entry	= &(Tab->Curves[CurveIndex]);
			if (LODLevel->spawnModule_->Rate.Distribution == Entry->CurveObject)
			{
				Entry->CurveColor	= Color;
			}
		}
	}
#endif // WITH_EDITORONLY_DATA
}

void ParticleEmitter::AutoPopulateInstanceProperties(ParticleSystemComponent* pSysComp)
{
	for (Int32 LODIndex = 0; LODIndex < LODLevels_.Size(); LODIndex++)
	{
		ParticleLODLevel* LODLevel	= LODLevels_[LODIndex];
		for (Int32 moduleIndex = 0; moduleIndex < LODLevel->modules_.Size(); moduleIndex++)
		{
			ParticleModule* module = LODLevel->modules_[moduleIndex];
			LODLevel->spawnModule_->AutoPopulateInstanceProperties(pSysComp);
			LODLevel->requiredModule_->AutoPopulateInstanceProperties(pSysComp);
			if (LODLevel->typeDataModule_)
			{
				LODLevel->typeDataModule_->AutoPopulateInstanceProperties(pSysComp);
			}

			module->AutoPopulateInstanceProperties(pSysComp);
		}
	}
}


Int32 ParticleEmitter::CreateLODLevel(Int32 LODLevel, bool generateModuleData)
{
	Int32				levelIndex		= -1;
	ParticleLODLevel*	createdLODLevel	= NULL;

	if (LODLevels_.Size() == 0)
	{
		LODLevel = 0;
	}

	// Is the requested index outside a viable range?
	if ((LODLevel < 0) || (LODLevel > LODLevels_.Size()))
	{
		return -1;
	}

	// NextHighestLODLevel is the one that will be 'copied'
	ParticleLODLevel*	nextHighestLODLevel	= NULL;
	Int32 nextHighIndex = -1;
	// NextLowestLODLevel is the one (and all ones lower than it) that will have their LOD indices updated
	ParticleLODLevel*	nextLowestLODLevel	= NULL;
	Int32 nextLowIndex = -1;

	// Grab the two surrounding LOD levels...
	if (LODLevel == 0)
	{
		// It is being added at the front of the list... (highest)
		if (LODLevels_.Size() > 0)
		{
			nextHighestLODLevel = LODLevels_[0];
			nextHighIndex = 0;
			nextLowestLODLevel = nextHighestLODLevel;
			nextLowIndex = 0;
		}
	}
	else
	if (LODLevel > 0)
	{
		nextHighestLODLevel = LODLevels_[LODLevel - 1];
		nextHighIndex = LODLevel - 1;
		if (LODLevel < LODLevels_.Size())
		{
			nextLowestLODLevel = LODLevels_[LODLevel];
			nextLowIndex = LODLevel;
		}
	}
	
	// Update the LODLevel index for the lower levels and
	// offset the LOD validity flags for the modules...
	if (nextLowestLODLevel)
	{
		nextLowestLODLevel->ConditionalPostLoad();
		for (Int32 lowIndex = LODLevels_.Size() - 1; lowIndex >= nextLowIndex; lowIndex--)
		{
			ParticleLODLevel* lowRemapLevel = LODLevels_[lowIndex];
			if (lowRemapLevel)
			{
				lowRemapLevel->SetLevelIndex(lowIndex + 1);
			}
		}
	}

	// Create a ParticleLODLevel
	SharedPtr<ParticleLODLevel> createdLODLevel = MakeShared<ParticleLODLevel>();
	ASSERT(createdLODLevel);

	createdLODLevel->level_ = LODLevel;
	createdLODLevel->enabled_ = true;
	createdLODLevel->convertedModules_ = true;
	createdLODLevel->peakActiveParticles_ = 0;

	// Determine where to place it...
	if (LODLevels_.Size() == 0)
	{
		LODLevels_.Insert(0u, createdLODLevel);
		createdLODLevel->level_	= 0;
	}
	else
	{
		LODLevels_.Insert(LODLevel, createdLODLevel);
		createdLODLevel->level_ = LODLevel;
	}

	if (nextHighestLODLevel)
	{
		nextHighestLODLevel->ConditionalPostLoad();

		// Generate from the higher LOD level
		if (createdLODLevel->GenerateFromLODLevel(nextHighestLODLevel, 100.0, generateModuleData) == false)
		{
			FLAGGG_LOG_WARN("Failed to generate LOD level %d from level %d", LODLevel, nextHighestLODLevel->level_);
		}
	}
	else
	{
		// Create the requiredModule
		SharedPtr<ParticleModuleRequired> requiredModule = MakeShared<ParticleModuleRequired>();
		ASSERT(requiredModule);
		requiredModule->SetToSensibleDefaults(this);
		createdLODLevel->requiredModule_	= requiredModule;

		// The SpawnRate for the required module
		requiredModule->useLocalSpace_			= false;
		requiredModule->killOnDeactivate_		= false;
		requiredModule->killOnCompleted_		= false;
		requiredModule->emitterDuration_		= 1.0f;
		requiredModule->emitterLoops_			= 0;
		requiredModule->particleBurstMethod_	= EPBM_Instant;
#if WITH_EDITORONLY_DATA
		requiredModule->moduleEditorColor_		= Color::MakeRandomColor();
#endif // WITH_EDITORONLY_DATA
		requiredModule->interpolationMethod_	= PSUVIM_None;
		requiredModule->subImages_Horizontal_	= 1;
		requiredModule->subImages_Vertical_		= 1;
		requiredModule->scaleUV_				= false;
		requiredModule->randomImageTime_		= 0.0f;
		requiredModule->randomImageChanges_		= 0;
		requiredModule->enabled_				= true;

		requiredModule->LODValidity_ = (1 << LODLevel);

		// There must be a spawn module as well...
		SharedPtr<ParticleModuleSpawn> spawnModule = MakeShared<ParticleModuleSpawn>();
		ASSERT(spawnModule);
		createdLODLevel->spawnModule_ = spawnModule;
		spawnModule->LODValidity_ = (1 << LODLevel);
		DistributionFloatConstant* constantSpawn	= Cast<DistributionFloatConstant>(spawnModule->rate_.distribution_);
		constantSpawn->constant_				= 10;
		constantSpawn->isDirty_					= true;
		spawnModule->burstList_.Empty();

		// Copy the TypeData module
		createdLODLevel->typeDataModule_			= NULL;
	}

	levelIndex	= createdLODLevel->level_;

	// MarkPackageDirty();

	return levelIndex;
}


bool ParticleEmitter::IsLODLevelValid(Int32 LODLevel)
{
	for (Int32 LODIndex = 0; LODIndex < LODLevels_.Size(); LODIndex++)
	{
		ParticleLODLevel* checkLODLevel	= LODLevels_[LODIndex];
		if (checkLODLevel->level_ == LODLevel)
		{
			return true;
		}
	}

	return false;
}


void ParticleEmitter::EditorUpdateCurrentLOD(ParticleEmitterInstance* instance)
{
#if WITH_EDITORONLY_DATA
	ParticleLODLevel*	currentLODLevel	= NULL;
	ParticleLODLevel*	higher			= NULL;

	Int32 setLODLevel = -1;
	if (instance->component_ && instance->component_->template_)
	{
		Int32 desiredLODLevel = instance->component_->Template->EditorLODSetting;
		if (GIsEditor && GEngine->bEnableEditorPSysRealtimeLOD)
		{
			desiredLODLevel = instance->component_->GetCurrentLODIndex();
		}

		for (Int32 levelIndex = 0; levelIndex < LODLevels_.Size(); levelIndex++)
		{
			higher	= LODLevels_[levelIndex];
			if (higher && (higher->Level == desiredLODLevel))
			{
				setLODLevel = levelIndex;
				break;
			}
		}
	}

	if (setLODLevel == -1)
	{
		setLODLevel = 0;
	}
	instance->SetCurrentLODIndex(setLODLevel, false);
#endif // WITH_EDITORONLY_DATA
}



ParticleLODLevel* ParticleEmitter::GetLODLevel(Int32 LODLevel)
{
	if (LODLevel >= LODLevels_.Size())
	{
		return NULL;
	}

	return LODLevels_[LODLevel];
}


bool ParticleEmitter::AutogenerateLowestLODLevel(bool bDuplicateHighest)
{
	// Didn't find it?
	if (LODLevels_.Size() == 1)
	{
		// We need to generate it...
		SharedPtr<ParticleLODLevel> LODLevel = MakeShared<ParticleLODLevel>();
		ASSERT(LODLevel);
		LODLevels_.Insert(1, LODLevel);
		LODLevel->level_				= 1;
		LODLevel->convertedModules_		= true;
		LODLevel->peakActiveParticles_	= 0;

		// Grab LODLevel 0 for creation
		ParticleLODLevel* sourceLODLevel = LODLevels_[0];

		LODLevel->enabled_				= sourceLODLevel->enabled_;

		float percentage = 10.0f;
		if (sourceLODLevel->typeDataModule_)
		{
			ParticleModuleTypeDataBeam2* beam2TD = Cast<ParticleModuleTypeDataBeam2>(sourceLODLevel->typeDataModule_);

			if (beam2TD)
			{
				// For now, don't support LOD on beams and trails
				percentage	= 100.0f;
			}
		}

		if (bDuplicateHighest == true)
		{
			percentage = 100.0f;
		}

		if (LODLevel->GenerateFromLODLevel(sourceLODLevel, percentage) == false)
		{
			FLAGGG_LOG_WARN("Failed to generate LOD level %d from LOD level 0", 1);
			return false;
		}

		// MarkPackageDirty();
		return true;
	}

	return true;
}


bool ParticleEmitter::CalculateMaxActiveParticleCount()
{
	Int32 currMaxAPC = 0;

	Int32 maxCount = 0;
	
	for (Int32 LODIndex = 0; LODIndex < LODLevels_.Size(); LODIndex++)
	{
		ParticleLODLevel* LODLevel = LODLevels_[LODIndex];
		if (LODLevel && LODLevel->enabled_)
		{
			bool forceMaxCount = false;
			// Check for beams or trails
			if ((LODLevel->level_ == 0) && (LODLevel->typeDataModule_ != NULL))
			{
				ParticleModuleTypeDataBeam2* beamTD = Cast<ParticleModuleTypeDataBeam2>(LODLevel->typeDataModule_);
				if (beamTD)
				{
					forceMaxCount = true;
					maxCount = beamTD->maxBeamCount_ + 2;
				}
			}

			Int32 LODMaxAPC = LODLevel->CalculateMaxActiveParticleCount();
			if (forceMaxCount == true)
			{
				LODLevel->peakActiveParticles_ = maxCount;
				LODMaxAPC = maxCount;
			}

			if (LODMaxAPC > currMaxAPC)
			{
				if (LODIndex > 0)
				{
					// Check for a ridiculous difference in counts...
					if ((currMaxAPC > 0) && (LODMaxAPC / currMaxAPC) > 2)
					{
						//UE_LOG(LogParticles, Log, TEXT("MaxActiveParticleCount Discrepancy?\n\tLOD %2d, emitter %16s"), LODIndex, *GetName());
					}
				}
				currMaxAPC = LODMaxAPC;
			}
		}
	}

#if WITH_EDITOR
	if ((GIsEditor == true) && (currMaxAPC > 500))
	{
		//@todo. Added an option to the emitter to disable this warning - for 
		// the RARE cases where it is really required to render that many.
		FLAGGG_LOG_WARN("MaxCount = %4d for emitter %s",
			currMaxAPC, GetTypeName().CString());
	}
#endif
	return true;
}


void ParticleEmitter::GetParametersUtilized(Vector<String>& particleSysParamList, Vector<String>& particleParameterList)
{
	// Clear the lists
	particleSysParamList.Clear();
	particleParameterList.Clear();

	PODVector<ParticleModule*> processedModules;
	processedModules.Clear();

	for (Int32 LODIndex = 0; LODIndex < LODLevels_.Size(); LODIndex++)
	{
		ParticleLODLevel* LODLevel = LODLevels_[LODIndex];
		if (LODLevel)
		{
			// Grab that parameters from each module...
			ASSERT(LODLevel->requiredModule_);
			if (processedModules.Contains(LODLevel->requiredModule_) == false)
			{
				LODLevel->requiredModule_->GetParticleSysParamsUtilized(particleSysParamList);
				LODLevel->requiredModule_->GetParticleParametersUtilized(particleParameterList);
				processedModules.Push(LODLevel->requiredModule_);
			}

			ASSERT(LODLevel->spawnModule_);
			if (processedModules.Contains(LODLevel->spawnModule_) == false)
			{
				LODLevel->spawnModule_->GetParticleSysParamsUtilized(particleSysParamList);
				LODLevel->spawnModule_->GetParticleParametersUtilized(particleParameterList);
				processedModules.Push(LODLevel->spawnModule_);
			}

			if (LODLevel->typeDataModule_)
			{
				if (processedModules.Contains(LODLevel->typeDataModule_) == false)
				{
					LODLevel->typeDataModule_->GetParticleSysParamsUtilized(particleSysParamList);
					LODLevel->typeDataModule_->GetParticleParametersUtilized(particleParameterList);
					processedModules.Push(LODLevel->typeDataModule_);
				}
			}
			
			for (Int32 moduleIndex = 0; moduleIndex < LODLevel->modules_.Size(); moduleIndex++)
			{
				ParticleModule* module = LODLevel->modules_[moduleIndex];
				if (module)
				{
					if (processedModules.Contains(module) == false)
					{
						module->GetParticleSysParamsUtilized(particleSysParamList);
						module->GetParticleParametersUtilized(particleParameterList);
						processedModules.Push(module);
					}
				}
			}
		}
	}
}


void ParticleEmitter::Build()
{
	const Int32 LODCount = LODLevels_.Size();
	if (LODCount > 0)
	{
		ParticleLODLevel* highLODLevel = LODLevels_[0];
		ASSERT(highLODLevel);
		if (highLODLevel->typeDataModule_ != nullptr)
		{
			if (highLODLevel->typeDataModule_->RequiresBuild())
			{
				ParticleEmitterBuildInfo emitterBuildInfo;
#if WITH_EDITOR
				if (!GetOutermost()->bIsCookedForEditor)
				{
					highLODLevel->CompileModules(emitterBuildInfo);
				}
#endif
				highLODLevel->typeDataModule_->Build(emitterBuildInfo);
			}

			// Allow TypeData module to cache pointers to modules
			highLODLevel->typeDataModule_->CacheModuleInfo(this);
		}

		// Cache particle size/offset data for all LOD Levels
		CacheEmitterModuleInfo();
	}
}

void ParticleEmitter::CacheEmitterModuleInfo()
{
	// This assert makes sure that packing is as expected.
	// Added FBaseColor...
	// Linear color change
	// Added Flags field

	requiresLoopNotification_ = false;
	axisLockEnabled_ = false;
	meshRotationActive_ = false;
	lockAxisFlags_ = EPAL_NONE;
	moduleOffsetMap_.Clear();
	moduleInstanceOffsetMap_.Clear();
	moduleRandomSeedInstanceOffsetMap_.Clear();
	modulesNeedingInstanceData_.Clear();
	modulesNeedingRandomSeedInstanceData_.Clear();
	meshMaterials_.Clear();
	dynamicParameterDataOffset_ = 0;
	lightDataOffset_ = 0;
	lightVolumetricScatteringIntensity_ = 0;
	cameraPayloadOffset_ = 0;
	particleSize_ = sizeof(BaseParticle);
	reqInstanceBytes_ = 0;
	pivotOffset_ = Vector2(-0.5f, -0.5f);
	typeDataOffset_ = 0;
	typeDataInstanceOffset_ = -1;
	subUVAnimation_ = nullptr;

	ParticleLODLevel* highLODLevel = GetLODLevel(0);
	ASSERT(highLODLevel);

	ParticleModuleTypeDataBase* highTypeData = highLODLevel->typeDataModule_;
	if (highTypeData)
	{
		Int32 reqBytes = highTypeData->RequiredBytes(static_cast<ParticleModuleTypeDataBase*>(nullptr));
		if (reqBytes)
		{
			typeDataOffset_ = particleSize_;
			particleSize_ += reqBytes;
		}

		Int32 tempInstanceBytes = highTypeData->RequiredBytesPerInstance();
		if (tempInstanceBytes)
		{
			typeDataInstanceOffset_ = reqInstanceBytes_;
			reqInstanceBytes_ += tempInstanceBytes;
		}
	}

	// Grab required module
	ParticleModuleRequired* requiredModule = highLODLevel->requiredModule_;
	ASSERT(requiredModule);
	// mesh rotation active if alignment is set
	meshRotationActive_ = (requiredModule->screenAlignment_ == PSA_Velocity || requiredModule->screenAlignment_ == PSA_AwayFromCenter);

	// NOTE: This code assumes that the same module order occurs in all LOD levels

	for (Int32 moduleIdx = 0; moduleIdx < highLODLevel->modules_.Size(); moduleIdx++)
	{
		ParticleModule* particleModule = highLODLevel->modules_[moduleIdx];
		ASSERT(particleModule);

		// Loop notification?
		requiresLoopNotification_ |= (particleModule->enabled_ && particleModule->RequiresLoopingNotification());

		if (particleModule->IsInstanceOf(ParticleModuleTypeDataBase::GetTypeStatic()) == false)
		{
			Int32 reqBytes = particleModule->RequiredBytes(highTypeData);
			if (reqBytes)
			{
				moduleOffsetMap_.Populate(particleModule, particleSize_);
				if (particleModule->IsInstanceOf(ParticleModuleParameterDynamic::GetTypeStatic()) && (dynamicParameterDataOffset_ == 0))
				{
					dynamicParameterDataOffset_ = particleSize_;
				}
				if (particleModule->IsInstanceOf(ParticleModuleLight::GetTypeStatic()) && (lightDataOffset_ == 0))
				{
					ParticleModuleLight* particleModuleLight = Cast<ParticleModuleLight>(particleModule);
					lightVolumetricScatteringIntensity_ = particleModuleLight->volumetricScatteringIntensity_;
					lightDataOffset_ = particleSize_;
				}
				if (particleModule->IsInstanceOf(ParticleModuleCameraOffset::GetTypeStatic()) && (cameraPayloadOffset_ == 0))
				{
					cameraPayloadOffset_ = particleSize_;
				}
				particleSize_ += reqBytes;
			}

			Int32 tempInstanceBytes = particleModule->RequiredBytesPerInstance();
			if (tempInstanceBytes > 0)
			{
				// Add the high-lodlevel offset to the lookup map
				moduleInstanceOffsetMap_.Populate(particleModule, reqInstanceBytes_);
				// Remember that this module has emitter-instance data
				modulesNeedingInstanceData_.Push(particleModule);

				// Add all the other LODLevel modules, using the same offset.
				// This removes the need to always also grab the HighestLODLevel pointer.
				for (Int32 LODIdx = 1; LODIdx < LODLevels_.Size(); LODIdx++)
				{
					ParticleLODLevel* curLODLevel = LODLevels_[LODIdx];
					moduleInstanceOffsetMap_.Populate(curLODLevel->modules_[moduleIdx], reqInstanceBytes_);
				}
				reqInstanceBytes_ += tempInstanceBytes;
			}

			// Add space for per instance random seed value if required
			if (/*FApp::bUseFixedSeed || */particleModule->supportsRandomSeed_)
			{
				// Add the high-lodlevel offset to the lookup map
				moduleRandomSeedInstanceOffsetMap_.Populate(particleModule, reqInstanceBytes_);
				// Remember that this module has emitter-instance data
				modulesNeedingRandomSeedInstanceData_.Push(particleModule);

				// Add all the other LODLevel modules, using the same offset.
				// This removes the need to always also grab the HighestLODLevel pointer.
				for (Int32 LODIdx = 1; LODIdx < LODLevels_.Size(); LODIdx++)
				{
					ParticleLODLevel* curLODLevel = LODLevels_[LODIdx];
					moduleRandomSeedInstanceOffsetMap_.Populate(curLODLevel->modules_[moduleIdx], reqInstanceBytes_);
				}

				reqInstanceBytes_ += sizeof(ParticleRandomSeedInstancePayload);
			}
		}

		if (particleModule->IsInstanceOf(ParticleModuleOrientationAxisLock::GetTypeStatic()))
		{
			ParticleModuleOrientationAxisLock* module_AxisLock = Cast<ParticleModuleOrientationAxisLock>(particleModule);
			axisLockEnabled_ = module_AxisLock->enabled_;
			lockAxisFlags_ = module_AxisLock->lockAxisFlags_;
		}
		else if (particleModule->IsInstanceOf(ParticleModulePivotOffset::GetTypeStatic()))
		{
			pivotOffset_ += Cast<ParticleModulePivotOffset>(particleModule)->pivotOffset_;
		}
		else if (particleModule->IsInstanceOf(ParticleModuleMeshMaterial::GetTypeStatic()))
		{
			ParticleModuleMeshMaterial* meshMaterialModule = Cast<ParticleModuleMeshMaterial>(particleModule);
			if (meshMaterialModule->enabled_)
			{
				meshMaterials_.Reserve(meshMaterialModule->meshMaterials_.Size());
				for (auto& mt : meshMaterialModule->meshMaterials_)
				{
					meshMaterials_.Push(mt);
				}
			}
		}
		else if (particleModule->IsInstanceOf(ParticleModuleSubUV::GetTypeStatic()))
		{
			SubUVAnimation* moduleSubUVAnimation = Cast<ParticleModuleSubUV>(particleModule)->animation_;
			subUVAnimation_ = moduleSubUVAnimation && moduleSubUVAnimation->subUVTexture_ && moduleSubUVAnimation->IsBoundingGeometryValid()
				? moduleSubUVAnimation
				: NULL;
		}
		// Perform validation / fixup on some modules that can cause crashes if LODs / Modules are out of sync
		// This should only be applied on uncooked builds to avoid wasting cycles
		//else if (!PlatformProperties::RequiresCookedData())
		//{
		//	if (particleModule->IsInstanceOf(ParticleModuleLocationBoneSocket::GetTypeStatic()))
		//	{
		//		ParticleModuleLocationBoneSocket::ValidateLODLevels(this, moduleIdx);
		//	}
		//}

		// Set bMeshRotationActive if module says so
		if(!meshRotationActive_ && particleModule->TouchesMeshRotation())
		{
			meshRotationActive_ = true;
		}
	}
}

float ParticleEmitter::GetQualityLevelSpawnRateMult()
{
	return 1.0f;
}

bool ParticleEmitter::HasAnyEnabledLODs()const
{
	for (ParticleLODLevel* LodLevel : LODLevels_)
	{
		if (LodLevel && LodLevel->enabled_)
		{
			return true;
		}
	}
	
	return false;
}

bool ParticleEmitter::IsSignificant(ParticleSignificanceLevel requiredSignificance)
{
	ParticleSystem* pSysOuter = Cast<ParticleSystem>(GetOuter());
	ParticleSignificanceLevel Significance = Min(pSysOuter->maxSignificanceLevel_, significanceLevel_);
	return Significance >= requiredSignificance;
}

ParticleLODLevel* ParticleEmitter::GetCurrentLODLevel(ParticleEmitterInstance* instance)
{
	//if (!FPlatformProperties::HasEditorOnlyData())
	//{
	//	return Instance->CurrentLODLevel;
	//}
	//else
	//{
	//	// for the game (where we care about perf) we don't branch
	//	if (Instance->GetWorld()->IsGameWorld() )
	//	{
	//		return Instance->CurrentLODLevel;
	//	}
	//	else
	//	{
	//		EditorUpdateCurrentLOD( Instance );
	//		return Instance->CurrentLODLevel;
	//	}
	//}
	return instance->currentLODLevel_;
}

}
