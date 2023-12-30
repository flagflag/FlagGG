#include "ParticleModuleAcceleration.h"
#include "Math/Distributions/DistributionVectorUniform.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/ParticleLODLevel.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "Core/ObjectFactory.h"

namespace FlagGG
{

REGISTER_TYPE_FACTORY(ParticleModuleAcceleration);

/*-----------------------------------------------------------------------------
	ParticleModuleAccelerationBase implementation.
-----------------------------------------------------------------------------*/

void ParticleModuleAccelerationBase::SetToSensibleDefaults(ParticleEmitter* owner)
{
	alwaysInWorldSpace_ = true;
	ParticleModule::SetToSensibleDefaults(owner);
}

/*-----------------------------------------------------------------------------
	ParticleModuleAcceleration implementation.
-----------------------------------------------------------------------------*/
ParticleModuleAcceleration::ParticleModuleAcceleration()
{
	spawnModule_ = true;
	updateModule_ = true;
}

void ParticleModuleAcceleration::InitializeDefaults()
{
	if (!acceleration_.IsCreated())
	{
		acceleration_.distribution_ = MakeShared<DistributionVectorUniform>();
	}
}

void ParticleModuleAcceleration::CompileModule(ParticleEmitterBuildInfo& emitterInfo)
{
	emitterInfo.constantAcceleration_ = acceleration_.GetValue();
}

void ParticleModuleAcceleration::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	SPAWN_INIT;
	PARTICLE_ELEMENT(Vector3, usedAcceleration);
	usedAcceleration = acceleration_.GetValue(owner->emitterTime_, owner->component_);
	if ((applyOwnerScale_ == true) && owner && owner->component_)
	{
		Vector3 scale(owner->component_->GetNode()->GetWorldScale());
		usedAcceleration *= scale;
	}
	ParticleLODLevel* LODLevel = owner->spriteTemplate_->GetCurrentLODLevel(owner);
	ASSERT(LODLevel);
	if (alwaysInWorldSpace_ && LODLevel->requiredModule_->useLocalSpace_)
	{
		Vector3 tempUsedAcceleration = owner->component_->GetNode()->GetWorldTransform().Inverse() * usedAcceleration; // World space to local space
		particle.velocity_ += tempUsedAcceleration * spawnTime;
		particle.baseVelocity_ += tempUsedAcceleration * spawnTime;
	}
	else
	{
		if (LODLevel->requiredModule_->useLocalSpace_)
		{
			usedAcceleration = owner->emitterToSimulation_ * usedAcceleration;
		}
		particle.velocity_ += usedAcceleration * spawnTime;
		particle.baseVelocity_ += usedAcceleration * spawnTime;
	}
}

void ParticleModuleAcceleration::Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime)
{
	if ((owner == NULL) || (owner->activeParticles_ <= 0) ||
		(owner->particleData_ == NULL) || (owner->particleIndices_ == NULL))
	{
		return;
	}
	ParticleLODLevel* LODLevel = owner->spriteTemplate_->GetCurrentLODLevel(owner);
	ASSERT(LODLevel);
	// PlatformMisc::Prefetch(owner->particleData_, (owner->particleIndices_[0] * owner->particleStride_));
	// PlatformMisc::Prefetch(owner->particleData_, (owner->particleIndices_[0] * owner->particleStride_) + PLATFORM_CACHE_LINE_SIZE);
	if (alwaysInWorldSpace_ && LODLevel->requiredModule_->useLocalSpace_)
	{
		const Matrix3x4& mat = owner->component_->GetNode()->GetWorldTransform();
		BEGIN_UPDATE_LOOP;
		{
			Vector3& usedAcceleration = *((Vector3*)(particleBase + currentOffset));																\
			Vector3 transformedUsedAcceleration = mat.Inverse() * usedAcceleration;
			// PlatformMisc::Prefetch(particleData, (particleIndices[i + 1] * particleStride));
			// PlatformMisc::Prefetch(particleData, (particleIndices[i + 1] * particleStride) + PLATFORM_CACHE_LINE_SIZE);
			particle.velocity_ += transformedUsedAcceleration * deltaTime;
			particle.baseVelocity_ += transformedUsedAcceleration * deltaTime;
		}
		END_UPDATE_LOOP;
	}
	else
	{
		BEGIN_UPDATE_LOOP;
		{
			Vector3& usedAcceleration = *((Vector3*)(particleBase + currentOffset));																\
			// PlatformMisc::Prefetch(particleData, (particleIndices[i + 1] * particleStride));
			// PlatformMisc::Prefetch(particleData, (particleIndices[i + 1] * particleStride) + PLATFORM_CACHE_LINE_SIZE);
			particle.velocity_ += usedAcceleration * deltaTime;
			particle.baseVelocity_ += usedAcceleration * deltaTime;
		}
		END_UPDATE_LOOP;
	}
}

UInt32 ParticleModuleAcceleration::RequiredBytes(ParticleModuleTypeDataBase* typeData)
{
	// Vector3 usedAcceleration
	return sizeof(Vector3);
}

bool ParticleModuleAcceleration::LoadXML(const XMLElement& root)
{
	if (XMLElement alwaysInWorldSpaceNode = root.GetChild("alwaysInWorldSpace"))
	{
		alwaysInWorldSpace_ = alwaysInWorldSpaceNode.GetBool("value");
	}

	if (XMLElement applyOwnerScaleNode = root.GetChild("applyOwnerScale"))
	{
		applyOwnerScale_ = applyOwnerScaleNode.GetBool("value");
	}

	if (XMLElement acclerationNode = root.GetChild("accleration"))
	{
		const String curveType = acclerationNode.GetAttribute("type");
		if (curveType == "uniform")
		{
			auto uniformCurve = MakeShared<DistributionVectorUniform>();
			uniformCurve->min_ = acclerationNode.GetChild("min").GetVector3("value");
			uniformCurve->max_ = acclerationNode.GetChild("max").GetVector3("value");
			acceleration_.distribution_ = uniformCurve;
		}
	}

	return true;
}

bool ParticleModuleAcceleration::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}

}
