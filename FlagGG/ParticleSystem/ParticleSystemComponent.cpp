#include "ParticleSystemComponent.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleSystemRenderer.h"
#include "ParticleSystem/ParticleLODLevel.h"
#include "Scene/Scene.h"

namespace FlagGG
{

ParticleSystemComponent::ParticleSystemComponent()
{
	//PrimaryComponentTick.bCanEverTick = true;
	//PrimaryComponentTick.TickGroup = TG_DuringPhysics;
	//PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
	//tickInEditor_ = true;
	//maxTimeBeforeForceUpdateTransform_ = 5.0f;
	//autoActivate_ = true;
	//resetOnDetach_ = false;
	//oldPositionValid_ = false;
	//oldPosition_ = Vector::ZERO;

	justRegistered_ = false;
	warmingUp_ = false;

	randomStream_.Initialize(String::EMPTY);

	//partSysVelocity_ = Vector::ZERO;

	warmupTime_ = 0.0f;
	warmupTickRate_ = 1.f;
	emitterDelay_ = 0.f;
	//secondsBeforeInactive_ = 1.0f;
	//isTransformDirty_ = false;
	//skipUpdateDynamicDataDuringTick_ = false;
	//isViewRelevanceDirty_ = true;
	//customTimeDilation_ = 1.0f;
	//allowConcurrentTick_ = true;
	//asyncWorkOutstanding_ = false;
	//poolingMethod_ = PSCPoolMethod::None;
	//wasActive_ = false;
#if WITH_EDITORONLY_DATA
	editorDetailMode_ = -1;
#endif // WITH_EDITORONLY_DATA
	//SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	//SetGenerateOverlapEvents(false);

	//castVolumetricTranslucentShadow_ = true;

	// Disable receiving decals by default.
	//receivesDecals_ = false;

	// Don't need to call OnUpdateTransform, no physics state to update
	//wantsOnUpdateTransform_ = false;

	//savedAutoAttachRelativeScale3D_ = Vector::ONE;
	//timeSinceLastTick_ = 0;

	//requiredSignificance_ = ParticleSignificanceLevel::Low;
	//lastSignificantTime_ = 0.0f;
	//isManagingSignificance_ = 0;
	//wasManagingSignificance_ = 0;

	//managerHandle_ = INDEX_NONE;
	//pendingManagerAdd_ = false;
	//pendingManagerRemove_ = false;

	//excludeFromLightAttachmentGroup_ = true;

	LODLevel_ = 0;
}

ParticleSystemComponent::~ParticleSystemComponent()
{
	for (auto* instance : emitterInstances_)
	{
		delete instance;
	}
	emitterInstances_.Clear();
}

void ParticleSystemComponent::InitializeSystem()
{
	// System settings may have been lowered. Support late deactivation.
	const bool detailModeAllowsRendering = /*detailMode_ <= GetCurrentDetailMode()*/true;

	if (detailModeAllowsRendering)
	{
		if (template_)
		{
			emitterDelay_ = template_->delay_;

			if (template_->useDelayRange_)
			{
				const float	rand = randomStream_.FRand();
				emitterDelay_ = template_->delayLow_ + ((template_->delay_ - template_->delayLow_) * rand);
			}
		}

		// Allocate the emitter instances and particle data
		InitParticles();
	}
}

void ParticleSystemComponent::InitParticles()
{
	if (!template_)
		return;

	warmupTime_ = template_->warmupTime_;
	warmupTickRate_ = template_->warmupTickRate_;
	// isViewRelevanceDirty_ = true;
	const Int32 globalDetailMode = GetCurrentDetailMode();
	const bool canEverRender = /*CanEverRender()*/true;

	//simplified version.
	Int32 numInstances = emitterInstances_.Size();
	Int32 numEmitters = template_->emitters_.Size();
	const bool isFirstCreate = numInstances == 0;
	emitterInstances_.Resize(numEmitters);

	wasCompleted_ = isFirstCreate ? false : wasCompleted_;

	bool clearDynamicData = false;
	Int32 preferredLODLevel = LODLevel_;
	bool setLodLevels = LODLevel_ > 0; //We should set the lod level even when creating all emitters if the requested LOD is not 0. 

	for (Int32 idx = 0; idx < numEmitters; idx++)
	{
		ParticleEmitter* emitter = template_->emitters_[idx];
		if (emitter)
		{
			ParticleEmitterInstance* instance = numInstances == 0 ? NULL : emitterInstances_[idx];
			ASSERT(globalDetailMode < NUM_DETAILMODE_FLAGS);
			const bool detailModeAllowsRendering = /*detailMode_ <= globalDetailMode &&*/ /*(emitter->detailModeBitmask_ & (1 << globalDetailMode))*/ true;
			const bool shouldCreateAndOrInit = detailModeAllowsRendering && emitter->HasAnyEnabledLODs() && canEverRender;

			if (shouldCreateAndOrInit)
			{
				if (instance)
				{
					instance->SetHaltSpawning(false);
					instance->SetHaltSpawningExternal(false);
				}
				else
				{
					instance = emitter->CreateInstance(this);
					emitterInstances_[idx] = instance;
				}

				if (instance)
				{
					instance->enabled_ = true;
					instance->InitParameters(emitter, this);
					instance->Init();

					preferredLODLevel = Min<Int32>(preferredLODLevel, emitter->LODLevels_.Size());
					setLodLevels |= !isFirstCreate;//Only set lod levels if we init any instances and it's not the first creation time.
				}
			}
			else
			{
				if (instance)
				{
					delete instance;
					emitterInstances_[idx] = NULL;
					clearDynamicData = true;
				}
			}
		}
	}

	if (clearDynamicData)
	{
		// ClearDynamicData();
	}

	if (setLodLevels)
	{
		if (preferredLODLevel != LODLevel_)
		{
			// This should never be higher...
			ASSERT(preferredLODLevel < LODLevel_);
			LODLevel_ = preferredLODLevel;
		}

		for (Int32 Idx = 0; Idx < emitterInstances_.Size(); Idx++)
		{
			ParticleEmitterInstance* instance = emitterInstances_[Idx];
			// set the LOD levels here
			if (instance)
			{
				instance->currentLODLevelIndex_ = LODLevel_;

				// small safety net for OR-11322; can be removed if the ensure never fires after the change in SetTemplate (reset all instances LOD indices to 0)
				if (instance->currentLODLevelIndex_ >= instance->spriteTemplate_->LODLevels_.Size())
				{
					instance->currentLODLevelIndex_ = instance->spriteTemplate_->LODLevels_.Size() - 1;
					CRY_ENSURE(false, "LOD access out of bounds (OR-11322). Please let olaf.piesche or simon.tovey know.");
				}
				instance->currentLODLevel_ = instance->spriteTemplate_->LODLevels_[instance->currentLODLevelIndex_];
			}
		}
	}
}

void ParticleSystemComponent::ResetParticles(bool clearInstances)
{
	// TODO: 区分编辑器和游戏场景
	const bool isGameWorld = true;

	// Remove instances if we're not running gameplay
	if (!isGameWorld || clearInstances)
	{
		for (Int32 instanceIndex = 0; instanceIndex < emitterInstances_.Size(); instanceIndex++)
		{
			ParticleEmitterInstance* emitInst = emitterInstances_[instanceIndex];
			if (emitInst)
			{
				delete emitInst;
				emitterInstances_[instanceIndex] = nullptr;
			}
		}
		emitterInstances_.Clear();
		// ClearDynamicData();
	}
	else
	{
		for (Int32 instanceIndex = 0; instanceIndex < emitterInstances_.Size(); instanceIndex++)
		{
			ParticleEmitterInstance* emitInst = emitterInstances_[instanceIndex];
			if (emitInst)
			{
				emitInst->Rewind();
			}
		}
	}
}

void ParticleSystemComponent::ResetBurstLists()
{
	for (Int32 i = 0; i < emitterInstances_.Size(); i++)
	{
		if (emitterInstances_[i])
		{
			emitterInstances_[i]->ResetBurstList();
		}
	}
}

Int32 ParticleSystemComponent::GetCurrentDetailMode() const
{
	return -1;
}

bool ParticleSystemComponent::GetFloatParameter(const String& inName,float& outFloat)
{
	// Always fail if we pass in no name.
	if (inName.Empty())
	{
		return false;
	}

	for (Int32 i = 0; i < instanceParameters_.Size(); i++)
	{
		const ParticleSysParam& param = instanceParameters_[i];
		if (param.name_ == inName)
		{
			if (param.paramType_ == PSPT_Scalar)
			{
				outFloat = param.scalar_;
				return true;
			}
			else if (param.paramType_ == PSPT_ScalarRand)
			{
				outFloat = param.scalar_ + (param.scalar_Low_ - param.scalar_) * randomStream_.FRand();
				return true;
			}
		}
	}

	return false;
}


bool ParticleSystemComponent::GetVectorParameter(const String& inName,Vector3& outVector)
{
	// Always fail if we pass in no name.
	if (inName.Empty())
	{
		return false;
	}

	for (Int32 i = 0; i < instanceParameters_.Size(); i++)
	{
		const ParticleSysParam& param = instanceParameters_[i];
		if (param.name_ == inName)
		{
			if (param.paramType_ == PSPT_Vector)
			{
				outVector = param.vector_;
				return true;
			}
			else if (param.paramType_ == PSPT_VectorRand)
			{
				Vector3 randValue(randomStream_.FRand(), randomStream_.FRand(), randomStream_.FRand());
				outVector = param.vector_ + (param.vector_Low_ - param.vector_) * randValue;
				return true;
			}
			else if (param.paramType_ == PSPT_VectorUnitRand)
			{
				return true;
			}
		}
	}

	return false;
}

bool ParticleSystemComponent::GetAnyVectorParameter(const String& inName, Vector3& outVector)
{
	// Always fail if we pass in no name.
	if (inName.Empty())
	{
		return false;
	}

	for (Int32 i = 0; i < instanceParameters_.Size(); i++)
	{
		const ParticleSysParam& param = instanceParameters_[i];
		if (param.name_ == inName)
		{
			if (param.paramType_ == PSPT_Vector)
			{
				outVector = param.vector_;
				return true;
			}
			if (param.paramType_ == PSPT_VectorRand)
			{
				Vector3 randValue(randomStream_.FRand(), randomStream_.FRand(), randomStream_.FRand());
				outVector = param.vector_ + (param.vector_Low_ - param.vector_) * randValue;
				return true;
			}
			else if (param.paramType_ == PSPT_VectorUnitRand)
			{
				return true;
			}
			if (param.paramType_ == PSPT_Scalar)
			{
				float OutFloat = param.scalar_;
				outVector = Vector3(OutFloat, OutFloat, OutFloat);
				return true;
			}
			if (param.paramType_ == PSPT_ScalarRand)
			{
				float OutFloat = param.scalar_ + (param.scalar_Low_ - param.scalar_) * randomStream_.FRand();
				outVector = Vector3(OutFloat, OutFloat, OutFloat);
				return true;
			}
			if (param.paramType_ == PSPT_Color)
			{
				outVector = Vector3(param.color_.r_, param.color_.g_, param.color_.b_);
				return true;
			}
		}
	}

	return false;
}


bool ParticleSystemComponent::GetColorParameter(const String& inName, Color& outColor)
{
	// Always fail if we pass in no name.
	if (inName.Empty())
	{
		return false;
	}

	for (Int32 i = 0; i < instanceParameters_.Size(); i++)
	{
		const ParticleSysParam& param = instanceParameters_[i];
		if (param.name_ == inName && param.paramType_ == PSPT_Color)
		{
			outColor = param.color_;
			return true;
		}
	}

	return false;
}


bool ParticleSystemComponent::GetNodeParameter(const String& inName, Node*& outNode)
{
	// Always fail if we pass in no name.
	if (inName.Empty())
	{
		return false;
	}

	for (Int32 i = 0; i < instanceParameters_.Size(); i++)
	{
		const ParticleSysParam& param = instanceParameters_[i];
		if (param.name_ == inName && param.paramType_ == PSPT_Node)
		{
			outNode = param.node_;
			return true;
		}
	}

	return false;
}


bool ParticleSystemComponent::GetMaterialParameter(const String& inName, Material*& outMaterial)
{
	// Always fail if we pass in no name.
	if (inName.Empty())
	{
		return false;
	}

	for (Int32 i = 0; i < instanceParameters_.Size(); i++)
	{
		const ParticleSysParam& param = instanceParameters_[i];
		if (param.name_ == inName && param.paramType_ == PSPT_Material)
		{
			outMaterial = param.material_;
			return true;
		}
	}

	return false;
}

void ParticleSystemComponent::OnUpdateWorldBoundingBox()
{

}

void ParticleSystemComponent::Update(Real timeStep)
{
	for (auto* emitterInst : emitterInstances_)
	{
		if (emitterInst)
		{
			emitterInst->Tick(timeStep, false);
		}
	}

	auto* particleSystemRenderer = ownerScene_->GetComponent<ParticleSystemRenderer>();
	if (!particleSystemRenderer)
	{
		particleSystemRenderer = ownerScene_->CreateComponent<ParticleSystemRenderer>();
	}

	particleSystemRenderer->PostToRenderUpdateQueue(this);
}

void ParticleSystemComponent::RenderUpdate(const RenderPiplineContext* renderPiplineContext, ParticleMeshDataBuilder* particleMeshDataBuilder)
{
	for (auto* emitterInst : emitterInstances_)
	{
		if (emitterInst && emitterInst->spriteTemplate_)
		{
			auto currentLODLevel = emitterInst->spriteTemplate_->GetCurrentLODLevel(emitterInst);

			if (currentLODLevel)
			{
				if (!emitterInst->geometry_)
				{
					emitterInst->geometry_ = new Geometry();

					auto& renderContext = renderContexts_.EmplaceBack();
					renderContext.geometryType_ = GEOMETRY_BILLBOARD;
					renderContext.geometry_ = emitterInst->geometry_;
					renderContext.material_ = currentLODLevel->requiredModule_->material_;
					renderContext.numWorldTransform_ = 1;
					renderContext.worldTransform_ = &(node_->GetWorldTransform());
					renderContext.viewMask_ = GetViewMask();
				}

				emitterInst->RenderUpdate(renderPiplineContext, particleMeshDataBuilder);
			}
		}
	}
}

}
