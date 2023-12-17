#include "ParticleSystem.h"
#include "Math/InterpCurveEdSetup.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "ParticleSystem/ParticleLODLevel.h"
#include "ParticleSystem/Module/ParticleModuleTypeData.h"
#include "ParticleSystem/Module/ParticleModuleColor.h"

namespace FlagGG
{

/*-----------------------------------------------------------------------------
	ParticleSystem implementation.
-----------------------------------------------------------------------------*/

ParticleSystem::ParticleSystem()
	: occlusionBoundsMethod_(EPSOBM_ParticleBounds)
	, anyEmitterLoopsForever_(false)
	, highestSignificance_(ParticleSignificanceLevel::Critical)
	, lowestSignificance_(ParticleSignificanceLevel::Low)
	, shouldManageSignificance_(false)
	, isImmortal_(false)
	, willBecomeZombie_(false)
{
#if WITH_EDITORONLY_DATA
	thumbnailDistance_ = 200.0;
	thumbnailWarmup_ = 1.0;
#endif // WITH_EDITORONLY_DATA
	updateTime_FPS_ = 60.0f;
	updateTime_Delta_ = 1.0f / 60.0f;
	warmupTime_ = 0.0f;
	warmupTickRate_ = 0.0f;
#if WITH_EDITORONLY_DATA
	editorLODSetting_ = 0;
#endif // WITH_EDITORONLY_DATA
	fixedRelativeBoundingBox_.min_ = Vector3(-1.0f, -1.0f, -1.0f);
	fixedRelativeBoundingBox_.max_ = Vector3(1.0f, 1.0f, 1.0f);

	LODMethod_ = PARTICLESYSTEMLODMETHOD_Automatic;
	LODDistanceCheckTime_ = 0.25f;
	regenerateLODDuplicate_ = false;
	thumbnailImageOutOfDate_ = true;
#if WITH_EDITORONLY_DATA
	floorMesh_ = TEXT("/Engine/EditorMeshes/AnimTreeEd_PreviewFloor.AnimTreeEd_PreviewFloor");
	floorPosition_ = Vector3(0.0f, 0.0f, 0.0f);
	floorRotation_ = Rotator(0.0f, 0.0f, 0.0f);
	floorScale_ = 1.0f;
	floorScale3D_ = Vector3(1.0f, 1.0f, 1.0f);
#endif // WITH_EDITORONLY_DATA

	macroUVPosition_ = Vector3(0.0f, 0.0f, 0.0f);

	macroUVRadius_ = 200.0f;
	autoDeactivate_ = true;
	minTimeBetweenTicks_ = 0;
	insignificantReaction_ = ParticleSystemInsignificanceReaction::Auto;
	insignificanceDelay_ = 0.0f;
	maxSignificanceLevel_ = ParticleSignificanceLevel::Critical;
	maxPoolSize_ = 32;


	allowManagedTicking_ = true;
}

ParticleSystem::~ParticleSystem() = default;


ParticleSystemLODMethod ParticleSystem::GetCurrentLODMethod()
{
	return ParticleSystemLODMethod(LODMethod_);
}


Int32 ParticleSystem::GetLODLevelCount()
{
	return LODDistances_.Size();
}


float ParticleSystem::GetLODDistance(Int32 LODLevelIndex)
{
	if (LODLevelIndex >= LODDistances_.Size())
	{
		return -1.0f;
	}

	return LODDistances_[LODLevelIndex];
}

void ParticleSystem::SetCurrentLODMethod(ParticleSystemLODMethod InMethod)
{
	LODMethod_ = InMethod;
}


bool ParticleSystem::SetLODDistance(Int32 LODLevelIndex, float inDistance)
{
	if (LODLevelIndex >= LODDistances_.Size())
	{
		return false;
	}

	LODDistances_[LODLevelIndex] = inDistance;

	return true;
}

bool ParticleSystem::DoesAnyEmitterHaveMotionBlur(Int32 LODLevelIndex) const
{
	for (auto& emitterIter : emitters_)
	{
		if (emitterIter)
		{
			auto* emitterLOD = emitterIter->GetLODLevel(LODLevelIndex);
			if (!emitterLOD)
			{
				continue;
			}

			if (emitterLOD->typeDataModule_ && emitterLOD->typeDataModule_->IsMotionBlurEnabled())
			{
				return true;
			}
		}
	}

	return false;
}

void ParticleSystem::UpdateColorModuleClampAlpha(ParticleModuleColorBase* colorModule)
{
	if (colorModule)
	{
		PODVector<const CurveEdEntry*> curveEntries;
		colorModule->RemoveModuleCurvesFromEditor(curveEdSetup_);
		colorModule->AddModuleCurvesToEditor(curveEdSetup_, curveEntries);
	}
}

bool ParticleSystem::CanBePooled() const
{
	if (maxPoolSize_ == 0)
	{
		return false;
	}

	return true;
}

bool ParticleSystem::CalculateMaxActiveParticleCounts()
{
	bool success = true;

	for (Int32 emitterIndex = 0; emitterIndex < emitters_.Size(); emitterIndex++)
	{
		ParticleEmitter* emitter = emitters_[emitterIndex];
		if (emitter)
		{
			if (emitter->CalculateMaxActiveParticleCount() == false)
			{
				success = false;
			}
		}
	}

	return success;
}


void ParticleSystem::GetParametersUtilized(Vector<Vector<String>>& particleSysParamList,
	Vector<Vector<String>>& particleParameterList)
{
	particleSysParamList.Clear();
	particleParameterList.Clear();

	for (Int32 emitterIndex = 0; emitterIndex < emitters_.Size(); emitterIndex++)
	{
		particleSysParamList.EmplaceBack();
		particleParameterList.EmplaceBack();

		ParticleEmitter* emitter = emitters_[emitterIndex];
		if (emitter)
		{
			emitter->GetParametersUtilized(
				particleSysParamList[emitterIndex],
				particleParameterList[emitterIndex]);
		}
	}
}


void ParticleSystem::SetupSoloing()
{

}


bool ParticleSystem::ToggleSoloing(class ParticleEmitter* inEmitter)
{
	bool bSoloingReturn = false;
	if (inEmitter != NULL)
	{
		bool bOtherEmitterIsSoloing = false;
		// Set the given one
		Int32 selectedIndex = -1;
		for (Int32 emitterIdx = 0; emitterIdx < emitters_.Size(); emitterIdx++)
		{
			ParticleEmitter* emitter = emitters_[emitterIdx];
			ASSERT(emitter != NULL);
			if (emitter == inEmitter)
			{
				selectedIndex = emitterIdx;
			}
			else
			{
				if (emitter->isSoloing_ == true)
				{
					bOtherEmitterIsSoloing = true;
					bSoloingReturn = true;
				}
			}
		}

		if (selectedIndex != -1)
		{
			inEmitter->isSoloing_ = !inEmitter->isSoloing_;
			for (Int32 emitterIdx = 0; emitterIdx < emitters_.Size(); emitterIdx++)
			{
				ParticleEmitter* emitter = emitters_[emitterIdx];
				LODSoloTrack& soloTrack = soloTracking_[emitterIdx];
				if (emitterIdx == selectedIndex)
				{
					for (Int32 LODIdx = 0; LODIdx < inEmitter->LODLevels_.Size(); LODIdx++)
					{
						ParticleLODLevel* LODLevel = inEmitter->LODLevels_[LODIdx];
						if (inEmitter->isSoloing_ == false)
						{
							if (bOtherEmitterIsSoloing == false)
							{
								// Restore the enabled settings - ie turn off soloing...
								LODLevel->enabled_ = soloTrack.soloEnableSetting_[LODIdx];
							}
							else
							{
								// Disable the emitter
								LODLevel->enabled_ = false;
							}
						}
						else
							if (bOtherEmitterIsSoloing == true)
							{
								// Need to restore old settings of this emitter as it is now soloing
								LODLevel->enabled_ = soloTrack.soloEnableSetting_[LODIdx];
							}
					}
				}
				else
				{
					// Restore all other emitters if this disables soloing...
					if ((inEmitter->isSoloing_ == false) && (bOtherEmitterIsSoloing == false))
					{
						for (Int32 LODIdx = 0; LODIdx < emitter->LODLevels_.Size(); LODIdx++)
						{
							ParticleLODLevel* LODLevel = emitter->LODLevels_[LODIdx];
							// Restore the enabled settings - ie turn off soloing...
							LODLevel->enabled_ = soloTrack.soloEnableSetting_[LODIdx];
						}
					}
					else
					{
						if (emitter->isSoloing_ == false)
						{
							for (Int32 LODIdx = 0; LODIdx < emitter->LODLevels_.Size(); LODIdx++)
							{
								ParticleLODLevel* LODLevel = emitter->LODLevels_[LODIdx];
								// Disable the emitter
								LODLevel->enabled_ = false;
							}
						}
					}
				}
			}
		}

		// We checked the other emitters above...
		// Make sure we catch the case of the first one toggled to true!
		if (inEmitter->isSoloing_ == true)
		{
			bSoloingReturn = true;
		}
	}

	return bSoloingReturn;
}


bool ParticleSystem::TurnOffSoloing()
{
	for (Int32 emitterIdx = 0; emitterIdx < emitters_.Size(); emitterIdx++)
	{
		ParticleEmitter* emitter = emitters_[emitterIdx];
		if (emitter != NULL)
		{
			LODSoloTrack& soloTrack = soloTracking_[emitterIdx];
			for (Int32 LODIdx = 0; LODIdx < emitter->LODLevels_.Size(); LODIdx++)
			{
				ParticleLODLevel* LODLevel = emitter->LODLevels_[LODIdx];
				if (LODLevel != NULL)
				{
					// Restore the enabled settings - ie turn off soloing...
					LODLevel->enabled_ = soloTrack.soloEnableSetting_[LODIdx];
				}
			}
			emitter->isSoloing_ = false;
		}
	}

	return true;
}


void ParticleSystem::SetupLODValidity()
{
	for (Int32 emitterIdx = 0; emitterIdx < emitters_.Size(); emitterIdx++)
	{
		ParticleEmitter* emitter = emitters_[emitterIdx];
		if (emitter != NULL)
		{
			for (Int32 Pass = 0; Pass < 2; Pass++)
			{
				for (Int32 LODIdx = 0; LODIdx < emitter->LODLevels_.Size(); LODIdx++)
				{
					ParticleLODLevel* LODLevel = emitter->LODLevels_[LODIdx];
					if (LODLevel != NULL)
					{
						for (Int32 moduleIdx = -3; moduleIdx < LODLevel->modules_.Size(); moduleIdx++)
						{
							Int32 moduleFetchIdx;
							switch (moduleIdx)
							{
							case -3:	moduleFetchIdx = INDEX_REQUIREDMODULE;	break;
							case -2:	moduleFetchIdx = INDEX_SPAWNMODULE;		break;
							case -1:	moduleFetchIdx = INDEX_TYPEDATAMODULE;	break;
							default:	moduleFetchIdx = moduleIdx;				break;
							}

							ParticleModule* module = LODLevel->GetModuleAtIndex(moduleFetchIdx);
							if (module != NULL)
							{
								// On pass 1, clear the LODValidity flags
								// On pass 2, set it
								if (Pass == 0)
								{
									module->LODValidity_ = 0;
								}
								else
								{
									module->LODValidity_ |= (1 << LODIdx);
								}
							}
						}
					}
				}
			}
		}
	}
}

void ParticleSystem::SetDelay(float inDelay)
{
	delay_ = inDelay;
}

void ParticleSystem::BuildEmitters()
{
	const Int32 emitterCount = emitters_.Size();
	for (Int32 emitterIndex = 0; emitterIndex < emitterCount; ++emitterIndex)
	{
		if (ParticleEmitter* emitter = emitters_[emitterIndex])
		{
			emitter->Build();
		}
	}
}

static bool LogReasoningForAnyThreadTicking()
{
	static bool bLogThreadedParticleTicking = /*FParse::Param(FCommandLine::Get(), TEXT("LogThreadedParticleTicking"))*/false;
	return bLogThreadedParticleTicking;
}

void ParticleSystem::ComputeCanTickInAnyThread()
{
	ASSERT(!isElligibleForAsyncTickComputed_);
	isElligibleForAsyncTickComputed_ = true;

	isElligibleForAsyncTick_ = true; // assume everything is async
	Int32 emitterIndex;
	for (emitterIndex = 0; emitterIndex < emitters_.Size(); emitterIndex++)
	{
		ParticleEmitter* emitter = emitters_[emitterIndex];
		if (emitter)
		{
			for (Int32 levelIndex = 0; levelIndex < emitter->LODLevels_.Size(); levelIndex++)
			{
				ParticleLODLevel* LODLevel = emitter->LODLevels_[levelIndex];
				if (LODLevel)
				{
					for (Int32 moduleIndex = 0; moduleIndex < LODLevel->modules_.Size(); moduleIndex++)
					{
						ParticleModule* module = LODLevel->modules_[moduleIndex];
						if (module && !module->CanTickInAnyThread())
						{
							isElligibleForAsyncTick_ = false;
							if (LogReasoningForAnyThreadTicking())
							{
								FLAGGG_LOG_INFO("Cannot tick %s in parallel because module %s in Emitter %s cannot tick in in parallel.", GetName().CString(), *module->GetTypeName().CString(), *emitter->GetTypeName().CString());
							}
							else
							{
								return;
							}
						}
					}
				}
			}

		}
	}
	if (LogReasoningForAnyThreadTicking() && isElligibleForAsyncTick_)
	{
		FLAGGG_LOG_INFO("Can tick %s in parallel.", GetName().CString());
	}
}

bool ParticleSystem::HasGPUEmitter() const
{
	//for (Int32 emitterIndex = 0; emitterIndex < emitters_.Size(); ++emitterIndex)
	//{
	//	if (emitters_[emitterIndex] == nullptr)
	//	{
	//		continue;
	//	}
	//	// We can just check for the GPU type data at the highest LOD.
	//	ParticleLODLevel* LODLevel = emitters_[emitterIndex]->LODLevels_[0];
	//	if (LODLevel)
	//	{
	//		ParticleModule* typeDataModule = LODLevel->typeDataModule_;
	//		if (typeDataModule && typeDataModule->IsInstanceOf(ParticleModuleTypeDataGpu::GetTypeNameStatic()))
	//		{
	//			return true;
	//		}
	//	}
	//}
	return false;
}

}
