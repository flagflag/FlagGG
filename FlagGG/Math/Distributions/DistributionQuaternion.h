//
// 四元素曲线
//

#pragma once

#include "Math/Distributions/Distribution.h"
#include "Math/Distributions/Distributions.h"

namespace FlagGG
{

class FlagGG_API DistributionQuaternion : public Distribution
{
public:
	DistributionQuaternion()
		: isDirty_(true)
	{
	}

	/** Script-accessible way to query a FVector distribution */
	virtual Quaternion GetQuatValue(float f = 0.f);

	virtual Quaternion GetValue(float f = 0.f, Object* data = NULL, Int32 lastExtreme = 0, struct RandomStream* randomStream = NULL) const;

	//~ Begin CurveEdInterface Interface
	virtual void GetInRange(float& minIn, float& maxIn) const override;
	virtual void GetOutRange(float& minOut, float& maxOut) const override;
	virtual	void GetRange(Quaternion& outMin, Quaternion& outMax) const;
	//~ End CurveEdInterface Interface

public:
	/** Set internally when the distribution is updated so that that FRawDistribution can know to update itself*/
	Int8 isDirty_ : 1;
};

}
