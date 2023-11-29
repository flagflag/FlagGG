#pragma once

#include "Core/Object.h"
#include "Math/CurveEdInterface.h"

namespace FlagGG
{

extern FlagGG_API UInt32 GDistributionType;

enum DistributionParamMode
{
	DPM_Normal,
	DPM_Abs,
	DPM_Direct,
	DPM_MAX,
};

class FlagGG_API Distribution : public Object, public CurveEdInterface
{
	OBJECT_OVERRIDE(Distribution, Object);
public:
	/** Default value for initializing and checking correct values on UDistributions. */
	static const float defaultValue_;
};

}
