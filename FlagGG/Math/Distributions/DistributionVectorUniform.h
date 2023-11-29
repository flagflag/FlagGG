#pragma once

#include "Math/Distributions/DistributionVector.h"

namespace FlagGG
{

class FlagGG_API DistributionVectorUniform : public DistributionVector
{
	OBJECT_OVERRIDE(DistributionVectorUniform, DistributionVector);
public:
	DistributionVectorUniform();

	virtual Vector3	GetValue(float f = 0.f, Object* data = NULL, Int32 lastExtreme = 0, struct RandomStream* randomStream = NULL) const override;

	//Begin UDistributionVector Interface

	//@todo.CONSOLE: Currently, consoles need this? At least until we have some sort of cooking/packaging step!
	virtual RawDistributionOperation GetOperation() const override;
	virtual UInt8 GetLockFlag() const override;
	virtual UInt32 InitializeRawEntry(float time, float* values) const override;
	virtual	void	GetRange(Vector3& outMin, Vector3& outMax) const override;
	//End UDistributionVector Interface

	/** These two functions will retrieve the Min/Max values respecting the Locked and Mirror flags. */
	virtual Vector3 GetMinValue() const;
	virtual Vector3 GetMaxValue() const;

	// We have 6 subs, 3 mins and three maxes. They are assigned as:
	// 0,1 = min/max x
	// 2,3 = min/max y
	// 4,5 = min/max z

	//~ Begin FCurveEdInterface Interface
	virtual Int32		GetNumKeys() const override;
	virtual Int32		GetNumSubCurves() const override;
	virtual Color	GetSubCurveButtonColor(Int32 SubCurveIndex, bool bIsSubCurveHidden) const override;
	virtual float	GetKeyIn(Int32 KeyIndex) override;
	virtual float	GetKeyOut(Int32 SubIndex, Int32 KeyIndex) override;
	virtual Color	GetKeyColor(Int32 SubIndex, Int32 KeyIndex, const Color& CurveColor) override;
	virtual void	GetInRange(float& MinIn, float& MaxIn) const override;
	virtual void	GetOutRange(float& MinOut, float& MaxOut) const override;
	virtual InterpCurveMode	GetKeyInterpMode(Int32 KeyIndex) const override;
	virtual void	GetTangents(Int32 SubIndex, Int32 KeyIndex, float& ArriveTangent, float& LeaveTangent) const override;
	virtual float	EvalSub(Int32 SubIndex, float InVal) override;
	virtual Int32		CreateNewKey(float KeyIn) override;
	virtual void	DeleteKey(Int32 KeyIndex) override;
	virtual Int32		SetKeyIn(Int32 KeyIndex, float NewInVal) override;
	virtual void	SetKeyOut(Int32 SubIndex, Int32 KeyIndex, float NewOutVal) override;
	virtual void	SetKeyInterpMode(Int32 KeyIndex, InterpCurveMode NewMode) override;
	virtual void	SetTangents(Int32 SubIndex, Int32 KeyIndex, float ArriveTangent, float LeaveTangent) override;
	//~ Begin FCurveEdInterface Interface

	/** Upper end of Vector3 magnitude range. */
	Vector3 max_;

	/** Lower end of Vector3 magnitude range. */
	Vector3 min_;

	/** If true, X == Y == Z ie. only one degree of freedom. If false, each axis is picked independently. */
	UInt32 lockAxes_ : 1;

	EnumAsByte<DistributionVectorLockFlags> lockedAxes_;

	EnumAsByte<DistributionVectorMirrorFlags> mirrorFlags_[3];

	UInt32 useExtremes_ : 1;
};

}
