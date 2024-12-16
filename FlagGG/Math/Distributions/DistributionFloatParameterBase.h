#pragma once

#include "Math/Distributions/DistributionFloatConstant.h"

namespace FlagGG
{

class FlagGG_API DistributionFloatParameterBase : public DistributionFloatConstant
{
	OBJECT_OVERRIDE(DistributionFloatParameterBase, DistributionFloatConstant);
public:
	DistributionFloatParameterBase();

	//~ Begin DistributionFloat Interface
	virtual float GetValue(float f = 0.f, Object* data = NULL, struct RandomStream* randomStream = NULL) const override;
	//~ End DistributionFloat Interface


	/** todo document */
	virtual bool GetParamValue(Object* Data, const String& ParamName, float& OutFloat) const { return false; }


	// Begin Distribution Float
	virtual bool CanBeBaked() const override { return false; }


	/** todo document */
	String parameterName_;

	/** todo document */
	float minInput_;

	/** todo document */
	float maxInput_;

	/** todo document */
	float minOutput_;

	/** todo document */
	float maxOutput_;

	/** todo document */
	EnumAsByte<DistributionParamMode> paramMode_;
};

}
