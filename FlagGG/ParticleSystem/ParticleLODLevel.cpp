#include "ParticleLODLevel.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleModule.h"
#include "ParticleSystem/Module/ParticleModuleSpawn.h"
#include "ParticleSystem/Module/ParticleModuleTypeData.h"
#include "ParticleSystem/Module/ParticleModuleLifetime.h"
#include "Log.h"

namespace FlagGG
{

	/*-----------------------------------------------------------------------------
	ParticleLODLevel implementation.
-----------------------------------------------------------------------------*/
ParticleLODLevel::ParticleLODLevel()
{
	enabled_ = true;
	convertedModules_ = true;
	peakActiveParticles_ = 0;
}


void ParticleLODLevel::CompileModules(ParticleEmitterBuildInfo& emitterBuildInfo)
{
	ASSERT(requiredModule_);
	ASSERT(spawnModule_);

	// Store a few special modules.
	emitterBuildInfo.requiredModule_ = requiredModule_;
	emitterBuildInfo.spawnModule_ = spawnModule_;

	// Compile those special modules.
	requiredModule_->CompileModule(emitterBuildInfo);
	if (spawnModule_->enabled_)
	{
		spawnModule_->CompileModule(emitterBuildInfo);
	}

	// Compile all remaining modules.
	const Int32 moduleCount = modules_.Size();
	for (Int32 moduleIndex = 0; moduleIndex < moduleCount; ++moduleIndex)
	{
		ParticleModule* module = modules_[moduleIndex];
		if (module && module->enabled_)
		{
			module->CompileModule(emitterBuildInfo);
		}
	}

	// Estimate the maximum number of active particles.
	emitterBuildInfo.estimatedMaxActiveParticleCount_ = CalculateMaxActiveParticleCount();
}

void ParticleLODLevel::UpdateModuleLists()
{
	spawningModules_.Clear();
	spawnModules_.Clear();
	updateModules_.Clear();
	orbitModules_.Clear();

	ParticleModule* module;
	Int32 typeDataModuleIndex = -1;

	for (Int32 i = 0; i < modules_.Size(); i++)
	{
		module = modules_[i];
		if (!module)
		{
			continue;
		}

		if (module->spawnModule_)
		{
			spawnModules_.Push(SharedPtr<ParticleModule>(module));
		}
		if (module->updateModule_ || module->finalUpdateModule_)
		{
			updateModules_.Push(SharedPtr<ParticleModule>(module));
		}

		if (module->IsInstanceOf(ParticleModuleTypeDataBase::GetTypeStatic()))
		{
			typeDataModule_ = Cast<ParticleModuleTypeDataBase>(module);
			if (!module->spawnModule_ && !module->updateModule_)
			{
				// For now, remove it from the list and set it as the TypeDataModule
				typeDataModuleIndex = i;
			}
		}
		else if (module->IsInstanceOf(ParticleModuleSpawnBase::GetTypeStatic()))
		{
			ParticleModuleSpawnBase* spawnBase = Cast<ParticleModuleSpawnBase>(module);
			spawningModules_.Push(SharedPtr<ParticleModuleSpawnBase>(spawnBase));
		}
		else if (module->IsInstanceOf(ParticleModuleOrbit::GetTypeStatic()))
		{
			ParticleModuleOrbit* orbit = Cast<ParticleModuleOrbit>(module);
			orbitModules_.Push(SharedPtr<ParticleModuleOrbit>(orbit));
		}
	}

	if (typeDataModuleIndex != -1)
	{
		modules_.Erase(typeDataModuleIndex);
	}

	if (typeDataModule_ /**&& (Level == 0)**/)
	{
		ParticleModuleTypeDataMesh* MeshTD = Cast<ParticleModuleTypeDataMesh>(typeDataModule_);
		if (MeshTD
			&& MeshTD->Mesh
			&& MeshTD->Mesh->HasValidRenderData(false))
		{
			ParticleSpriteEmitter* SpriteEmitter = Cast<ParticleSpriteEmitter>(GetOuter());
			if (SpriteEmitter && (MeshTD->bOverrideMaterial == false))
			{
				FStaticMeshSection& Section = MeshTD->Mesh->GetRenderData()->LODResources[0].Sections[0];
				Material* Material = MeshTD->Mesh->GetMaterial(Section.MaterialIndex);
				if (Material)
				{
					requiredModule_->Material = Material;
				}
			}
		}
	}
}


bool ParticleLODLevel::GenerateFromLODLevel(ParticleLODLevel* sourceLODLevel, float percentage, bool generateModuleData)
{
	// See if there are already modules in place
	if (modules_.Size() > 0)
	{
		FLAGGG_LOG_ERROR("ERROR? - GenerateFromLODLevel - modules already present!");
		return false;
	}

	// Allocate slots in the array...
	modules_.Resize(sourceLODLevel->modules_.Size());

	// Set the enabled flag
	enabled_ = sourceLODLevel->enabled_;

	// Set up for undo/redo!
	// SetFlags(RF_Transactional);

	// Required module...
	requiredModule_ = Cast<ParticleModuleRequired>(
		sourceLODLevel->requiredModule_->GenerateLODModule(sourceLODLevel, this, percentage, generateModuleData));

	// Spawn module...
	spawnModule_ = Cast<ParticleModuleSpawn>(
		sourceLODLevel->spawnModule_->GenerateLODModule(sourceLODLevel, this, percentage, generateModuleData));

	// TypeData module, if present...
	if (sourceLODLevel->typeDataModule_)
	{
		typeDataModule_ = 
			Cast<ParticleModuleTypeDataBase>(
			sourceLODLevel->typeDataModule_->GenerateLODModule(sourceLODLevel, this, percentage, generateModuleData));
		ASSERT(typeDataModule_ == sourceLODLevel->typeDataModule_); // Code expects typedata to be the same across LODs
	}

	// The remaining modules...
	for (Int32 moduleIndex = 0; moduleIndex < sourceLODLevel->modules_.Size(); moduleIndex++)
	{
		if (sourceLODLevel->modules_[moduleIndex])
		{
			modules_[moduleIndex] = sourceLODLevel->modules_[moduleIndex]->GenerateLODModule(sourceLODLevel, this, percentage, generateModuleData);
		}
		else
		{
			modules_[moduleIndex] = NULL;
		}
	}

	return true;
}


Int32 ParticleLODLevel::CalculateMaxActiveParticleCount()
{
	ASSERT(requiredModule_ != NULL);

	// Determine the lifetime for particles coming from the emitter
	float particleLifetime = 0.0f;
	float maxSpawnRate = spawnModule_->GetEstimatedSpawnRate();
	Int32 maxBurstCount = spawnModule_->GetMaximumBurstCount();
	for (Int32 moduleIndex = 0; moduleIndex < modules_.Size(); moduleIndex++)
	{
		ParticleModuleLifetimeBase* lifetimeMod = Cast<ParticleModuleLifetimeBase>(modules_[moduleIndex]);
		if (lifetimeMod != NULL)
		{
			particleLifetime += lifetimeMod->GetMaxLifetime();
		}

		ParticleModuleSpawnBase* spawnMod = Cast<ParticleModuleSpawnBase>(modules_[moduleIndex]);
		if (spawnMod != NULL)
		{
			maxSpawnRate += spawnMod->GetEstimatedSpawnRate();
			maxBurstCount += spawnMod->GetMaximumBurstCount();
		}
	}

	// Determine the maximum duration for this particle system
	float maxDuration = 0.0f;
	float totalDuration = 0.0f;
	Int32 totalLoops = 0;
	if (requiredModule_ != NULL)
	{
		// We don't care about delay wrt spawning...
		maxDuration = Max<float>(requiredModule_->emitterDuration_, requiredModule_->emitterDurationLow_);
		totalLoops = requiredModule_->emitterLoops_;
		totalDuration = maxDuration * totalLoops;
	}

	// Determine the max
	Int32 maxAPC = 0;

	if (totalDuration != 0.0f)
	{
		if (totalLoops == 1)
		{
			// Special case for one loop... 
			if (particleLifetime < maxDuration)
			{
				maxAPC += CeilToInt(particleLifetime * maxSpawnRate);
			}
			else
			{
				maxAPC += CeilToInt(maxDuration * maxSpawnRate);
			}
			// Safety zone...
			maxAPC += 1;
			// Add in the bursts...
			maxAPC += maxBurstCount;
		}
		else
		{
			if (particleLifetime < maxDuration)
			{
				maxAPC += CeilToInt(particleLifetime * maxSpawnRate);
			}
			else
			{
				maxAPC += (CeilToInt(CeilToInt(maxDuration * maxSpawnRate) * particleLifetime));
			}
			// Safety zone...
			maxAPC += 1;
			// Add in the bursts...
			maxAPC += maxBurstCount;
			if (particleLifetime > maxDuration)
			{
				maxAPC += maxBurstCount * CeilToInt(particleLifetime - maxDuration);
			}
		}
	}
	else
	{
		// We are infinite looping... 
		// Single loop case is all we will worry about. Safer base estimate - but not ideal.
		if (particleLifetime < maxDuration)
		{
			maxAPC += CeilToInt(particleLifetime * CeilToInt(maxSpawnRate));
		}
		else
		{
			if (particleLifetime != 0.0f)
			{
				if (particleLifetime <= maxDuration)
				{
					maxAPC += CeilToInt(maxDuration * maxSpawnRate);
				}
				else //if (particleLifetime > maxDuration)
				{
					maxAPC += CeilToInt(maxDuration * maxSpawnRate) * particleLifetime;
				}
			}
			else
			{
				// No lifetime, no duration...
				maxAPC += CeilToInt(maxSpawnRate);
			}
		}
		// Safety zone...
		maxAPC += Max<Int32>(CeilToInt(maxSpawnRate * 0.032f), 2);
		// Burst
		maxAPC += maxBurstCount;
	}

	peakActiveParticles_ = maxAPC;

	return maxAPC;
}


void ParticleLODLevel::ConvertToSpawnModule()
{
#if WITH_EDITOR
	// Move the required module SpawnRate and Burst information to a new spawnModule_.
	if (spawnModule_)
	{
//		UE_LOG(LogParticles, Warning, TEXT("LOD Level already has a spawn module!"));
		return;
	}

	ParticleEmitter* EmitterOuter = Cast<ParticleEmitter>(GetOuter());
	spawnModule_ = NewObject<ParticleModuleSpawn>(EmitterOuter->GetOuter());
	ASSERT(spawnModule_);

	UDistributionFloat* SourceDist = requiredModule_->SpawnRate.Distribution;
	if (SourceDist)
	{
		spawnModule_->Rate.Distribution = Cast<UDistributionFloat>(StaticDuplicateObject(SourceDist, spawnModule_));
		spawnModule_->Rate.Distribution->bIsDirty = true;
		spawnModule_->Rate.Initialize();
	}

	// Now the burst list.
	Int32 BurstCount = requiredModule_->BurstList.Size();
	if (BurstCount > 0)
	{
		spawnModule_->BurstList.AddZeroed(BurstCount);
		for (Int32 BurstIndex = 0; BurstIndex < BurstCount; BurstIndex++)
		{
			spawnModule_->BurstList[BurstIndex].Count = requiredModule_->BurstList[BurstIndex].Count;
			spawnModule_->BurstList[BurstIndex].CountLow = requiredModule_->BurstList[BurstIndex].CountLow;
			spawnModule_->BurstList[BurstIndex].Time = requiredModule_->BurstList[BurstIndex].Time;
		}
	}

	MarkPackageDirty();
#endif	//#if WITH_EDITOR
}


Int32 ParticleLODLevel::GetModuleIndex(ParticleModule* inModule)
{
	if (inModule)
	{
		if (inModule == requiredModule_)
		{
			return INDEX_REQUIREDMODULE;
		}
		else if (inModule == spawnModule_)
		{
			return INDEX_SPAWNMODULE;
		}
		else if (inModule == typeDataModule_)
		{
			return INDEX_TYPEDATAMODULE;
		}
		else
		{
			for (Int32 moduleIndex = 0; moduleIndex < modules_.Size(); moduleIndex++)
			{
				if (inModule == modules_[moduleIndex])
				{
					return moduleIndex;
				}
			}
		}
	}

	return INDEX_NONE;
}


ParticleModule* ParticleLODLevel::GetModuleAtIndex(Int32 inIndex)
{
	// 'Normal' modules
	if (inIndex > INDEX_NONE)
	{
		if (inIndex < modules_.Size())
		{
			return modules_[inIndex];
		}

		return NULL;
	}

	switch (inIndex)
	{
	case INDEX_REQUIREDMODULE:		return requiredModule_;
	case INDEX_SPAWNMODULE:			return spawnModule_;
	case INDEX_TYPEDATAMODULE:		return typeDataModule_;
	}

	return NULL;
}


void ParticleLODLevel::SetLevelIndex(Int32 inLevelIndex)
{
	// Remove the 'current' index from the validity flags and set the new one.
	requiredModule_->LODValidity_ &= ~(1 << level_);
	requiredModule_->LODValidity_ |= (1 << inLevelIndex);
	spawnModule_->LODValidity_ &= ~(1 << level_);
	spawnModule_->LODValidity_ |= (1 << inLevelIndex);
	if (typeDataModule_)
	{
		typeDataModule_->LODValidity_ &= ~(1 << level_);
		typeDataModule_->LODValidity_ |= (1 << inLevelIndex);
	}
	for (Int32 moduleIndex = 0; moduleIndex < modules_.Size(); moduleIndex++)
	{
		ParticleModule* CheckModule = modules_[moduleIndex];
		if (CheckModule)
		{
			CheckModule->LODValidity_ &= ~(1 << level_);
			CheckModule->LODValidity_ |= (1 << inLevelIndex);
		}
	}

	level_ = inLevelIndex;
}


bool ParticleLODLevel::IsModuleEditable(ParticleModule* inModule)
{
	// If the module validity flag is not set for this level, it is not editable.
	if ((inModule->LODValidity_ & (1 << level_)) == 0)
	{
		return false;
	}

	// If the module is shared w/ higher LOD levels, then it is not editable...
	Int32 validity = 0;
	if (level_ > 0)
	{
		Int32 check = level_ - 1;
		while (check >= 0)
		{
			validity |= (1 << check);
			check--;
		}

		if ((validity & inModule->LODValidity_) != 0)
		{
			return false;
		}
	}

	return true;
}

}
