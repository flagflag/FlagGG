#pragma once

#include "Container/Vector.h"
#include "Container/EnumAsByte.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/TwoVector3.h"
#include "Math/Color.h"

namespace FlagGG
{

enum InterpCurveMode
{
	/** A straight line between two keypoint values. */
	CIM_Linear,

	/** A cubic-hermite curve between two keypoints, using Arrive/Leave tangents. These tangents will be automatically
		updated when points are moved, etc.  Tangents are unclamped and will plateau at curve start and end points. */
	CIM_CurveAuto,

	/** The out value is held constant until the next key, then will jump to that value. */
	CIM_Constant,

	/** A smooth curve just like CIM_Curve, but tangents are not automatically updated so you can have manual control over them (eg. in Curve Editor). */
	CIM_CurveUser,

	/** A curve like CIM_Curve, but the arrive and leave tangents are not forced to be the same, so you can create a 'corner' at this key. */
	CIM_CurveBreak,

	/** A cubic-hermite curve between two keypoints, using Arrive/Leave tangents. These tangents will be automatically
		updated when points are moved, etc.  Tangents are clamped and will plateau at curve start and end points. */
	CIM_CurveAutoClamped,

	/** Invalid or unknown curve type. */
	CIM_Unknown
};

/**
 * Template for interpolation points.
 *
 * Interpolation points are used for describing the shape of interpolation curves.
 *
 * @see FInterpCurve
 * @todo Docs: InterpCurvePoint needs better function documentation.
 */
template< class T > class InterpCurvePoint
{
public:

	/**
	 * Default constructor (no initialization).
	 */
	InterpCurvePoint() { };

	/**
	 * Constructor
	 *
	 * @param In input value that corresponds to this key
	 * @param Out Output value of templated type
	 * @note uses linear interpolation
	 */
	InterpCurvePoint(const float In, const T& Out);

	/**
	 * Constructor
	 *
	 * @param In input value that corresponds to this key
	 * @param Out Output value of templated type
	 * @param InArriveTangent Tangent of curve arriving at this point.
	 * @param InLeaveTangent Tangent of curve leaving from this point.
	 * @param InInterpMode interpolation mode to use
	 */
	InterpCurvePoint(const float In, const T& Out, const T& InArriveTangent, const T& InLeaveTangent, const InterpCurveMode InInterpMode);

	/**
	 * Constructor which initializes all components to zero.
	 *
	 * @param EForceInit Force init enum
	 */
	explicit FORCEINLINE InterpCurvePoint(EForceInit);

public:

	/** @return true if the key value is using a curve interp mode, otherwise false */
	FORCEINLINE bool IsCurveKey() const;

public:

	/**
	 * Serializes the Curve Point.
	 *
	 * @param Ar Reference to the serialization archive.
	 * @param Point Reference to the curve point being serialized.
	 *
	 * @return Reference to the Archive after serialization.
	 */
	//friend FArchive& operator<<(FArchive& Ar, InterpCurvePoint& Point)
	//{
	//	Ar << Point.inVal_ << Point.outVal_;
	//	Ar << Point.arriveTangent_ << Point.leaveTangent_;
	//	Ar << Point.interpMode_;
	//	return Ar;
	//}

	/**
	 * Compare equality of two Curve Points
	 */
	friend bool operator==(const InterpCurvePoint& Point1, const InterpCurvePoint& Point2)
	{
		return (Point1.inVal_ == Point2.inVal_ &&
			Point1.outVal_ == Point2.outVal_ &&
			Point1.arriveTangent_ == Point2.arriveTangent_ &&
			Point1.leaveTangent_ == Point2.leaveTangent_ &&
			Point1.interpMode_ == Point2.interpMode_);
	}

	/**
	 * Compare inequality of two Curve Points
	 */
	friend bool operator!=(const InterpCurvePoint& Point1, const InterpCurvePoint& Point2)
	{
		return !(Point1 == Point2);
	}

public:
	/** Float input value that corresponds to this key (eg. time). */
	float inVal_;

	/** Output value of templated type when input is equal to inVal_. */
	T outVal_;

	/** Tangent of curve arrive this point. */
	T arriveTangent_;

	/** Tangent of curve leaving this point. */
	T leaveTangent_;

	/** Interpolation mode between this point and the next one. @see EInterpCurveMode */
	EnumAsByte<InterpCurveMode> interpMode_;
};


/* InterpCurvePoint inline functions
 *****************************************************************************/

template <class T>
FORCEINLINE InterpCurvePoint<T>::InterpCurvePoint(const float in, const T& out)
	: inVal_(in)
	, outVal_(out)
{
	memset(&arriveTangent_, 0, sizeof(T));
	memset(&leaveTangent_, 0, sizeof(T));

	interpMode_ = CIM_Linear;
}


template <class T>
FORCEINLINE InterpCurvePoint<T>::InterpCurvePoint(const float in, const T& out, const T& inArriveTangent, const T& inLeaveTangent, const InterpCurveMode inInterpMode)
	: inVal_(in)
	, outVal_(out)
	, arriveTangent_(inArriveTangent)
	, leaveTangent_(inLeaveTangent)
	, interpMode_(inInterpMode)
{ }

template <class T>
FORCEINLINE InterpCurvePoint<T>::InterpCurvePoint(EForceInit)
{
	inVal_ = 0.0f;
	memset(&outVal_, 0, sizeof(T));
	memset(&arriveTangent_, 0, sizeof(T));
	memset(&leaveTangent_, 0, sizeof(T));
	interpMode_ = CIM_Linear;
}

template <class T>
FORCEINLINE bool InterpCurvePoint<T>::IsCurveKey() const
{
	return ((interpMode_ == CIM_CurveAuto) || (interpMode_ == CIM_CurveAutoClamped) || (interpMode_ == CIM_CurveUser) || (interpMode_ == CIM_CurveBreak));
}

typedef InterpCurvePoint<float> InterpCurvePointFloat;
typedef InterpCurvePoint<Vector2> InterpCurvePointVector2D;
typedef InterpCurvePoint<Vector3> InterpCurvePointVector;
typedef InterpCurvePoint<TwoVectors> InterpCurvePointTwoVectors;


/** Computes Tangent for a curve segment */
template <class T>
inline void AutoCalcTangent(const T& prevP, const T& P, const T& nextP, const float tension, T& outTan)
{
	outTan = (1.f - tension) * ((P - prevP) + (nextP - P));
}

FlagGG_API float ClampFloatTangent(float PrevPointVal, float prevTime, float CurPointVal, float curTime, float NextPointVal, float nextTime);

/**
 * Computes a tangent for the specified control point; supports clamping, but only works
 * with floats or contiguous arrays of floats.
 */
template< class T >
inline void ComputeClampableFloatVectorCurveTangent(float prevTime, const T& prevPoint,
	float curTime, const T& curPoint,
	float nextTime, const T& nextPoint,
	float tension,
	bool wantClamping,
	T& outTangent)
{
	// Clamp the tangents if we need to do that
	if (wantClamping)
	{
		// NOTE: We always treat the type as an array of floats
		float* PrevPointVal = (float*)&prevPoint;
		float* CurPointVal = (float*)&curPoint;
		float* NextPointVal = (float*)&nextPoint;
		float* OutTangentVal = (float*)&outTangent;
		for (Int32 CurValPos = 0; CurValPos < sizeof(T); CurValPos += sizeof(float))
		{
			// Clamp it!
			const float ClampedTangent =
				ClampFloatTangent(
					*PrevPointVal, prevTime,
					*CurPointVal, curTime,
					*NextPointVal, nextTime);

			// Apply tension value
			*OutTangentVal = (1.0f - tension) * ClampedTangent;


			// Advance pointers
			++OutTangentVal;
			++PrevPointVal;
			++CurPointVal;
			++NextPointVal;
		}
	}
	else
	{
		// No clamping needed
		AutoCalcTangent(prevPoint, curPoint, nextPoint, tension, outTangent);

		const float PrevToNextTimeDiff = Max<float>(KINDA_SMALL_NUMBER, nextTime - prevTime);

		outTangent /= PrevToNextTimeDiff;
	}
}

/** Computes a tangent for the specified control point.  Special case for float types; supports clamping. */
inline void ComputeCurveTangent(float prevTime, const float& prevPoint,
	float curTime, const float& curPoint,
	float nextTime, const float& nextPoint,
	float tension,
	bool wantClamping,
	float& outTangent)
{
	ComputeClampableFloatVectorCurveTangent(
		prevTime, prevPoint,
		curTime, curPoint,
		nextTime, nextPoint,
		tension, wantClamping, outTangent);
}


/** Computes a tangent for the specified control point.  Special case for Vector3 types; supports clamping. */
inline void ComputeCurveTangent(float prevTime, const Vector3& prevPoint,
	float curTime, const Vector3& curPoint,
	float nextTime, const Vector3& nextPoint,
	float tension,
	bool wantClamping,
	Vector3& outTangent)
{
	ComputeClampableFloatVectorCurveTangent(
		prevTime, prevPoint,
		curTime, curPoint,
		nextTime, nextPoint,
		tension, wantClamping, outTangent);
}


/** Computes a tangent for the specified control point.  Special case for Vector2 types; supports clamping. */
inline void ComputeCurveTangent(float prevTime, const Vector2& prevPoint,
	float curTime, const Vector2& curPoint,
	float nextTime, const Vector2& nextPoint,
	float tension,
	bool wantClamping,
	Vector2& outTangent)
{
	ComputeClampableFloatVectorCurveTangent(
		prevTime, prevPoint,
		curTime, curPoint,
		nextTime, nextPoint,
		tension, wantClamping, outTangent);
}


/** Computes a tangent for the specified control point.  Special case for FTwoVectors types; supports clamping. */
inline void ComputeCurveTangent(float prevTime, const TwoVectors& prevPoint,
	float curTime, const TwoVectors& curPoint,
	float nextTime, const TwoVectors& nextPoint,
	float tension,
	bool wantClamping,
	TwoVectors& outTangent)
{
	ComputeClampableFloatVectorCurveTangent(
		prevTime, prevPoint,
		curTime, curPoint,
		nextTime, nextPoint,
		tension, wantClamping, outTangent);
}

FlagGG_API void CurveFloatFindIntervalBounds(const InterpCurvePoint<float>& start, const InterpCurvePoint<float>& End, float& currentMin, float& currentMax);

FlagGG_API void CurveVector2DFindIntervalBounds(const InterpCurvePoint<Vector2>& start, const InterpCurvePoint<Vector2>& End, Vector2& currentMin, Vector2& currentMax);

FlagGG_API void CurveVectorFindIntervalBounds(const InterpCurvePoint<Vector3>& start, const InterpCurvePoint<Vector3>& End, Vector3& currentMin, Vector3& currentMax);

FlagGG_API void CurveTwoVectorsFindIntervalBounds(const InterpCurvePoint<TwoVectors>& start, const InterpCurvePoint<TwoVectors>& End, TwoVectors& currentMin, TwoVectors& currentMax);

FlagGG_API void CurveLinearColorFindIntervalBounds(const InterpCurvePoint<Color>& start, const InterpCurvePoint<Color>& End, Color& currentMin, Color& currentMax);

template <class T>
inline void CurveFindIntervalBounds(const InterpCurvePoint<T>& start, const InterpCurvePoint<T>& End, T& currentMin, T& currentMax, const float dummy)
{ }

template <>
inline void CurveFindIntervalBounds(const InterpCurvePoint<float>& start, const InterpCurvePoint<float>& end, float& currentMin, float& currentMax, const float dummy)
{
	CurveFloatFindIntervalBounds(start, end, currentMin, currentMax);
}


template <>
inline void CurveFindIntervalBounds(const InterpCurvePoint<Vector2>& start, const InterpCurvePoint<Vector2>& end, Vector2& currentMin, Vector2& currentMax, const float dummy)
{
	CurveVector2DFindIntervalBounds(start, end, currentMin, currentMax);
}


template <>
inline void CurveFindIntervalBounds(const InterpCurvePoint<Vector3>& start, const InterpCurvePoint<Vector3>& end, Vector3& currentMin, Vector3& currentMax, const float dummy)
{
	CurveVectorFindIntervalBounds(start, end, currentMin, currentMax);
}


template <>
inline void CurveFindIntervalBounds(const InterpCurvePoint<TwoVectors>& start, const InterpCurvePoint<TwoVectors>& end, TwoVectors& currentMin, TwoVectors& currentMax, const float dummy)
{
	CurveTwoVectorsFindIntervalBounds(start, end, currentMin, currentMax);
}


template <>
inline void CurveFindIntervalBounds(const InterpCurvePoint<Color>& start, const InterpCurvePoint<Color>& end, Color& currentMin, Color& currentMax, const float dummy)
{
	CurveLinearColorFindIntervalBounds(start, end, currentMin, currentMax);
}

}
