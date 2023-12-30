#include "ParticleModuleLifetime.h"
#include "Math/Distributions/DistributionFloatUniform.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "Core/ObjectFactory.h"

namespace FlagGG
{

REGISTER_TYPE_FACTORY(ParticleModuleLifetime);

/*-----------------------------------------------------------------------------
	ParticleModuleLifetimeBase implementation.
-----------------------------------------------------------------------------*/
ParticleModuleLifetimeBase::ParticleModuleLifetimeBase()
{
}

/*-----------------------------------------------------------------------------
	ParticleModuleLifetime implementation.
-----------------------------------------------------------------------------*/

ParticleModuleLifetime::ParticleModuleLifetime()
{
	spawnModule_ = true;
}

void ParticleModuleLifetime::InitializeDefaults()
{
	if(!lifetime_.IsCreated())
	{
		lifetime_.distribution_ = MakeShared<DistributionFloatUniform>();
	}
}

void ParticleModuleLifetime::CompileModule(struct ParticleEmitterBuildInfo& emitterInfo)
{
	float minLifetime;
	float maxLifetime;

	// Call GetValue once to ensure the distribution has been initialized.
	lifetime_.GetValue();
	lifetime_.GetOutRange(minLifetime, maxLifetime);
	emitterInfo.maxLifetime_ = maxLifetime;
	emitterInfo.spawnModules_.Push(this);
}

void ParticleModuleLifetime::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	SpawnEx(owner, offset, spawnTime, &GetRandomStream(owner), particleBase);
}

void ParticleModuleLifetime::SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, RandomStream* inRandomStream, BaseParticle* particleBase)
{
	SPAWN_INIT;
	{
		float maxLifetime = lifetime_.GetValue(owner->emitterTime_, owner->component_, inRandomStream);
		if(particle.oneOverMaxLifetime_ > 0.f)
		{
			// Another module already modified lifetime.
			particle.oneOverMaxLifetime_ = 1.f / (maxLifetime + 1.f / particle.oneOverMaxLifetime_);
		}
		else
		{
			// First module to modify lifetime.
			particle.oneOverMaxLifetime_ = maxLifetime > 0.f ? 1.f / maxLifetime : 0.f;
		}
		//If the relative time is already > 1.0f then we don't want to be setting it. Some modules use this to mark a particle as dead during spawn.
		particle.relativeTime_ = particle.relativeTime_ > 1.0f ? particle.relativeTime_ : spawnTime * particle.oneOverMaxLifetime_;
	}
}

void ParticleModuleLifetime::SetToSensibleDefaults(ParticleEmitter* owner)
{
	DistributionFloatUniform* lifetimeDist = RTTICast<DistributionFloatUniform>(lifetime_.distribution_);
	if (lifetimeDist)
	{
		lifetimeDist->min_ = 1.0f;
		lifetimeDist->max_ = 1.0f;
		lifetimeDist->isDirty_ = true;
	}
}

float ParticleModuleLifetime::GetMaxLifetime()
{
	// Check the distribution for the max value
	float min, max;
	lifetime_.GetOutRange(min, max);
	return max;
}

float ParticleModuleLifetime::GetLifetimeValue(ParticleEmitterInstance* owner, float inTime, Object* data)
{
	return lifetime_.GetValue(inTime, data);
}

bool ParticleModuleLifetime::LoadXML(const XMLElement& root)
{
	if (XMLElement lifetimeNode = root.GetChild("lifetime"))
	{
		const String curveType = lifetimeNode.GetAttribute("type");
		if (curveType == "uniform")
		{
			auto uniformCurve = MakeShared<DistributionFloatUniform>();
			uniformCurve->min_ = lifetimeNode.GetChild("min").GetFloat("value");
			uniformCurve->max_ = lifetimeNode.GetChild("max").GetFloat("value");
			lifetime_.distribution_ = uniformCurve;
		}
	}

	return true;
}

bool ParticleModuleLifetime::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}

}
