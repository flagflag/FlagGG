#include "ParticleModuleLocation.h"
#include "Math/Distributions/DistributionVectorUniform.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "Core/ObjectFactory.h"

namespace FlagGG
{

REGISTER_TYPE_FACTORY(ParticleModulePivotOffset);
REGISTER_TYPE_FACTORY(ParticleModuleLocation);

/*-----------------------------------------------------------------------------
	ParticleModulePivotOffset implementation.
-----------------------------------------------------------------------------*/

ParticleModulePivotOffset::ParticleModulePivotOffset()
{
	spawnModule_ = false;
	updateModule_ = false;
}

void ParticleModulePivotOffset::InitializeDefaults()
{
	pivotOffset_ = Vector2(0.0f, 0.0f);
}

void ParticleModulePivotOffset::CompileModule(ParticleEmitterBuildInfo& emitterInfo)
{
	emitterInfo.pivotOffset_ = pivotOffset_ - Vector2(0.5f, 0.5f);
}

bool ParticleModulePivotOffset::LoadXML(const XMLElement& root)
{
	return true;
}

bool ParticleModulePivotOffset::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}


/*-----------------------------------------------------------------------------
	ParticleModuleLocation implementation.
-----------------------------------------------------------------------------*/

ParticleModuleLocation::ParticleModuleLocation()
{
	spawnModule_ = true;
	supported3DDrawMode_ = true;
	distributeOverNPoints_ = 0.0f;
}

void ParticleModuleLocation::InitializeDefaults()
{
	if (!startLocation_.IsCreated())
	{
		startLocation_.distribution_ = MakeShared<DistributionVectorUniform>();
	}
}

void ParticleModuleLocation::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	SpawnEx(owner, offset, spawnTime, &GetRandomStream(owner), particleBase);
}

void ParticleModuleLocation::SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, struct RandomStream* inRandomStream, BaseParticle* particleBase)
{
	SPAWN_INIT;
	ParticleLODLevel* LODLevel	= owner->spriteTemplate_->GetCurrentLODLevel(owner);
	ASSERT(LODLevel);
	Vector3 locationOffset;

	// Avoid divide by zero.
	if ((distributeOverNPoints_ != 0.0f) && (distributeOverNPoints_ != 1.f))
	{
		float RandomNum = inRandomStream->FRand() * Fract(owner->emitterTime_);

		if(RandomNum > distributeThreshold_)
		{
			locationOffset = startLocation_.GetValue(owner->emitterTime_, owner->component_, 0, inRandomStream);
		}
		else
		{
			Vector3 min, max;
			startLocation_.GetRange(min, max);
			Vector3 lerped = Lerp(min, max, Floor((inRandomStream->FRand() * (distributeOverNPoints_ - 1.0f)) + 0.5f)/(distributeOverNPoints_ - 1.0f));
			locationOffset.Set(lerped.x_, lerped.y_, lerped.z_);
		}
	}
	else
	{
		locationOffset = startLocation_.GetValue(owner->emitterTime_, owner->component_, 0, inRandomStream);
	}

	locationOffset = owner->emitterToSimulation_ * locationOffset;
	particle.location_ += locationOffset;
	CRY_ENSURE(!particle.location_.ContainsNaN(), "NaN in Particle Location. Template: %s", owner->component_ ? owner->component_->template_->GetName().CString() : "UNKNOWN");
}

bool ParticleModuleLocation::LoadXML(const XMLElement& root)
{
	if (XMLElement startLocationNode = root.GetChild("startLocation"))
	{
		const String curveType = startLocationNode.GetAttribute("type");

		if (curveType == "uniform")
		{
			auto uniformCurve = MakeShared<DistributionVectorUniform>();
			uniformCurve->min_ = startLocationNode.GetChild("min").GetVector3("value");
			uniformCurve->max_ = startLocationNode.GetChild("max").GetVector3("value");
		}

		return true;
	}

	return false;
}

bool ParticleModuleLocation::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}

}
