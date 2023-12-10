#include "ParticleModuleVelocity.h"
#include "Math/Distributions/DistributionFloatUniform.h"
#include "Math/Distributions/DistributionVectorUniform.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/ParticleLODLevel.h"
#include "ParticleSystem/ParticleEmitter.h"

namespace FlagGG
{

/*-----------------------------------------------------------------------------
	Abstract base modules used for categorization.
-----------------------------------------------------------------------------*/
ParticleModuleVelocityBase::ParticleModuleVelocityBase()
{
}

/*-----------------------------------------------------------------------------
	ParticleModuleVelocity implementation.
-----------------------------------------------------------------------------*/

ParticleModuleVelocity::ParticleModuleVelocity()
{
	spawnModule_ = true;
}

void ParticleModuleVelocity::InitializeDefaults()
{
	if (!startVelocity_.IsCreated())
	{
		startVelocity_.distribution_ = MakeShared<DistributionVectorUniform>();
	}

	if (!startVelocityRadial_.IsCreated())
	{
		startVelocityRadial_.distribution_ = MakeShared<DistributionFloatUniform>();
	}
}

void ParticleModuleVelocity::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	SpawnEx(owner, offset, spawnTime, &GetRandomStream(owner), particleBase);
}

void ParticleModuleVelocity::SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, RandomStream* inRandomStream, BaseParticle* particleBase)
{
	SPAWN_INIT;
	{
		Vector3 vel = startVelocity_.GetValue(owner->emitterTime_, owner->component_, 0, inRandomStream);
		Vector3 FromOrigin = (particle.location_ - owner->emitterToSimulation_.Translation()).Normalized();

		Vector3 ownerScale(Vector3::ZERO);
		if ((applyOwnerScale_ == true) && owner->component_)
		{
			ownerScale = owner->component_->GetNode()->GetWorldScale();
		}

		ParticleLODLevel* LODLevel	= owner->spriteTemplate_->GetCurrentLODLevel(owner);
		ASSERT(LODLevel);
		if (LODLevel->requiredModule_->useLocalSpace_)
		{
			if (inWorldSpace_ == true)
			{
				vel = owner->simulationToWorld_.Inverse() * vel;
			}
			else
			{
				vel = owner->emitterToSimulation_ * vel;
			}
		}
		else if (inWorldSpace_ == false)
		{
			vel = owner->emitterToSimulation_ * vel;
		}
		vel *= ownerScale;
		vel += FromOrigin * startVelocityRadial_.GetValue(owner->emitterTime_, owner->component_, inRandomStream) * ownerScale;
		particle.velocity_		+= vel;
		particle.baseVelocity_	+= vel;
	}
}

}
