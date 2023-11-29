#pragma once

#include "Math/Distributions/DistributionFloat.h"

namespace FlagGG
{

class FlagGG_API DistributionFloatUniformCurve : public DistributionFloat
{
	OBJECT_OVERRIDE(DistributionFloatUniformCurve, DistributionFloat);
public:
	//~ Begin UDistributionFloat Interface
	virtual float GetValue(float f = 0.f, Object* data = NULL, struct RandomStream* randomStream = NULL) const override;
	//@todo.CONSOLE: Currently, consoles need this? At least until we have some sort of cooking/packaging step!
	virtual RawDistributionOperation GetOperation() const override;
	virtual UInt32 InitializeRawEntry(float Time, float* Values) const override;
	//~ End UDistributionFloat Interface

	/**
	 *	This function will retrieve the max and min values at the given time.
	 */
	virtual Vector2 GetMinMaxValue(float f = 0.f, Object* data = NULL) const;

	//~ Begin FCurveEdInterface Interface
	virtual UInt32		GetNumKeys() const override;
	virtual UInt32		GetNumSubCurves() const override;
	virtual Color	GetSubCurveButtonColor(UInt32 subCurveIndex, bool isSubCurveHidden) const override;
	virtual float	GetKeyIn(UInt32 keyIndex) override;
	virtual float	GetKeyOut(UInt32 subIndex, UInt32 keyIndex) override;
	virtual Color	GetKeyColor(UInt32 subIndex, UInt32 keyIndex, const Color& curveColor) override;
	virtual void	GetInRange(float& minIn, float& maxIn) const override;
	virtual void	GetOutRange(float& minOut, float& maxOut) const override;
	virtual InterpCurveMode	GetKeyInterpMode(UInt32 keyIndex) const override;
	virtual void	GetTangents(UInt32 subIndex, UInt32 keyIndex, float& arriveTangent, float& leaveTangent) const override;
	virtual float	EvalSub(UInt32 subIndex, float inVal) override;
	virtual UInt32		CreateNewKey(float keyIn) override;
	virtual void	DeleteKey(UInt32 keyIndex) override;
	virtual UInt32		SetKeyIn(UInt32 keyIndex, float newInVal) override;
	virtual void	SetKeyOut(UInt32 subIndex, UInt32 keyIndex, float newOutVal) override;
	virtual void	SetKeyInterpMode(UInt32 keyIndex, InterpCurveMode newMode) override;
	virtual void	SetTangents(UInt32 subIndex, UInt32 keyIndex, float arriveTangent, float leaveTangent) override;
	//~ End FCurveEdInterface Interface

	/** Keyframe data for how output constant varies over time. */
	InterpCurveVector2D constantCurve_;
};

}
