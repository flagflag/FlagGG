#include "ParticleModuleParameter.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleSystemComponent.h"

namespace FlagGG
{

float ParticleModuleParameterDynamic::GetParameterValue(EmitterDynamicParameter& inDynParams, BaseParticle& particle, ParticleEmitterInstance* owner, RandomStream* inRandomStream)
{
	float scaleValue = 1.0f;
	float distributionValue = 1.0f;
	switch (inDynParams.valueMethod_)
	{
	case EDPV_VelocityX:
		scaleValue = particle.velocity_.x_;
		break;
	case EDPV_VelocityY:
		scaleValue = particle.velocity_.y_;
		break;
	case EDPV_VelocityZ:
		scaleValue = particle.velocity_.z_;
		break;
	case EDPV_VelocityMag:
		scaleValue = particle.velocity_.Length();
		break;
	default:
		//case EDPV_UserSet:
		//case EDPV_AutoSet:
		break;
	}

	if ((inDynParams.scaleVelocityByParamValue_ == true) || (inDynParams.valueMethod_ == EDPV_UserSet))
	{
		float TimeValue = inDynParams.useEmitterTime_ ? owner->emitterTime_ : particle.relativeTime_;
		distributionValue = inDynParams.paramValue_.GetValue(TimeValue, owner->component_, inRandomStream);
	}

	return distributionValue * scaleValue;
}

float ParticleModuleParameterDynamic::GetParameterValue_UserSet(EmitterDynamicParameter& inDynParams, BaseParticle& particle, ParticleEmitterInstance* owner, RandomStream* inRandomStream)
{
	return inDynParams.paramValue_.GetValue(inDynParams.useEmitterTime_ ? owner->emitterTime_ : particle.relativeTime_, owner->component_, inRandomStream);
}

}
