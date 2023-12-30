#include "ParticleModuleRotationRate.h"
#include "Math/Distributions/DistributionFloatConstant.h"
#include "Math/Distributions/DistributionFloatUniform.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "Core/ObjectFactory.h"

namespace FlagGG
{

REGISTER_TYPE_FACTORY(ParticleModuleRotationRate);

/*-----------------------------------------------------------------------------
	ParticleModuleRotationRate implementation.
-----------------------------------------------------------------------------*/
ParticleModuleRotationRate::ParticleModuleRotationRate()
{
	spawnModule_ = true;
}

void ParticleModuleRotationRate::InitializeDefaults()
{
	if (!startRotationRate_.IsCreated())
	{
		startRotationRate_.distribution_ = MakeShared<DistributionFloatConstant>();
	}
}

void ParticleModuleRotationRate::CompileModule(ParticleEmitterBuildInfo& emitterInfo)
{
	float minRate;
	float maxRate;

	// Call GetValue once to ensure the distribution has been initialized.
	startRotationRate_.GetValue();
	startRotationRate_.GetOutRange(minRate, maxRate);
	emitterInfo.maxRotationRate_ = maxRate;
	emitterInfo.spawnModules_.Push(this);
}

void ParticleModuleRotationRate::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	SpawnEx(owner, offset, spawnTime, &GetRandomStream(owner), particleBase);
}

void ParticleModuleRotationRate::SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, struct RandomStream* inRandomStream, BaseParticle* particleBase)
{
	SPAWN_INIT;
	{
		float startRotRate = (PI / 180.f) * 360.0f * startRotationRate_.GetValue(owner->emitterTime_, owner->component_, inRandomStream);
		particle.rotationRate_ += startRotRate;
		particle.baseRotationRate_ += startRotRate;
	}
}

void ParticleModuleRotationRate::SetToSensibleDefaults(ParticleEmitter* owner)
{
	auto startRotationRateDist = MakeShared<DistributionFloatUniform>();
	startRotationRateDist->min_ = 0.0f;
	startRotationRateDist->max_ = 1.0f;
	startRotationRateDist->isDirty_ = true;
	startRotationRate_.distribution_ = startRotationRateDist;
}

bool ParticleModuleRotationRate::LoadXML(const XMLElement& root)
{
	if (XMLElement startRotationRateNode = root.GetChild("startRotationRate"))
	{
		const String curveType = startRotationRateNode.GetAttribute("type");
		if (curveType == "uniform")
		{
			auto uniformCurve = MakeShared<DistributionFloatUniform>();
			uniformCurve->min_ = startRotationRateNode.GetChild("min").GetFloat("value");
			uniformCurve->max_ = startRotationRateNode.GetChild("max").GetFloat("value");
			startRotationRate_.distribution_ = uniformCurve;
		}

		return true;
	}

	return false;
}

bool ParticleModuleRotationRate::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}

}
