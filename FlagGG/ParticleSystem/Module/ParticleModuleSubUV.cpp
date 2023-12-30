#include "ParticleModuleSubUV.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleLODLevel.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/Module/ParticleModuleRequired.h"
#include "ParticleSystem/Module/ParticleModuleTypeData.h"
#include "Math/Distributions/DistributionFloatConstant.h"
#include "Math/Distributions/DistributionFloatConstantCurve.h"
#include "Core/ObjectFactory.h"

namespace FlagGG
{

REGISTER_TYPE_FACTORY(ParticleModuleSubUV);

/*-----------------------------------------------------------------------------
	ParticleModuleSubUV implementation.
-----------------------------------------------------------------------------*/
ParticleModuleSubUV::ParticleModuleSubUV()
{
	spawnModule_ = true;
	updateModule_ = true;
}

ParticleModuleSubUV::~ParticleModuleSubUV()
{

}

void ParticleModuleSubUV::InitializeDefaults()
{
	if (!subImageIndex_.IsCreated())
	{
		subImageIndex_.distribution_ = MakeShared<DistributionFloatConstant>();
	}
}

void ParticleModuleSubUV::CompileModule(ParticleEmitterBuildInfo& emitterInfo)
{
	ASSERT(emitterInfo.requiredModule_);
	ParticleSubUVInterpMethod interpMethod = (ParticleSubUVInterpMethod)emitterInfo.requiredModule_->interpolationMethod_;
	if (interpMethod == PSUVIM_Linear || interpMethod == PSUVIM_Linear_Blend)
	{
		emitterInfo.subImageIndex_.Initialize(subImageIndex_.distribution_);
	}
}

void ParticleModuleSubUV::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	ASSERT(owner->spriteTemplate_);

	ParticleLODLevel* LODLevel = owner->spriteTemplate_->GetCurrentLODLevel(owner);
	ASSERT(LODLevel);
	// Grab the interpolation method...
	ParticleSubUVInterpMethod interpMethod = (ParticleSubUVInterpMethod)(LODLevel->requiredModule_->interpolationMethod_);
	const Int32 payloadOffset = owner->subUVDataOffset_;
	if ((interpMethod == PSUVIM_None) || (payloadOffset == 0))
	{
		return;
	}

	if (!LODLevel->typeDataModule_ || LODLevel->typeDataModule_->SupportsSubUV())
	{
		SPAWN_INIT;
		{
			Int32 tempOffset = currentOffset;
			currentOffset = payloadOffset;
			PARTICLE_ELEMENT(FullSubUVPayload, subUVPayload);
			currentOffset = tempOffset;

			subUVPayload.imageIndex_ = DetermineImageIndex(owner, offset, &particle, interpMethod, subUVPayload, spawnTime);
		}
	}
}

void ParticleModuleSubUV::Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime)
{
	ASSERT(owner->spriteTemplate_);

	ParticleLODLevel* LODLevel = owner->spriteTemplate_->GetCurrentLODLevel(owner);
	// Grab the interpolation method...
	ParticleSubUVInterpMethod interpMethod =
		(ParticleSubUVInterpMethod)(LODLevel->requiredModule_->interpolationMethod_);
	const Int32 payloadOffset = owner->subUVDataOffset_;
	if ((interpMethod == PSUVIM_None) || (payloadOffset == 0))
	{
		return;
	}

	// Quick-out in case of Random that only uses a single image for the whole lifetime...
	if ((interpMethod == PSUVIM_Random) || (interpMethod == PSUVIM_Random_Blend))
	{
		if (LODLevel->requiredModule_->randomImageChanges_ == 0)
		{
			// Never change the random image...
			return;
		}
	}

	if (!LODLevel->typeDataModule_ || LODLevel->typeDataModule_->SupportsSubUV())
	{
		BEGIN_UPDATE_LOOP;
		if (particle.relativeTime_ > 1.0f)
		{
			CONTINUE_UPDATE_LOOP;
		}

		Int32 TempOffset = currentOffset;
		currentOffset = payloadOffset;
		PARTICLE_ELEMENT(FullSubUVPayload, subUVPayload);
		currentOffset = TempOffset;

		subUVPayload.imageIndex_ = DetermineImageIndex(owner, offset, &particle, interpMethod, subUVPayload, deltaTime);
		END_UPDATE_LOOP;
	}
}

float ParticleModuleSubUV::DetermineImageIndex(ParticleEmitterInstance* owner, Int32 offset, BaseParticle* particle,
	ParticleSubUVInterpMethod interpMethod, FullSubUVPayload& subUVPayload, float deltaTime)
{
	ParticleLODLevel* LODLevel = owner->spriteTemplate_->GetCurrentLODLevel(owner);
	ASSERT(LODLevel);

	SubUVAnimation* RESTRICT subUVAnimation = owner->spriteTemplate_->subUVAnimation_;

	const Int32 totalSubImages = subUVAnimation
		? subUVAnimation->subImages_Horizontal_ * subUVAnimation->subImages_Vertical_
		: LODLevel->requiredModule_->subImages_Horizontal_ * LODLevel->requiredModule_->subImages_Vertical_;

	float imageIndex = subUVPayload.imageIndex_;

	if ((interpMethod == PSUVIM_Linear) || (interpMethod == PSUVIM_Linear_Blend))
	{
		if (useRealTime_ == false)
		{
			imageIndex = subImageIndex_.GetValue(particle->relativeTime_, owner->component_);
		}
		else
		{
			// TODO:
			//World* World = owner->component_->GetWorld();
			//if ((World != NULL) && (World->GetWorldSettings() != NULL))
			//{
			//	imageIndex = subImageIndex_.GetValue(particle->relativeTime_ / World->GetWorldSettings()->GetEffectiveTimeDilation(), Owner->Component);
			//}
			//else
			{
				imageIndex = subImageIndex_.GetValue(particle->relativeTime_, owner->component_);
			}
		}

		if (interpMethod == PSUVIM_Linear)
		{
			imageIndex = Floor(imageIndex);
		}
	}
	else if ((interpMethod == PSUVIM_Random) || (interpMethod == PSUVIM_Random_Blend))
	{
		if ((LODLevel->requiredModule_->randomImageTime_ == 0.0f) ||
			((particle->relativeTime_ - subUVPayload.randomImageTime_) > LODLevel->requiredModule_->randomImageTime_) ||
			(subUVPayload.randomImageTime_ == 0.0f))
		{
			imageIndex = GetRandomStream(owner).RandHelper(totalSubImages);
			subUVPayload.randomImageTime_ = particle->relativeTime_;
		}

		if (interpMethod == PSUVIM_Random)
		{
			imageIndex = Floor(imageIndex);
		}
	}
	else
	{
		imageIndex = 0;
	}

	return imageIndex;
}

void ParticleModuleSubUV::SetToSensibleDefaults(ParticleEmitter* owner)
{
	subImageIndex_.distribution_ = MakeShared<DistributionFloatConstantCurve>();
	DistributionFloatConstantCurve* subImageIndexDist = RTTICast<DistributionFloatConstantCurve>(subImageIndex_.distribution_);
	if (subImageIndexDist)
	{
		// Add two points, one at time 0.0f and one at 1.0f
		for (Int32 Key = 0; Key < 2; Key++)
		{
			Int32 KeyIndex = subImageIndexDist->CreateNewKey(Key * 1.0f);
			subImageIndexDist->SetKeyOut(0, KeyIndex, 0.0f);
		}
		subImageIndexDist->isDirty_ = true;
	}
}

bool ParticleModuleSubUV::LoadXML(const XMLElement& root)
{


	return true;
}

bool ParticleModuleSubUV::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}

}
