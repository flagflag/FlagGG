#pragma once

#include "Math/Distributions/DistributionQuaternion.h"
#include "Math/InterpCurve.h"

namespace FlagGG
{

class FlagGG_API DistributionQuaternionConstantCurve : public DistributionQuaternion
{
	OBJECT_OVERRIDE(DistributionQuaternionConstantCurve, DistributionQuaternion);
public:
	//Begin DistributionQuaternion Interface
	virtual Quaternion GetValue(float f = 0.f, Object* data = NULL, Int32 lastExtreme = 0, struct RandomStream* randomStream = NULL) const override;
	virtual	void	GetRange(Quaternion& outMin, Quaternion& outMax) const override;
	//End DistributionQuaternion Interface

	//~ Begin CurveEdInterface Interface
	virtual Int32		GetNumKeys() const override;
	virtual Int32		GetNumSubCurves() const override;
	virtual Color	GetSubCurveButtonColor(Int32 subCurveIndex, bool isSubCurveHidden) const override;
	virtual float	GetKeyIn(Int32 keyIndex) override;
	virtual float	GetKeyOut(Int32 subIndex, Int32 keyIndex) override;
	virtual void	GetInRange(float& minIn, float& maxIn) const override;
	virtual void	GetOutRange(float& minOut, float& maxOut) const override;
	virtual Color	GetKeyColor(Int32 subIndex, Int32 keyIndex, const Color& curveColor) override;
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

	/** Keyframe data for each component (X,Y,Z) over time. */
	InterpCurveQuat constantCurve_;
};

}
