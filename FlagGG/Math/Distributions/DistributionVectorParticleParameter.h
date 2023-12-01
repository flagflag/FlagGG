#pragma once

#include "Math/Distributions/DistributionVectorParameterBase.h"

namespace FlagGG
{

class FlagGG_API DistributionVectorParticleParameter : public DistributionVectorParameterBase
{
	OBJECT_OVERRIDE(DistributionVectorParticleParameter, DistributionVectorParameterBase);
public:
	//~ Begin UDistributionVectorParameterBase Interface
	virtual bool GetParamValue(Object* data, const String& paramName, Vector3& outVector) const override
	{
		// TODO => Get ParticleSystemComponent
		return false;
	}
	//~ End UDistributionVectorParameterBase Interface
};

}
