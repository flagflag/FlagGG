#include "ParticleModuleRotation.h"
#include "Math/Distributions/DistributionFloatUniform.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "Core/ObjectFactory.h"

namespace FlagGG
{

REGISTER_TYPE_FACTORY(ParticleModuleRotation);

/*-----------------------------------------------------------------------------
	ParticleModuleRotation implementation.
-----------------------------------------------------------------------------*/
ParticleModuleRotation::ParticleModuleRotation()
{
	spawnModule_ = true;
}

void ParticleModuleRotation::InitializeDefaults()
{
	if (!startRotation_.IsCreated())
	{
		auto distributionStartRotation = MakeShared<DistributionFloatUniform>();
		distributionStartRotation->min_ = 0.0f;
		distributionStartRotation->max_ = 1.0f;
		startRotation_.distribution_ = distributionStartRotation;
	}
}

void ParticleModuleRotation::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	SpawnEx(owner, offset, spawnTime, &GetRandomStream(owner), particleBase);
}

void ParticleModuleRotation::SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, struct RandomStream* inRandomStream, BaseParticle* particleBase)
{
	SPAWN_INIT;
	{
		particle.rotation_ += (PI / 180.f) * 360.0f * startRotation_.GetValue(owner->emitterTime_, owner->component_, inRandomStream);
	}
}

bool ParticleModuleRotation::LoadXML(const XMLElement& root)
{
	if (XMLElement startRotationNode = root.GetChild("startRotation"))
	{
		const String curveType = startRotationNode.GetAttribute("type");

		if (curveType == "uniform")
		{
			auto uniformCurve = MakeShared<DistributionFloatUniform>();
			uniformCurve->min_ = startRotationNode.GetChild("min").GetFloat("value");
			uniformCurve->max_ = startRotationNode.GetChild("max").GetFloat("value");
			startRotation_.distribution_ = uniformCurve;
		}

		return true;
	}

	return false;
}

bool ParticleModuleRotation::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}

}
