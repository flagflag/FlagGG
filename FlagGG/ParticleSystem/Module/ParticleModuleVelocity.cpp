#include "ParticleModuleVelocity.h"
#include "Math/Distributions/DistributionFloatUniform.h"
#include "Math/Distributions/DistributionVectorUniform.h"
#include "Math/Distributions/DistributionVectorConstant.h"
#include "Math/Distributions/DistributionVectorConstantCurve.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/ParticleLODLevel.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "Core/ObjectFactory.h"

namespace FlagGG
{

REGISTER_TYPE_FACTORY(ParticleModuleVelocity);
REGISTER_TYPE_FACTORY(ParticleModuleVelocityOverLifetime);

/*-----------------------------------------------------------------------------
	Abstract base modules used for categorization.
-----------------------------------------------------------------------------*/
ParticleModuleVelocityBase::ParticleModuleVelocityBase()
{
}

bool ParticleModuleVelocityBase::LoadXML(const XMLElement& root)
{
	if (XMLElement inWorldSpaceNode = root.GetChild("inWorldSpace"))
	{
		inWorldSpace_ = inWorldSpaceNode.GetBool("value");
	}

	if (XMLElement applyOwnerScaleNode = root.GetChild("applyOwnerScale"))
	{
		applyOwnerScale_ = applyOwnerScaleNode.GetBool("value");
	}

	return true;
}

bool ParticleModuleVelocityBase::SaveXML(XMLElement& root)
{
	// TODO
	return false;
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

bool ParticleModuleVelocity::LoadXML(const XMLElement& root)
{
	if (XMLElement superModule = root.GetChild("module"))
	{
		if (superModule.GetAttribute("type") != "ParticleModuleVelocityBase")
		{
			return false;
		}

		if (!ParticleModuleVelocityBase::LoadXML(superModule))
		{
			return false;
		}
	}

	if (XMLElement startVelocityNode = root.GetChild("startVelocity"))
	{
		const String curveType = startVelocityNode.GetAttribute("type");

		if (curveType == "uniform")
		{
			auto uniformCurve = MakeShared<DistributionVectorUniform>();
			uniformCurve->min_ = startVelocityNode.GetChild("min").GetVector3("value");
			uniformCurve->max_ = startVelocityNode.GetChild("max").GetVector3("value");
			startVelocity_.distribution_ = uniformCurve;
		}

		return true;
	}

	return false;
}

bool ParticleModuleVelocity::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}

/*-----------------------------------------------------------------------------
	ParticleModuleVelocityOverLifetime implementation.
-----------------------------------------------------------------------------*/
ParticleModuleVelocityOverLifetime::ParticleModuleVelocityOverLifetime()
{
	spawnModule_ = true;
	updateModule_ = true;

	absolute_ = false;
}

void ParticleModuleVelocityOverLifetime::InitializeDefaults()
{
	if (!velOverLife_.IsCreated())
	{
		velOverLife_.distribution_ = MakeShared<DistributionVectorConstantCurve>();
	}
}

void ParticleModuleVelocityOverLifetime::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	if (absolute_)
	{
		SPAWN_INIT;
		Vector3 ownerScale(Vector3::ONE);
		if ((applyOwnerScale_ == true) && owner && owner->component_)
		{
			ownerScale = owner->component_->GetNode()->GetWorldScale();
		}
		Vector3 vel = velOverLife_.GetValue(particle.relativeTime_, owner->component_) * ownerScale;
		particle.velocity_ = vel;
		particle.baseVelocity_ = vel;
	}
}

void ParticleModuleVelocityOverLifetime::Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime)
{
	ASSERT(owner && owner->component_);
	ParticleLODLevel* LODLevel = owner->spriteTemplate_->GetCurrentLODLevel(owner);
	ASSERT(LODLevel);
	Vector3 ownerScale(Vector3::ONE);
	const Matrix3x4& ownerTM = owner->component_->GetNode()->GetWorldTransform();
	if (applyOwnerScale_ == true)
	{
		ownerScale = ownerTM.SignedScale();
	}
	if (absolute_)
	{
		if (LODLevel->requiredModule_->useLocalSpace_ == false)
		{
			if (inWorldSpace_ == false)
			{
				Vector3 vel;
				const Matrix3x4 localToWorld = /*ownerTM.ToMatrixNoScale()*/ownerTM;
				BEGIN_UPDATE_LOOP;
				{
					vel = velOverLife_.GetValue(particle.relativeTime_, owner->component_);
					particle.velocity_ = localToWorld * vel * ownerScale;
				}
				END_UPDATE_LOOP;
			}
			else
			{
				BEGIN_UPDATE_LOOP;
				{
					particle.velocity_ = velOverLife_.GetValue(particle.relativeTime_, owner->component_) * ownerScale;
				}
				END_UPDATE_LOOP;
			}
		}
		else
		{
			if (inWorldSpace_ == false)
			{
				BEGIN_UPDATE_LOOP;
				{
					particle.velocity_ = velOverLife_.GetValue(particle.relativeTime_, owner->component_) * ownerScale;
				}
				END_UPDATE_LOOP;
			}
			else
			{
				Vector3 vel;
				const Matrix3x4 localToWorld = /*ownerTM.ToMatrixNoScale()*/ownerTM;
				const Matrix3x4 invMat = localToWorld.Inverse();
				BEGIN_UPDATE_LOOP;
				{
					vel = velOverLife_.GetValue(particle.relativeTime_, owner->component_);
					particle.velocity_ = invMat * vel * ownerScale;
				}
				END_UPDATE_LOOP;
			}
		}
	}
	else
	{
		if (LODLevel->requiredModule_->useLocalSpace_ == false)
		{
			Vector3 vel;
			if (inWorldSpace_ == false)
			{
				const Matrix3x4 localToWorld = /*ownerTM.ToMatrixNoScale()*/ownerTM;
				BEGIN_UPDATE_LOOP;
				{
					vel = velOverLife_.GetValue(particle.relativeTime_, owner->component_);
					particle.velocity_ *= localToWorld * vel * ownerScale;
				}
				END_UPDATE_LOOP;
			}
			else
			{
				BEGIN_UPDATE_LOOP;
				{
					particle.velocity_ *= velOverLife_.GetValue(particle.relativeTime_, owner->component_) * ownerScale;
				}
				END_UPDATE_LOOP;
			}
		}
		else
		{
			if (inWorldSpace_ == false)
			{
				BEGIN_UPDATE_LOOP;
				{
					particle.velocity_ *= velOverLife_.GetValue(particle.relativeTime_, owner->component_) * ownerScale;
				}
				END_UPDATE_LOOP;
			}
			else
			{
				Vector3 vel;
				const Matrix3x4 localToWorld = /*ownerTM.ToMatrixNoScale()*/ownerTM;
				const Matrix3x4 invMat = localToWorld.Inverse();
				BEGIN_UPDATE_LOOP;
				{
					vel = velOverLife_.GetValue(particle.relativeTime_, owner->component_);
					particle.velocity_ *= invMat * vel * ownerScale;
				}
				END_UPDATE_LOOP;
			}
		}
	}
}

bool ParticleModuleVelocityOverLifetime::LoadXML(const XMLElement& root)
{
	if (XMLElement superModule = root.GetChild("module"))
	{
		if (superModule.GetAttribute("type") != "ParticleModuleVelocityBase")
		{
			return false;
		}

		if (!ParticleModuleVelocityBase::LoadXML(superModule))
		{
			return false;
		}
	}

	if (XMLElement velOverLifeNode = root.GetChild("velOverLife"))
	{
		const String curveType = velOverLifeNode.GetAttribute("type");

		if (curveType == "multipleCurve")
		{
			// TODO
		}
	}

	if (XMLElement absoluteNode = root.GetChild("absolute"))
	{
		absolute_ = absoluteNode.GetBool("value");
	}

	if (XMLElement orbitOverLifeNode = root.GetChild("orbitOverLife"))
	{
		const String curveType = orbitOverLifeNode.GetAttribute("type");
		if (curveType == "constant")
		{
			auto constant = MakeShared<DistributionVectorConstant>();
			constant->constant_ = orbitOverLifeNode.GetChild("constant").GetVector3("value");
			velOverLife_.distribution_ = constant;
		}
	}

	if (XMLElement offsetOverLifeNode = root.GetChild("offsetOverLife"))
	{

	}

	if (XMLElement radialOverLifeNode = root.GetChild("radialOverLife"))
	{

	}
	
	return true;
}

bool ParticleModuleVelocityOverLifetime::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}

}
