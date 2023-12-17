#include "ParticleModuleSize.h"
#include "Math/Distributions/DistributionVectorUniform.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/ParticleLODLevel.h"

namespace FlagGG
{

/*-----------------------------------------------------------------------------
	Abstract base modules used for categorization.
-----------------------------------------------------------------------------*/
ParticleModuleSizeBase::ParticleModuleSizeBase()
{
}

/*-----------------------------------------------------------------------------
	ParticleModuleSize implementation.
-----------------------------------------------------------------------------*/

ParticleModuleSize::ParticleModuleSize()
{
	spawnModule_ = true;
	updateModule_ = false;
}

void ParticleModuleSize::InitializeDefaults()
{
	if (!startSize_.IsCreated())
	{
		SharedPtr<DistributionVectorUniform> distributionStartSize = MakeShared<DistributionVectorUniform>();
		distributionStartSize->min_ = Vector3(1.0f, 1.0f, 1.0f);
		distributionStartSize->max_ = Vector3(1.0f, 1.0f, 1.0f);
		startSize_.distribution_ = distributionStartSize;
	}
}

void ParticleModuleSize::CompileModule(ParticleEmitterBuildInfo& emitterInfo)
{
	float minSize = 0.0f;
	float maxSize = 0.0f;
	startSize_.GetValue();
	startSize_.GetOutRange(minSize, maxSize);
	emitterInfo.maxSize_.x_ *= maxSize;
	emitterInfo.maxSize_.y_ *= maxSize;
	emitterInfo.spawnModules_.Push(this);
	emitterInfo.sizeScale_.InitializeWithConstant(Vector3(1.0f, 1.0f, 1.0f));
}

void ParticleModuleSize::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	SpawnEx(owner, offset, spawnTime, &GetRandomStream(owner), particleBase);
}

void ParticleModuleSize::SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, struct RandomStream* inRandomStream, BaseParticle* particleBase)
{
	SPAWN_INIT;
	Vector3 size  = startSize_.GetValue(owner->emitterTime_, owner->component_, 0, inRandomStream);
	particle.size_ += size;

	AdjustParticleBaseSizeForUVFlipping(size, owner->currentLODLevel_->requiredModule_->UVFlippingMode_, *inRandomStream);
	particle.baseSize_ += size;
}


}
