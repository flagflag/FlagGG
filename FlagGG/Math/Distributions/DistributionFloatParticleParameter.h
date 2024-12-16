#pragma once

#include "Math/Distributions/DistributionFloatParameterBase.h"

namespace FlagGG
{

class FlagGG_API DistributionFloatParticleParameter : public DistributionFloatParameterBase
{
	OBJECT_OVERRIDE(DistributionFloatParticleParameter, DistributionFloatParameterBase);
public:
	//~ Begin DistributionFloatParameterBase Interface
	virtual bool GetParamValue(Object* Data, const String& ParamName, float& OutFloat) const override
	{
		// TODO => Get ParticleSystemComponent
		return false;
	}
	//~ End DistributionFloatParameterBase Interface
};

}
