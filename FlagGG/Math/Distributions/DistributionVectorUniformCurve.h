#pragma once

#include "Math/Distributions/DistributionVector.h"

namespace FlagGG
{

class FlagGG_API DistributionVectorUniformCurve : public DistributionVector
{
	OBJECT_OVERRIDE(DistributionVectorUniformCurve, DistributionVector);
public:
	DistributionVectorUniformCurve();

	virtual Vector3	GetValue(float f = 0.f, Object* data = NULL, Int32 lastExtreme = 0, struct RandomStream* randomStream = NULL) const override;

	//Begin UDistributionVector Interface
	//@todo.CONSOLE: Currently, consoles need this? At least until we have some sort of cooking/packaging step!
	virtual RawDistributionOperation GetOperation() const override;
	virtual UInt32 InitializeRawEntry(float time, float* values) const override;
	virtual	void	GetRange(Vector3& outMin, Vector3& outMax) const override;
	//End UDistributionVector Interface

	/** These two functions will retrieve the Min/Max values respecting the Locked and Mirror flags. */
	virtual Vector3 GetMinValue() const;
	virtual Vector3 GetMaxValue() const;

	/**
		*	This function will retrieve the max and min values at the given time.
		*/
	virtual TwoVectors GetMinMaxValue(float f = 0.f, Object* data = NULL) const;

	//~ Begin CurveEdInterface Interface
	virtual Int32		GetNumKeys() const override;
	virtual Int32		GetNumSubCurves() const override;
	virtual Color	GetSubCurveButtonColor(Int32 subCurveIndex, bool isSubCurveHidden) const override;
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
	virtual void	LockAndMirror(TwoVectors& value) const;
	//~ Begin CurveEdInterface Interface

	/** Keyframe data for how output constant varies over time. */
	InterpCurveTwoVectors constantCurve_;

	/** If true, X == Y == Z ie. only one degree of freedom. If false, each axis is picked independently. */
	UInt32 lockAxes1_ : 1;

	UInt32 lockAxes2_ : 1;

	EnumAsByte<DistributionVectorLockFlags> lockedAxes_[2];

	EnumAsByte<DistributionVectorMirrorFlags> mirrorFlags_[3];

	UInt32 useExtremes_ : 1;
};

}
