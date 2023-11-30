#pragma once

#include "Container/Vector.h"
#include "Container/EnumAsByte.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/TwoVector3.h"

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

template< class T >
FORCEINLINE InterpCurvePoint<T>::InterpCurvePoint(const float in, const T& out)
	: inVal_(in)
	, outVal_(out)
{
	memset(&arriveTangent_, 0, sizeof(T));
	memset(&leaveTangent_, 0, sizeof(T));

	interpMode_ = CIM_Linear;
}


template< class T >
FORCEINLINE InterpCurvePoint<T>::InterpCurvePoint(const float in, const T& out, const T& inArriveTangent, const T& inLeaveTangent, const InterpCurveMode inInterpMode)
	: inVal_(in)
	, outVal_(out)
	, arriveTangent_(inArriveTangent)
	, leaveTangent_(inLeaveTangent)
	, interpMode_(inInterpMode)
{ }

template< class T >
FORCEINLINE InterpCurvePoint<T>::InterpCurvePoint(EForceInit)
{
	inVal_ = 0.0f;
	memset(&outVal_, 0, sizeof(T));
	memset(&arriveTangent_, 0, sizeof(T));
	memset(&leaveTangent_, 0, sizeof(T));
	interpMode_ = CIM_Linear;
}

template< class T >
FORCEINLINE bool InterpCurvePoint<T>::IsCurveKey() const
{
	return ((interpMode_ == CIM_CurveAuto) || (interpMode_ == CIM_CurveAutoClamped) || (interpMode_ == CIM_CurveUser) || (interpMode_ == CIM_CurveBreak));
}

typedef InterpCurvePoint<float> InterpCurvePointFloat;
typedef InterpCurvePoint<Vector2> InterpCurvePointVector2D;
typedef InterpCurvePoint<Vector3> InterpCurvePointVector;
typedef InterpCurvePoint<TwoVectors> InterpCurvePointTwoVectors;

struct InterpCurveFloat
{
	/** Holds the collection of interpolation points. */
	Vector<InterpCurvePointFloat> points_;

	/** Specify whether the curve is looped or not */
	bool isLooped_;

	/** Specify the offset from the last point's input key corresponding to the loop point */
	float loopKeyOffset_;
};

struct InterpCurveVector2D
{
	/** Holds the collection of interpolation points. */
	Vector<InterpCurvePointVector2D> points_;

	/** Specify whether the curve is looped or not */
	bool isLooped_;

	/** Specify the offset from the last point's input key corresponding to the loop point */
	float loopKeyOffset_;
};

struct InterpCurveVector
{
	/** Holds the collection of interpolation points. */
	Vector<InterpCurvePointVector> points_;

	/** Specify whether the curve is looped or not */
	bool isLooped_;

	/** Specify the offset from the last point's input key corresponding to the loop point */
	float loopKeyOffset_;
};

struct InterpCurveTwoVectors
{
	/** Holds the collection of interpolation points. */
	Vector<InterpCurvePointTwoVectors> points_;

	/** Specify whether the curve is looped or not */
	bool isLooped_;

	/** Specify the offset from the last point's input key corresponding to the loop point */
	float loopKeyOffset_;
};

}
