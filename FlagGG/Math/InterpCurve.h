// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/BaseTypes.h"
#include "Container/Vector.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/TwoVector3.h"
#include "Math/InterpCurvePoint.h"

namespace FlagGG
{

/**
 * Template for interpolation curves.
 *
 * @see InterpCurvePoint
 * @todo Docs: InterpCurve needs template and function documentation
 */
template<class T>
class InterpCurve
{
public:

	/** Holds the collection of interpolation points. */
	Vector<InterpCurvePoint<T>> points_;

	/** Specify whether the curve is looped or not */
	bool isLooped_;

	/** Specify the offset from the last point's input key corresponding to the loop point */
	float loopKeyOffset_;

public:

	/** default constructor. */
	InterpCurve()
		: isLooped_(false)
		, loopKeyOffset_(0.f)
	{
	}

public:

	/**
	 * Adds a new keypoint to the InterpCurve with the supplied In and Out value.
	 *
	 * @param inVal
	 * @param outVal
	 * @return The index of the new key.
	 */
	Int32 AddPoint(const float inVal, const T& outVal);

	/**
	 * Moves a keypoint to a new In value.
	 *
	 * This may change the index of the keypoint, so the new key index is returned.
	 *
	 * @param pointIndex
	 * @param newInVal
	 * @return
	 */
	Int32 MovePoint(Int32 pointIndex, float newInVal);

	/** Clears all keypoints from InterpCurve. */
	void Reset();

	/** Set loop key for curve */
	void SetLoopKey(float inLoopKey);

	/** Clear loop key for curve */
	void ClearLoopKey();

	/**
	 *	Evaluate the output for an arbitary input value.
	 *	For inputs outside the range of the keys, the first/last key value is assumed.
	 */
	T Eval(const float inVal, const T& default = T(ForceInit)) const;

	/**
	 *	Evaluate the derivative at a point on the curve.
	 */
	T EvalDerivative(const float inVal, const T& default = T(ForceInit)) const;

	/**
	 *	Evaluate the second derivative at a point on the curve.
	 */
	T EvalSecondDerivative(const float inVal, const T& default = T(ForceInit)) const;

	/**
	 * Find the nearest point on spline to the given point.
	 *
	 * @param pointInSpace - the given point
	 * @param outDistanceSq - output - the squared distance between the given point and the closest found point.
	 * @return The key (the 't' parameter) of the nearest point.
	 */
	float InaccurateFindNearest(const T& pointInSpace, float& outDistanceSq) const;

	/**
	* Find the nearest point on spline to the given point.
	 *
	* @param pointInSpace - the given point
	* @param outDistanceSq - output - the squared distance between the given point and the closest found point.
	* @param outSegment - output - the nearest segment to the given point.
	* @return The key (the 't' parameter) of the nearest point.
	*/
	float InaccurateFindNearest(const T& pointInSpace, float& outDistanceSq, float& outSegment) const;

	/**
	 * Find the nearest point (to the given point) on segment between points_[ptIdx] and points_[ptIdx+1]
	 *
	 * @param pointInSpace - the given point
	 * @return The key (the 't' parameter) of the found point.
	 */
	float InaccurateFindNearestOnSegment(const T& pointInSpace, Int32 ptIdx, float& outSquaredDistance) const;

	/** Automatically set the tangents on the curve based on surrounding points */
	void AutoSetTangents(float tension = 0.0f, bool stationaryEndpoints = true);

	/** Calculate the min/max out value that can be returned by this InterpCurve. */
	void CalcBounds(T& outMin, T& outMax, const T& default = T(ForceInit)) const;

public:

	/**
	 * Serializes the interp curve.
	 *
	 * @param Ar Reference to the serialization archive.
	 * @param Curve Reference to the interp curve being serialized.
	 *
	 * @return Reference to the Archive after serialization.
	 */
	 //friend FArchive& operator<<( FArchive& Ar, InterpCurve& Curve )
	 //{
	 //	// NOTE: This is not used often for FInterpCurves.  Most of the time these are serialized
	 //	//   as inline struct properties in UnClass.cpp!

	 //	Ar << Curve.points_;
	 //	if (Ar.UEVer() >= VER_UE4_INTERPCURVE_SUPPORTS_LOOPING)
	 //	{
	 //		Ar << Curve.isLooped_;
	 //		Ar << Curve.loopKeyOffset_;
	 //	}

	 //	return Ar;
	 //}

	 /**
	  * Compare equality of two FInterpCurves
	  */
	friend bool operator==(const InterpCurve& curve1, const InterpCurve& curve2)
	{
		return (curve1.points_ == curve2.points_ &&
			curve1.isLooped_ == curve2.isLooped_ &&
			(!curve1.isLooped_ || curve1.loopKeyOffset_ == curve2.loopKeyOffset_));
	}

	/**
	 * Compare inequality of two FInterpCurves
	 */
	friend bool operator!=(const InterpCurve& curve1, const InterpCurve& curve2)
	{
		return !(curve1 == curve2);
	}

	/**
	 * Finds the lower index of the two points whose input values bound the supplied input value.
	 */
	Int32 GetPointIndexForInputValue(const float inValue) const;
};


/* InterpCurve inline functions
 *****************************************************************************/

template< class T > 
Int32 InterpCurve<T>::AddPoint(const float inVal, const T& outVal)
{
	Int32 i = 0; for (i = 0; i < points_.Size() && points_[i].inVal_ < inVal; i++);
	points_.Insert(i, InterpCurvePoint<T>(inVal, outVal));
	return i;
}


template< class T > 
Int32 InterpCurve<T>::MovePoint(Int32 pointIndex, float newInVal)
{
	if (pointIndex < 0 || pointIndex >= points_.Size())
		return pointIndex;

	const T outVal = points_[pointIndex].outVal_;
	const InterpCurveMode Mode = points_[pointIndex].interpMode_;
	const T ArriveTan = points_[pointIndex].arriveTangent_;
	const T LeaveTan = points_[pointIndex].leaveTangent_;

	points_.Erase(pointIndex);

	const Int32 NewPointIndex = AddPoint(newInVal, outVal);
	points_[NewPointIndex].interpMode_ = Mode;
	points_[NewPointIndex].arriveTangent_ = ArriveTan;
	points_[NewPointIndex].leaveTangent_ = LeaveTan;

	return NewPointIndex;
}


template< class T > 
void InterpCurve<T>::Reset()
{
	points_.Clear();
}


template <class T>
void InterpCurve<T>::SetLoopKey(float inLoopKey)
{
	// Can't set a loop key if there are no points
	if (points_.Size() == 0)
	{
		isLooped_ = false;
		return;
	}

	const float LastInKey = points_.Back().inVal_;
	if (inLoopKey > LastInKey)
	{
		// Calculate loop key offset from the input key of the final point
		isLooped_ = true;
		loopKeyOffset_ = inLoopKey - LastInKey;
	}
	else
	{
		// Specified a loop key lower than the final point; turn off looping.
		isLooped_ = false;
	}
}


template <class T>
void InterpCurve<T>::ClearLoopKey()
{
	isLooped_ = false;
}


template< class T >
Int32 InterpCurve<T>::GetPointIndexForInputValue(const float inValue) const
{
	const Int32 numPoints = points_.Size();
	const Int32 lastPoint = numPoints - 1;

	ASSERT(numPoints > 0);

	if (inValue < points_[0].inVal_)
	{
		return -1;
	}

	if (inValue >= points_[lastPoint].inVal_)
	{
		return lastPoint;
	}

	Int32 minIndex = 0;
	Int32 maxIndex = numPoints;

	while (maxIndex - minIndex > 1)
	{
		Int32 midIndex = (minIndex + maxIndex) / 2;

		if (points_[midIndex].inVal_ <= inValue)
		{
			minIndex = midIndex;
		}
		else
		{
			maxIndex = midIndex;
		}
	}

	return minIndex;
}


template< class T >
T InterpCurve<T>::Eval(const float inVal, const T& default) const
{
	const Int32 numPoints = points_.Size();
	const Int32 lastPoint = numPoints - 1;

	// If no point in curve, return the default value we passed in.
	if (numPoints == 0)
	{
		return default;
	}

	// Binary search to find index of lower bound of input value
	const Int32 index = GetPointIndexForInputValue(inVal);

	// If before the first point, return its value
	if (index == -1)
	{
		return points_[0].outVal_;
	}

	// If on or beyond the last point, return its value.
	if (index == lastPoint)
	{
		if (!isLooped_)
		{
			return points_[lastPoint].outVal_;
		}
		else if (inVal >= points_[lastPoint].inVal_ + loopKeyOffset_)
		{
			// Looped spline: last point is the same as the first point
			return points_[0].outVal_;
		}
	}

	// Somewhere within curve range - interpolate.
	ASSERT(index >= 0 && ((isLooped_ && index < numPoints) || (!isLooped_ && index < lastPoint)));
	const bool bLoopSegment = (isLooped_ && index == lastPoint);
	const Int32 nextIndex = bLoopSegment ? 0 : (index + 1);

	const auto& prevPoint = points_[index];
	const auto& nextPoint = points_[nextIndex];

	const float diff = bLoopSegment ? loopKeyOffset_ : (nextPoint.inVal_ - prevPoint.inVal_);

	if (diff > 0.0f && prevPoint.interpMode_ != CIM_Constant)
	{
		const float alpha = (inVal - prevPoint.inVal_) / diff;
		ASSERT(alpha >= 0.0f && alpha <= 1.0f);

		if (prevPoint.interpMode_ == CIM_Linear)
		{
			return Lerp(prevPoint.outVal_, nextPoint.outVal_, alpha);
		}
		else
		{
			return CubicInterp(prevPoint.outVal_, prevPoint.leaveTangent_ * diff, nextPoint.outVal_, nextPoint.arriveTangent_ * diff, alpha);
		}
	}
	else
	{
		return points_[index].outVal_;
	}
}


template< class T >
T InterpCurve<T>::EvalDerivative(const float inVal, const T& default) const
{
	const Int32 numPoints = points_.Size();
	const Int32 lastPoint = numPoints - 1;

	// If no point in curve, return the default value we passed in.
	if (numPoints == 0)
	{
		return default;
	}

	// Binary search to find index of lower bound of input value
	const Int32 index = GetPointIndexForInputValue(inVal);

	// If before the first point, return its tangent value
	if (index == -1)
	{
		return points_[0].leaveTangent_;
	}

	// If on or beyond the last point, return its tangent value.
	if (index == lastPoint)
	{
		if (!isLooped_)
		{
			return points_[lastPoint].arriveTangent_;
		}
		else if (inVal >= points_[lastPoint].inVal_ + loopKeyOffset_)
		{
			// Looped spline: last point is the same as the first point
			return points_[0].arriveTangent_;
		}
	}

	// Somewhere within curve range - interpolate.
	ASSERT(index >= 0 && ((isLooped_ && index < numPoints) || (!isLooped_ && index < lastPoint)));
	const bool bLoopSegment = (isLooped_ && index == lastPoint);
	const Int32 nextIndex = bLoopSegment ? 0 : (index + 1);

	const auto& prevPoint = points_[index];
	const auto& nextPoint = points_[nextIndex];

	const float diff = bLoopSegment ? loopKeyOffset_ : (nextPoint.inVal_ - prevPoint.inVal_);

	if (diff > 0.0f && prevPoint.interpMode_ != CIM_Constant)
	{
		if (prevPoint.interpMode_ == CIM_Linear)
		{
			return (nextPoint.outVal_ - prevPoint.outVal_) / diff;
		}
		else
		{
			const float alpha = (inVal - prevPoint.inVal_) / diff;
			ASSERT(alpha >= 0.0f && alpha <= 1.0f);

			return CubicInterpDerivative(prevPoint.outVal_, prevPoint.leaveTangent_ * diff, nextPoint.outVal_, nextPoint.arriveTangent_ * diff, alpha) / diff;
		}
	}
	else
	{
		// Derivative of a constant is zero
		return T(ForceInit);
	}
}


template< class T >
T InterpCurve<T>::EvalSecondDerivative(const float inVal, const T& default) const
{
	const Int32 numPoints = points_.Size();
	const Int32 lastPoint = numPoints - 1;

	// If no point in curve, return the default value we passed in.
	if (numPoints == 0)
	{
		return default;
	}

	// Binary search to find index of lower bound of input value
	const Int32 index = GetPointIndexForInputValue(inVal);

	// If before the first point, return 0
	if (index == -1)
	{
		return T(ForceInit);
	}

	// If on or beyond the last point, return 0
	if (index == lastPoint)
	{
		if (!isLooped_ || (inVal >= points_[lastPoint].inVal_ + loopKeyOffset_))
		{
			return T(ForceInit);
		}
	}

	// Somewhere within curve range - interpolate.
	ASSERT(index >= 0 && ((isLooped_ && index < numPoints) || (!isLooped_ && index < lastPoint)));
	const bool bLoopSegment = (isLooped_ && index == lastPoint);
	const Int32 nextIndex = bLoopSegment ? 0 : (index + 1);

	const auto& prevPoint = points_[index];
	const auto& nextPoint = points_[nextIndex];

	const float diff = bLoopSegment ? loopKeyOffset_ : (nextPoint.inVal_ - prevPoint.inVal_);

	if (diff > 0.0f && prevPoint.interpMode_ != CIM_Constant)
	{
		if (prevPoint.interpMode_ == CIM_Linear)
		{
			// No change in tangent, return 0.
			return T(ForceInit);
		}
		else
		{
			const float alpha = (inVal - prevPoint.inVal_) / diff;
			ASSERT(alpha >= 0.0f && alpha <= 1.0f);

			return CubicInterpSecondDerivative(prevPoint.outVal_, prevPoint.leaveTangent_ * diff, nextPoint.outVal_, nextPoint.arriveTangent_ * diff, alpha) / (diff * diff);
		}
	}
	else
	{
		// Second derivative of a constant is zero
		return T(ForceInit);
	}
}


template< class T >
float InterpCurve<T>::InaccurateFindNearest(const T &pointInSpace, float& outDistanceSq) const							// LWC_TODO: Precision loss
{
	float outSegment;
	return InaccurateFindNearest(pointInSpace, outDistanceSq, outSegment);
}

template< class T >
float InterpCurve<T>::InaccurateFindNearest(const T &pointInSpace, float& outDistanceSq, float& outSegment) const		// LWC_TODO: Precision loss
{
	const Int32 numPoints = points_.Size();
	const Int32 numSegments = isLooped_ ? numPoints : numPoints - 1;

	if (numPoints > 1)
	{
		float bestDistanceSq;
		float bestResult = InaccurateFindNearestOnSegment(pointInSpace, 0, bestDistanceSq);
		float bestSegment = 0;
		for (Int32 segment = 1; segment < numSegments; ++segment)
		{
			float localDistanceSq;
			float localResult = InaccurateFindNearestOnSegment(pointInSpace, segment, localDistanceSq);
			if (localDistanceSq < bestDistanceSq)
			{
				bestDistanceSq = localDistanceSq;
				bestResult = localResult;
				bestSegment = (float)segment;
			}
		}
		outDistanceSq = bestDistanceSq;
		outSegment = bestSegment;
		return bestResult;
	}

	if (numPoints == 1)
	{
		outDistanceSq = static_cast<float>((pointInSpace - points_[0].outVal_).LengthSquared());
		outSegment = 0;
		return points_[0].inVal_;
	}

	return 0.0f;
}


template< class T >
float InterpCurve<T>::InaccurateFindNearestOnSegment(const T& pointInSpace, Int32 ptIdx, float& outSquaredDistance) const		// LWC_TODO: Precision loss
{
	const Int32 numPoints = points_.Size();
	const Int32 lastPoint = numPoints - 1;
	const Int32 nextPtIdx = (isLooped_ && ptIdx == lastPoint) ? 0 : (ptIdx + 1);
	ASSERT(ptIdx >= 0 && ((isLooped_ && ptIdx < numPoints) || (!isLooped_ && ptIdx < lastPoint)));

	const float nextInVal = (isLooped_ && ptIdx == lastPoint) ? (points_[lastPoint].inVal_ + loopKeyOffset_) : points_[nextPtIdx].inVal_;

	if (CIM_Constant == points_[ptIdx].interpMode_)
	{
		const float Distance1 = static_cast<float>((points_[ptIdx].outVal_ - pointInSpace).LengthSquared());
		const float Distance2 = static_cast<float>((points_[nextPtIdx].outVal_ - pointInSpace).LengthSquared());
		if (Distance1 < Distance2)
		{
			outSquaredDistance = Distance1;
			return points_[ptIdx].inVal_;
		}
		outSquaredDistance = Distance2;
		return nextInVal;
	}

	const float diff = nextInVal - points_[ptIdx].inVal_;
	if (CIM_Linear == points_[ptIdx].interpMode_)
	{
		// like in function: ClosestPointOnLine
		const float A = static_cast<float>((points_[ptIdx].outVal_ - pointInSpace) | (points_[nextPtIdx].outVal_ - points_[ptIdx].outVal_));
		const float B = static_cast<float>((points_[nextPtIdx].outVal_ - points_[ptIdx].outVal_).LengthSquared());
		const float V = Clamp(-A / B, 0.f, 1.f);
		outSquaredDistance = static_cast<float>((Lerp(points_[ptIdx].outVal_, points_[nextPtIdx].outVal_, V) - pointInSpace).LengthSquared());
		return V * diff + points_[ptIdx].inVal_;
	}

	{
		const Int32 pointsChecked = 3;
		const Int32 iterationNum = 3;
		const float Scale = 0.75;

		// Newton's methods is repeated 3 times, starting with t = 0, 0.5, 1.
		float valuesT[pointsChecked];
		valuesT[0] = 0.0f;
		valuesT[1] = 0.5f;
		valuesT[2] = 1.0f;

		T initialPoints[pointsChecked];
		initialPoints[0] = points_[ptIdx].outVal_;
		initialPoints[1] = CubicInterp(points_[ptIdx].outVal_, points_[ptIdx].leaveTangent_ * diff, points_[nextPtIdx].outVal_, points_[nextPtIdx].arriveTangent_ * diff, valuesT[1]);
		initialPoints[2] = points_[nextPtIdx].outVal_;

		float distancesSq[pointsChecked];

		for (Int32 point = 0; point < pointsChecked; ++point)
		{
			//Algorithm explanation: http://permalink.gmane.org/gmane.games.devel.sweng/8285
			T foundPoint = initialPoints[point];
			float lastMove = 1.0f;
			for (Int32 iter = 0; iter < iterationNum; ++iter)
			{
				const T lastBestTangent = CubicInterpDerivative(points_[ptIdx].outVal_, points_[ptIdx].leaveTangent_ * diff, points_[nextPtIdx].outVal_, points_[nextPtIdx].arriveTangent_ * diff, valuesT[point]);
				const T delta = (pointInSpace - foundPoint);
				float move = static_cast<float>((lastBestTangent | delta) / lastBestTangent.LengthSquared());
				move = Clamp(Move, -lastMove * Scale, lastMove * Scale);
				valuesT[point] += move;
				valuesT[point] = Clamp(valuesT[point], 0.0f, 1.0f);
				lastMove = Abs(move);
				foundPoint = CubicInterp(points_[ptIdx].outVal_, points_[ptIdx].leaveTangent_ * diff, points_[nextPtIdx].outVal_, points_[nextPtIdx].arriveTangent_ * diff, valuesT[point]);
			}
			distancesSq[point] = static_cast<float>((foundPoint - pointInSpace).LengthSquared());
			valuesT[point] = valuesT[point] * diff + points_[ptIdx].inVal_;
		}

		if (distancesSq[0] <= distancesSq[1] && distancesSq[0] <= distancesSq[2])
		{
			outSquaredDistance = distancesSq[0];
			return valuesT[0];
		}
		if (distancesSq[1] <= distancesSq[2])
		{
			outSquaredDistance = distancesSq[1];
			return valuesT[1];
		}
		outSquaredDistance = distancesSq[2];
		return valuesT[2];
	}
}


template< class T >
void InterpCurve<T>::AutoSetTangents(float tension, bool stationaryEndpoints)
{
	const Int32 numPoints = points_.Size();
	const Int32 lastPoint = numPoints - 1;

	// Iterate over all points in this InterpCurve
	for (Int32 pointIndex = 0; pointIndex < numPoints; pointIndex++)
	{
		const Int32 prevIndex = (pointIndex == 0) ? (isLooped_ ? lastPoint : 0) : (pointIndex - 1);
		const Int32 nextIndex = (pointIndex == lastPoint) ? (isLooped_ ? 0 : lastPoint) : (pointIndex + 1);

		auto& thisPoint = points_[pointIndex];
		const auto& prevPoint = points_[prevIndex];
		const auto& nextPoint = points_[nextIndex];

		if (thisPoint.interpMode_ == CIM_CurveAuto || thisPoint.interpMode_ == CIM_CurveAutoClamped)
		{
			if (stationaryEndpoints && (pointIndex == 0 || (pointIndex == lastPoint && !isLooped_)))
			{
				// Start and end points get zero tangents if stationaryEndpoints is true
				thisPoint.arriveTangent_ = T();
				thisPoint.leaveTangent_ = T();
			}
			else if (prevPoint.IsCurveKey())
			{
				const bool bWantClamping = (thisPoint.interpMode_ == CIM_CurveAutoClamped);
				T tangent;

				const float prevTime = (isLooped_ && pointIndex == 0) ? (thisPoint.inVal_ - loopKeyOffset_) : prevPoint.inVal_;
				const float nextTime = (isLooped_ && pointIndex == lastPoint) ? (thisPoint.inVal_ + loopKeyOffset_) : nextPoint.inVal_;

				ComputeCurveTangent(
					prevTime,			// Previous time
					prevPoint.outVal_,	// Previous point
					thisPoint.inVal_,	// Current time
					thisPoint.outVal_,	// Current point
					nextTime,			// Next time
					nextPoint.outVal_,	// Next point
					tension,			// tension
					bWantClamping,		// Want clamping?
					tangent);			// Out

				thisPoint.arriveTangent_ = tangent;
				thisPoint.leaveTangent_ = tangent;
			}
			else
			{
				// Following on from a line or constant; set curve tangent equal to that so there are no discontinuities
				thisPoint.arriveTangent_ = prevPoint.arriveTangent_;
				thisPoint.leaveTangent_ = prevPoint.leaveTangent_;
			}
		}
		else if (thisPoint.interpMode_ == CIM_Linear)
		{
			T tangent = nextPoint.outVal_ - thisPoint.outVal_;
			thisPoint.arriveTangent_ = tangent;
			thisPoint.leaveTangent_ = tangent;
		}
		else if (thisPoint.interpMode_ == CIM_Constant)
		{
			thisPoint.arriveTangent_ = T();
			thisPoint.leaveTangent_ = T();
		}
	}
}


template< class T >
void InterpCurve<T>::CalcBounds(T& outMin, T& outMax, const T& default) const
{
	const Int32 numPoints = points_.Size();

	if (numPoints == 0)
	{
		outMin = default;
		outMax = default;
	}
	else if (numPoints == 1)
	{
		outMin = points_[0].outVal_;
		outMax = points_[0].outVal_;
	}
	else
	{
		outMin = points_[0].outVal_;
		outMax = points_[0].outVal_;

		const Int32 numSegments = isLooped_ ? numPoints : (numPoints - 1);

		for (Int32 index = 0; index < numSegments; index++)
		{
			const Int32 nextIndex = (index == numPoints - 1) ? 0 : (index + 1);
			CurveFindIntervalBounds(points_[index], points_[nextIndex], outMin, outMax, 0.0f);
		}
	}
}

/* Common type definitions
 *****************************************************************************/

#define DEFINE_INTERPCURVE_WRAPPER_STRUCT(Name, ElementType) \
	struct Name : InterpCurve<ElementType> \
	{ \
	private: \
		typedef InterpCurve<ElementType> Super; \
	 \
	public: \
		Name() \
			: Super() \
		{ \
		} \
		 \
		Name(const Super& Other) \
			: Super( Other ) \
		{ \
		} \
	};

DEFINE_INTERPCURVE_WRAPPER_STRUCT(InterpCurveFloat,       float)
DEFINE_INTERPCURVE_WRAPPER_STRUCT(InterpCurveVector2D,    Vector2)
DEFINE_INTERPCURVE_WRAPPER_STRUCT(InterpCurveVector,      Vector3)
DEFINE_INTERPCURVE_WRAPPER_STRUCT(InterpCurveQuat,        Quaternion)
DEFINE_INTERPCURVE_WRAPPER_STRUCT(InterpCurveTwoVectors,  TwoVectors)
DEFINE_INTERPCURVE_WRAPPER_STRUCT(InterpCurveLinearColor, Color)

}
