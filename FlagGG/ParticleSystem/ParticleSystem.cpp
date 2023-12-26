#include "ParticleSystem.h"
#include "Math/InterpCurveEdSetup.h"
#include "Math/Distributions/DistributionFloatConstant.h"
#include "Math/Distributions/DistributionFloatUniform.h"
#include "Math/Distributions/DistributionVectorUniform.h"
#include "Math/Distributions/DistributionVectorConstantCurve.h"
#include "Math/Distributions/DistributionFloatConstantCurve.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "ParticleSystem/ParticleLODLevel.h"
#include "ParticleSystem/Module/ParticleModuleAcceleration.h"
#include "ParticleSystem/Module/ParticleModuleCamera.h"
#include "ParticleSystem/Module/ParticleModuleCollision.h"
#include "ParticleSystem/Module/ParticleModuleColor.h"
#include "ParticleSystem/Module/ParticleModuleLight.h"
#include "ParticleSystem/Module/ParticleModuleLifetime.h"
#include "ParticleSystem/Module/ParticleModuleLocation.h"
#include "ParticleSystem/Module/ParticleModuleMaterial.h"
#include "ParticleSystem/Module/ParticleModuleOrbit.h"
#include "ParticleSystem/Module/ParticleModuleOrientation.h"
#include "ParticleSystem/Module/ParticleModuleParameter.h"
#include "ParticleSystem/Module/ParticleModuleRotation.h"
#include "ParticleSystem/Module/ParticleModuleRotationRate.h"
#include "ParticleSystem/Module/ParticleModuleSize.h"
#include "ParticleSystem/Module/ParticleModuleSubUV.h"
#include "ParticleSystem/Module/ParticleModuleTrail.h"
#include "ParticleSystem/Module/ParticleModuleTypeData.h"
#include "ParticleSystem/Module/ParticleModuleVectorField.h"
#include "ParticleSystem/Module/ParticleModuleVelocity.h"
#include "Resource/XMLFile.h"
#include "Resource/ResourceCache.h"

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
	floorMesh_ = "/Engine/EditorMeshes/AnimTreeEd_PreviewFloor.AnimTreeEd_PreviewFloor";
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

bool ParticleSystem::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
	XMLFile xmlFile;
	if (!xmlFile.LoadStream(stream))
	{
		FLAGGG_LOG_ERROR("Failed to load particle system asset[%s].", GetName().CString());
		return false;
	}

	auto* cache = GetSubsystem<ResourceCache>();

	XMLElement root = xmlFile.GetRoot();
	if (root)
	{
		for (XMLElement particleEmitterNode = root.GetChild("ParticleEmitter"); particleEmitterNode; particleEmitterNode = particleEmitterNode.GetNext())
		{
			auto emitter = MakeShared<ParticleSpriteEmitter>();
			emitters_.Push(emitter);

			emitter->CreateLODLevel(0);
			auto* LODLevel = emitter->GetLODLevel(0);

			for (XMLElement particleModuleNode = particleEmitterNode.GetChild("module"); particleModuleNode; particleModuleNode = particleModuleNode.GetNext())
			{
				const String moduleType = particleModuleNode.GetAttribute("type");
				if (moduleType == "ParticleModuleRequired")
				{
					LODLevel->requiredModule_ = MakeShared<ParticleModuleRequired>();

					if (XMLElement materialNode = particleModuleNode.GetChild("material"))
					{
						auto material = new Material();
						material->CreateShaderParameters();

						if (XMLElement textureNode = materialNode.GetChild("texture"))
						{
							const UInt32 unit = textureNode.GetUInt("unit");
							const String texPath = textureNode.GetAttribute("value");
							auto tex = cache->GetResource<Texture2D>(texPath);
							material->SetTexture((TextureClass)0, tex);
						}

						for (XMLElement shaderParamNode = materialNode.GetChild("parameter"); shaderParamNode; shaderParamNode = shaderParamNode.GetNext())
						{
							const String name = shaderParamNode.GetAttribute("name");
							const String type = shaderParamNode.GetAttribute("type");
							if (type == "Float")
							{
								const float value = shaderParamNode.GetFloat("value");
								material->GetShaderParameters()->AddParametersDefine<float>(name);
								material->GetShaderParameters()->SetValue<float>(name, value);
							}
							else if (type == "Vector4")
							{
								const Vector4 value = shaderParamNode.GetVector4("value");
								material->GetShaderParameters()->AddParametersDefine<Vector4>(name);
								material->GetShaderParameters()->SetValue<Vector4>(name, value);
							}
						}

						auto& renderPass = material->GetRenderPass()[RENDER_PASS_TYPE_FORWARD_ALPHA];
						renderPass.SetDepthWrite(false);
					}

					if (XMLElement useLocalSpaceNode = particleModuleNode.GetChild("useLocalSpace"))
					{
						LODLevel->requiredModule_->useLocalSpace_ = useLocalSpaceNode.GetBool("value");
					}

					if (XMLElement screenAlignmentNode = particleModuleNode.GetChild("screenAlignment"))
					{
						LODLevel->requiredModule_->screenAlignment_= ParticleScreenAlignment(screenAlignmentNode.GetBool("value") + 1);
					}

					if (XMLElement emitterDurationNode = particleModuleNode.GetChild("emitterDuration"))
					{
						LODLevel->requiredModule_->emitterDuration_ = emitterDurationNode.GetFloat("value");
					}

					if (XMLElement emitterLoopsNode = particleModuleNode.GetChild("emitterLoops"))
					{
						LODLevel->requiredModule_->emitterLoops_ = emitterLoopsNode.GetFloat("value");
					}

					if (XMLElement emitterDelayNode = particleModuleNode.GetChild("emitterDelay"))
					{
						LODLevel->requiredModule_->emitterDelay_ = emitterDelayNode.GetFloat("value");
					}

					if (XMLElement preWarmNode = particleModuleNode.GetChild("preWarm"))
					{
						
					}

					if (XMLElement interpolationMethodNode = particleModuleNode.GetChild("interpolationMethod"))
					{
						LODLevel->requiredModule_->interpolationMethod_ = ParticleSubUVInterpMethod(interpolationMethodNode.GetUInt("value"));
					}

					if (XMLElement subImagesHorizontalNode = particleModuleNode.GetChild("subImagesHorizontal"))
					{
						LODLevel->requiredModule_->subImages_Horizontal_ = subImagesHorizontalNode.GetUInt("value");
					}

					if (XMLElement subImagesVerticalNode = particleModuleNode.GetChild("subImagesVertical"))
					{
						LODLevel->requiredModule_->subImages_Vertical_ = subImagesVerticalNode.GetUInt("value");
					}

					if (XMLElement randomImageChangesNode = particleModuleNode.GetChild("randomImageChanges"))
					{
						LODLevel->requiredModule_->randomImageChanges_ = randomImageChangesNode.GetUInt("value");
					}

					if (XMLElement randomImageTimeNode = particleModuleNode.GetChild("randomImageTime"))
					{
						LODLevel->requiredModule_->randomImageTime_ = randomImageTimeNode.GetFloat("value");
					}

					if (XMLElement killOnDeactivateNode = particleModuleNode.GetChild("killOnDeactivate"))
					{
						LODLevel->requiredModule_->killOnDeactivate_ = killOnDeactivateNode.GetBool("value");
					}
				}
				else if (moduleType == "ParticleModuleSpawn")
				{
					LODLevel->spawnModule_ = MakeShared<ParticleModuleSpawn>();

					if (XMLElement rateNode = particleModuleNode.GetChild("rate"))
					{
						const String curveType = rateNode.GetAttribute("type");
						if (curveType == "constant")
						{
							auto constantCurve = MakeShared<DistributionFloatConstant>();
							constantCurve->SetKeyOut(0, 0, rateNode.GetChild("constant").GetFloat("value"));
							LODLevel->spawnModule_->rate_.distribution_ = constantCurve;
						}
					}

					if (XMLElement burstListNode = particleModuleNode.GetChild("burstList"))
					{
						for (XMLElement pointNode = burstListNode.GetChild("point"); pointNode; pointNode = pointNode.GetNext())
						{
							auto& burst = LODLevel->spawnModule_->burstList_.EmplaceBack();
							burst.time_ = pointNode.GetFloat("time");
							burst.count_ = pointNode.GetInt("count");
						}
					}
				}
				else if (moduleType == "ParticleModuleLifetime")
				{
					auto lifetimeModule = MakeShared<ParticleModuleLifetime>();
					LODLevel->modules_.Push(lifetimeModule);
					if (XMLElement lifetimeNode = particleModuleNode.GetChild("lifetime"))
					{
						const String curveType = lifetimeNode.GetAttribute("type");
						if (curveType == "uniform")
						{
							auto uniformCurve = MakeShared<DistributionFloatUniform>();
							uniformCurve->min_ = lifetimeNode.GetChild("min").GetFloat("value");
							uniformCurve->max_ = lifetimeNode.GetChild("max").GetFloat("value");
							lifetimeModule->lifetime_.distribution_ = uniformCurve;
						}
					}
				}
				else if (moduleType == "ParticleModuleSize")
				{
					auto sizeModule = MakeShared<ParticleModuleSize>();
					LODLevel->modules_.Push(sizeModule);
					if (XMLElement startSizeNode = particleModuleNode.GetChild("startSize"))
					{
						const String curveType = startSizeNode.GetAttribute("type");
						if (curveType == "uniform")
						{
							auto uniformCurve = MakeShared<DistributionVectorUniform>();
							uniformCurve->min_ = startSizeNode.GetChild("min").GetVector3("value");
							uniformCurve->max_ = startSizeNode.GetChild("max").GetVector3("value");
							sizeModule->startSize_.distribution_ = uniformCurve;
						}
					}
				}
				else if (moduleType == "ParticleModuleSizeMultiplyLife")
				{
					auto sizeModule = MakeShared<ParticleModuleSizeMultiplyLife>();
					LODLevel->modules_.Push(sizeModule);
					if (XMLElement startSizeNode = particleModuleNode.GetChild("startSize"))
					{
						const String curveType = startSizeNode.GetAttribute("type");
						// TODO
					}
				}
				else if (moduleType == "ParticleModuleColorOverLife")
				{
					auto colorModule = MakeShared<ParticleModuleColorOverLife>();
					LODLevel->modules_.Push(colorModule);
					if (XMLElement colorOverLifeNode = particleModuleNode.GetChild("colorOverLife"))
					{
						const String curveType = colorOverLifeNode.GetAttribute("type");
						if (curveType == "colorsCurve")
						{
							if (XMLElement colorNode = colorOverLifeNode.GetChild("color"))
							{
								auto colorCurve = MakeShared<DistributionVectorConstantCurve>();
								colorModule->colorOverLife_.distribution_ = colorCurve;
								for (XMLElement pointNode = colorNode.GetChild("point"); pointNode; pointNode = pointNode.GetNext())
								{
									InterpCurvePoint<Vector3> curvePoint;
									curvePoint.inVal_ = pointNode.GetFloat("inVal");
									curvePoint.outVal_ = pointNode.GetVector3("outVal");
									curvePoint.arriveTangent_ = pointNode.GetVector3("arriveTangent");
									curvePoint.leaveTangent_ = pointNode.GetVector3("leaveTangent");
									curvePoint.interpMode_ = InterpCurveMode(pointNode.GetUInt("interpMode"));
									colorCurve->constantCurve_.points_.Push(curvePoint);
								}
							}

							if (XMLElement alphaNode = colorOverLifeNode.GetChild("alpha"))
							{
								auto alphCurve = MakeShared<DistributionFloatConstantCurve>();
								for (XMLElement pointNode = alphaNode.GetChild("point"); pointNode; pointNode = pointNode.GetNext())
								{
									InterpCurvePoint<float> curvePoint;
									curvePoint.inVal_ = pointNode.GetFloat("inVal");
									curvePoint.outVal_ = pointNode.GetFloat("outVal");
									curvePoint.arriveTangent_ = pointNode.GetFloat("arriveTangent");
									curvePoint.leaveTangent_ = pointNode.GetFloat("leaveTangent");
									curvePoint.interpMode_ = InterpCurveMode(pointNode.GetUInt("interpMode"));
									alphCurve->constantCurve_.points_.Push(curvePoint);
								}
							}
						}
					}
				}
				else if (moduleType == "ParticleModuleColorScaleOverLife")
				{
					auto colorModule = MakeShared<ParticleModuleColorScaleOverLife>();
					LODLevel->modules_.Push(colorModule);
					if (XMLElement colorScaleOverLifeNode = particleModuleNode.GetChild("colorScaleOverLife"))
					{
						const String curveType = colorScaleOverLifeNode.GetAttribute("type");
						if (curveType == "multipleCurve")
						{
							// TODO
						}
					}
					
					if (XMLElement alphaScaleOverLife = particleModuleNode.GetChild("alphaScaleOverLife"))
					{
						const String curveType = alphaScaleOverLife.GetAttribute("type");
						if (curveType == "constant")
						{
							auto curve = MakeShared<DistributionFloatConstant>();
							colorModule->alphaScaleOverLife_.distribution_ = curve;
							curve->constant_ = alphaScaleOverLife.GetChild("constant").GetFloat("value");
						}
					}
				}
				else if (moduleType == "ParticleModuleRotation")
				{

				}
				else if (moduleType == "ParticleModuleRotationRate")
				{

				}
				else if (moduleType == "ParticleModuleLocation")
				{

				}
				else if (moduleType == "ParticleModuleOrbit")
				{

				}
				else if (moduleType == "ParticleModuleOrientationAxisLock")
				{

				}
				else if (moduleType == "")
				{

				}
			}
		}
	}

	return true;
}

bool ParticleSystem::EndLoad()
{
	return true;
}

bool ParticleSystem::BeginSave(IOFrame::Buffer::IOBuffer* stream)
{
	// TODO
	return false;
}

bool ParticleSystem::EndSave()
{
	// TODO
	return false;
}

}
