#pragma once

#include "Math/Distributions/DistributionFloat.h"
#include "Math/InterpCurve.h"

namespace FlagGG
{

class FlagGG_API DistributionFloatConstantCurve : public DistributionFloat
{
	OBJECT_OVERRIDE(DistributionFloatConstantCurve, DistributionFloat);
public:

	//~ Begin DistributionFloat Interface
	virtual float GetValue(float f = 0.f, Object* data = NULL, struct RandomStream* randomStream = NULL) const override;
	//~ End DistributionFloat Interface

	//~ Begin CurveEdInterface Interface
	virtual Int32		GetNumKeys() const override;
	virtual Int32		GetNumSubCurves() const override;
	virtual float	GetKeyIn(Int32 keyIndex) override;
	virtual float	GetKeyOut(Int32 subIndex, Int32 keyIndex) override;
	virtual Color	GetKeyColor(Int32 subIndex, Int32 keyIndex, const Color& curveColor) override;
	virtual void	GetInRange(float& minIn, float& maxIn) const override;
	virtual void	GetOutRange(float& minOut, float& maxOut) const override;
	virtual InterpCurveMode	GetKeyInterpMode(Int32 keyIndex) const override;
	virtual void	GetTangents(Int32 subIndex, Int32 keyIndex, float& arriveTangent, float& leaveTangent) const override;
	virtual float	EvalSub(Int32 subIndex, float inVal) override;
	virtual Int32		CreateNewKey(float keyIn) override;
	virtual void	DeleteKey(Int32 keyIndex) override;
	virtual Int32		SetKeyIn(Int32 keyIndex, float newInVal) override;
	virtual void	SetKeyOut(Int32 subIndex, Int32 keyIndex, float newOutVal) override;
	virtual void	SetKeyInterpMode(Int32 keyIndex, InterpCurveMode newMode) override;
	virtual void	SetTangents(Int32 subIndex, Int32 keyIndex, float arriveTangent, float leaveTangent) override;
	//~ End CurveEdInterface Interface

	/** Keyframe data for how output constant varies over time. */
	InterpCurveFloat constantCurve_;
};

}
