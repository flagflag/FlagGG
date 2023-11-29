#pragma once

#include "Math/Distributions/DistributionVectorConstant.h"

namespace FlagGG
{

class FlagGG_API DistributionVectorParameterBase : public DistributionVectorConstant
{
	OBJECT_OVERRIDE(DistributionVectorParameterBase, DistributionVectorConstant);
public:
	DistributionVectorParameterBase();

	//Begin UDistributionVector Interface
	virtual Vector3 GetValue(float f = 0.f, Object* data = NULL, Int32 extreme = 0, struct RandomStream* randomStream = NULL) const override;
	virtual bool CanBeBaked() const override { return false; }
	//End UDistributionVector Interface
	
	/** todo document */
	virtual bool GetParamValue(Object* data, const String& paramName, Vector3& outVector) const { return false; }

	/**
	 * Return whether or not this distribution can be baked into a FRawDistribution lookup table
	 */

	String parameterName_;

	Vector3 minInput_;

	Vector3 maxInput_;

	Vector3 minOutput_;

	Vector3 maxOutput_;

	EnumAsByte<DistributionParamMode> paramModes_[3];
};

}
