#pragma once

#include "Core/BaseTypes.h"
#include "Math/Color.h"
#include "Math/InterpCurvePoint.h"

namespace FlagGG
{

class FlagGG_API CurveEdInterface
{
public:
	/** Get number of keyframes in curve. */
	virtual UInt32 GetNumKeys() const { return 0; }

	/** Get number of 'sub curves' in this Curve. For example, a vector curve will have 3 sub-curves, for X, Y and Z. */
	virtual UInt32 GetNumSubCurves() const { return 0; }

	/**
	 * Provides the color for the sub-curve button that is present on the curve tab.
	 *
	 * @param	SubCurveIndex		The index of the sub-curve. Cannot be negative nor greater or equal to the number of sub-curves.
	 * @param	bIsSubCurveHidden	Is the curve hidden?
	 * @return						The color associated to the given sub-curve index.
	 */
	virtual Color GetSubCurveButtonColor(UInt32 subCurveIndex, bool isSubCurveHidden) const { return isSubCurveHidden ? Color(32, 0, 0) : Color(255, 0, 0); }

	/** Get the input value for the Key with the specified index. KeyIndex must be within range ie >=0 and < NumKeys. */
	virtual float GetKeyIn(UInt32 keyIndex) { return 0.f; }

	/**
	 *	Get the output value for the key with the specified index on the specified sub-curve.
	 *	SubIndex must be within range ie >=0 and < NumSubCurves.
	 *	KeyIndex must be within range ie >=0 and < NumKeys.
	 */
	virtual float GetKeyOut(UInt32 subIndex, UInt32 keyIndex) { return 0.f; }

	/**
	 * Provides the color for the given key at the given sub-curve.
	 *
	 * @param		SubIndex	The index of the sub-curve
	 * @param		KeyIndex	The index of the key in the sub-curve
	 * @param[in]	CurveColor	The color of the curve
	 * @return					The color that is associated the given key at the given sub-curve
	 */
	virtual Color GetKeyColor(UInt32 subIndex, UInt32 keyIndex, const Color& curveColor) { return curveColor; }

	/** Evaluate a subcurve at an arbitary point. Outside the keyframe range, curves are assumed to continue their end values. */
	virtual float EvalSub(UInt32 subIndex, float value) { return 0.f; }

	/**
	 *	Get the interpolation mode of the specified keyframe. This can be CIM_Constant, CIM_Linear or CIM_Curve.
	 *	KeyIndex must be within range ie >=0 and < NumKeys.
	 */
	virtual InterpCurveMode	GetKeyInterpMode(UInt32 keyIndex) const { return CIM_Linear; }

	/**
	 *	Get the incoming and outgoing tangent for the given subcurve and key.
	 *	SubIndex must be within range ie >=0 and < NumSubCurves.
	 *	KeyIndex must be within range ie >=0 and < NumKeys.
	 */
	virtual void GetTangents(UInt32 subIndex, UInt32 keyIndex, float& arriveTangent, float& leaveTangent) const { arriveTangent = 0.f; leaveTangent = 0.f; }

	/** Get input range of keys. Outside this region curve continues constantly the start/end values. */
	virtual void GetInRange(float& minIn, float& maxIn) const { minIn = 0.f; maxIn = 0.f; }

	/** Get overall range of output values. */
	virtual void GetOutRange(float& minOut, float& maxOut) const { minOut = 0.f; maxOut = 0.f; }

	/**
	 *	Add a new key to the curve with the specified input. Its initial value is set using EvalSub at that location.
	 *	Returns the index of the new key.
	 */
	virtual UInt32 CreateNewKey(float KeyIn) { return INDEX_NONE; }

	/**
	 *	Remove the specified key from the curve.
	 *	KeyIndex must be within range ie >=0 and < NumKeys.
	 */
	virtual void DeleteKey(UInt32 keyIndex) {}

	/**
	 *	Set the input value of the specified Key. This may change the index of the key, so the new index of the key is retured.
	 *	KeyIndex must be within range ie >=0 and < NumKeys.
	 */
	virtual UInt32 SetKeyIn(UInt32 keyIndex, float newInVal) { return keyIndex; }

	/**
	 *	Set the output values of the specified key.
	 *	SubIndex must be within range ie >=0 and < NumSubCurves.
	 *	KeyIndex must be within range ie >=0 and < NumKeys.
	 */
	virtual void SetKeyOut(UInt32 subIndex, UInt32 keyIndex, float newOutVal) {}

	/**
	 *	Set the method to use for interpolating between the give keyframe and the next one.
	 *	KeyIndex must be within range ie >=0 and < NumKeys.
	 */
	virtual void SetKeyInterpMode(UInt32 keyIndex, InterpCurveMode newMode) {}

	/**
	 *	Set the incoming and outgoing tangent for the given subcurve and key.
	 *	SubIndex must be within range ie >=0 and < NumSubCurves.
	 *	KeyIndex must be within range ie >=0 and < NumKeys.
	 */
	virtual void SetTangents(UInt32 subIndex, UInt32 keyIndex, float arriveTangent, float leaveTangent) {}
};

}
