#include "ParticleModuleOrbit.h"
#include "Math/Distributions/DistributionVectorUniform.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleSystemComponent.h"

namespace FlagGG
{

/*-----------------------------------------------------------------------------
	ParticleModuleOrbit implementation.
-----------------------------------------------------------------------------*/
ParticleModuleOrbit::ParticleModuleOrbit()
{
	spawnModule_ = true;
	updateModule_ = true;
	chainMode_ = EOChainMode_Add;
}

void ParticleModuleOrbit::InitializeDefaults()
{
	if (!offsetAmount_.IsCreated())
	{
		SharedPtr<DistributionVectorUniform> DistributionOffsetAmount = MakeShared<DistributionVectorUniform>();
		DistributionOffsetAmount->min_ = Vector3(0.0f, 0.0f, 0.0f);
		DistributionOffsetAmount->max_ = Vector3(0.0f, 50.0f, 0.0f);
		offsetAmount_.distribution_ = DistributionOffsetAmount;
	}

	if (!rotationAmount_.IsCreated())
	{
		SharedPtr<DistributionVectorUniform> DistributionRotationAmount = MakeShared<DistributionVectorUniform>();
		DistributionRotationAmount->min_ = Vector3(0.0f, 0.0f, 0.0f);
		DistributionRotationAmount->max_ = Vector3(1.0f, 1.0f, 1.0f);
		rotationAmount_.distribution_ = DistributionRotationAmount;
	}

	if (!rotationRateAmount_.IsCreated())
	{
		SharedPtr<DistributionVectorUniform> DistributionRotationRateAmount = MakeShared<DistributionVectorUniform>();
		DistributionRotationRateAmount->min_ = Vector3(0.0f, 0.0f, 0.0f);
		DistributionRotationRateAmount->max_ = Vector3(1.0f, 1.0f, 1.0f);
		rotationRateAmount_.distribution_ = DistributionRotationRateAmount;
	}
}

void ParticleModuleOrbit::CompileModule(ParticleEmitterBuildInfo& emitterInfo)
{
	switch (chainMode_)
	{
	case EOChainMode_Add:
		{
			emitterInfo.orbitOffset_.AddDistribution(offsetAmount_.distribution_);
			emitterInfo.orbitInitialRotation_.AddDistribution(rotationAmount_.distribution_);
			emitterInfo.orbitRotationRate_.AddDistribution(rotationRateAmount_.distribution_);
		}
		break;

	case EOChainMode_Scale:
		{
			emitterInfo.orbitOffset_.ScaleByVectorDistribution(offsetAmount_.distribution_);
			emitterInfo.orbitInitialRotation_.ScaleByVectorDistribution(rotationAmount_.distribution_);
			emitterInfo.orbitRotationRate_.ScaleByVectorDistribution(rotationRateAmount_.distribution_);
		}
		break;

	case EOChainMode_Link:
		{
			emitterInfo.orbitOffset_.Initialize(offsetAmount_.distribution_);
			emitterInfo.orbitInitialRotation_.Initialize(rotationAmount_.distribution_);
			emitterInfo.orbitRotationRate_.Initialize(rotationRateAmount_.distribution_);
		}
		break;
	}
}

void ParticleModuleOrbit::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	SPAWN_INIT;
	{
		PARTICLE_ELEMENT(OrbitChainModuleInstancePayload, OrbitPayload);

		if (offsetOptions_.processDuringSpawn_ == true)
		{
			// Process the offset
			Vector3 localOffset;
			if (offsetOptions_.useEmitterTime_ == false)
			{
				localOffset = offsetAmount_.GetValue(particle.relativeTime_, owner->component_);
			}
			else
			{
				localOffset = offsetAmount_.GetValue(owner->emitterTime_, owner->component_);
			}
			OrbitPayload.baseOffset_ += localOffset;
			OrbitPayload.previousOffset_ = OrbitPayload.offset_;
			OrbitPayload.offset_ += localOffset;
		}

		if (rotationOptions_.processDuringSpawn_ == true)
		{
			// Process the rotation
			Vector3 localRotation;
			if (rotationOptions_.useEmitterTime_ == false)
			{
				localRotation = rotationAmount_.GetValue(particle.relativeTime_, owner->component_);
			}
			else
			{
				localRotation = rotationAmount_.GetValue(owner->emitterTime_, owner->component_);
			}
			OrbitPayload.rotation_ += localRotation;
		}

		if (rotationRateOptions_.processDuringSpawn_ == true)
		{
			// Process the rotation rate
			Vector3 localRotationRate;
			if (rotationRateOptions_.useEmitterTime_ == false)
			{
				localRotationRate = rotationRateAmount_.GetValue(particle.relativeTime_, owner->component_);
			}
			else
			{
				localRotationRate = rotationRateAmount_.GetValue(owner->emitterTime_, owner->component_);
			}
			OrbitPayload.baseRotationRate_ += localRotationRate;
			OrbitPayload.rotationRate_ += localRotationRate;
		}
	}
}

void ParticleModuleOrbit::Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime)
{
	BEGIN_UPDATE_LOOP;
	{
		PARTICLE_ELEMENT(OrbitChainModuleInstancePayload, orbitPayload);

		if (offsetOptions_.processDuringUpdate_ == true)
		{
			// Process the offset
			Vector3 localOffset;
			if (offsetOptions_.useEmitterTime_ == false)
			{
				localOffset = offsetAmount_.GetValue(particle.relativeTime_, owner->component_);
			}
			else
			{
				localOffset = offsetAmount_.GetValue(owner->emitterTime_, owner->component_);
			}

			//@todo. Do we need to update the base offset here???
//			orbitPayload.BaseOffset += LocalOffset;
			orbitPayload.previousOffset_ = orbitPayload.offset_;
			orbitPayload.offset_ += localOffset;
		}

		if (rotationOptions_.processDuringUpdate_ == true)
		{
			// Process the rotation
			Vector3 localRotation;
			if (rotationOptions_.useEmitterTime_ == false)
			{
				localRotation = rotationAmount_.GetValue(particle.relativeTime_, owner->component_);
			}
			else
			{
				localRotation = rotationAmount_.GetValue(owner->emitterTime_, owner->component_);
			}
			orbitPayload.rotation_ += localRotation;
		}


		if (rotationRateOptions_.processDuringUpdate_ == true)
		{
			// Process the rotation rate
			Vector3 localRotationRate;
			if (rotationRateOptions_.useEmitterTime_ == false)
			{
				localRotationRate = rotationRateAmount_.GetValue(particle.relativeTime_, owner->component_);
			}
			else
			{
				localRotationRate = rotationRateAmount_.GetValue(owner->emitterTime_, owner->component_);
			}
			//@todo. Do we need to update the base rotationrate here???
//			OrbitPayload.BaseRotationRate += LocalRotationRate;
			orbitPayload.rotationRate_ += localRotationRate;
		}
	}
	END_UPDATE_LOOP;
}

UInt32 ParticleModuleOrbit::RequiredBytes(ParticleModuleTypeDataBase* typeData)
{
	return sizeof(OrbitChainModuleInstancePayload);
}

UInt32 ParticleModuleOrbit::RequiredBytesPerInstance()
{
	return 0;
}

}
