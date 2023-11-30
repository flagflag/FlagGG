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
	Int32 i = 0; for (i = 0; i < points_.Size() && points_[i].inVal < inVal; i++);
	points_.InsertUninitialized(i);
	points_[i] = InterpCurvePoint<T>(inVal, outVal);
	return i;
}


template< class T > 
Int32 InterpCurve<T>::MovePoint(Int32 pointIndex, float newInVal)
{
	if (pointIndex < 0 || pointIndex >= points_.Size())
		return pointIndex;

	const T outVal = points_[pointIndex].outVal;
	const InterpCurveMode Mode = points_[pointIndex].InterpMode;
	const T ArriveTan = points_[pointIndex].ArriveTangent;
	const T LeaveTan = points_[pointIndex].LeaveTangent;

	points_.Erase(pointIndex);

	const Int32 NewPointIndex = AddPoint(newInVal, outVal);
	points_[NewPointIndex].InterpMode = Mode;
	points_[NewPointIndex].ArriveTangent = ArriveTan;
	points_[NewPointIndex].LeaveTangent = LeaveTan;

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

	const float LastInKey = points_.Back().inVal;
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
	const Int32 NumPoints = points_.Size();
	const Int32 LastPoint = NumPoints - 1;

	ASSERT(NumPoints > 0);

	if (inValue < points_[0].inVal)
	{
		return -1;
	}

	if (inValue >= points_[LastPoint].inVal)
	{
		return LastPoint;
	}

	Int32 MinIndex = 0;
	Int32 MaxIndex = NumPoints;

	while (MaxIndex - MinIndex > 1)
	{
		Int32 MidIndex = (MinIndex + MaxIndex) / 2;

		if (points_[MidIndex].inVal <= inValue)
		{
			MinIndex = MidIndex;
		}
		else
		{
			MaxIndex = MidIndex;
		}
	}

	return MinIndex;
}


template< class T >
T InterpCurve<T>::Eval(const float inVal, const T& default) const
{
	const Int32 NumPoints = points_.Size();
	const Int32 LastPoint = NumPoints - 1;

	// If no point in curve, return the default value we passed in.
	if (NumPoints == 0)
	{
		return default;
	}

	// Binary search to find index of lower bound of input value
	const Int32 Index = GetPointIndexForInputValue(inVal);

	// If before the first point, return its value
	if (Index == -1)
	{
		return points_[0].outVal;
	}

	// If on or beyond the last point, return its value.
	if (Index == LastPoint)
	{
		if (!isLooped_)
		{
			return points_[LastPoint].outVal;
		}
		else if (inVal >= points_[LastPoint].inVal + loopKeyOffset_)
		{
			// Looped spline: last point is the same as the first point
			return points_[0].outVal;
		}
	}

	// Somewhere within curve range - interpolate.
	ASSERT(Index >= 0 && ((isLooped_ && Index < NumPoints) || (!isLooped_ && Index < LastPoint)));
	const bool bLoopSegment = (isLooped_ && Index == LastPoint);
	const Int32 NextIndex = bLoopSegment ? 0 : (Index + 1);

	const auto& PrevPoint = points_[Index];
	const auto& NextPoint = points_[NextIndex];

	const float Diff = bLoopSegment ? loopKeyOffset_ : (NextPoint.inVal - PrevPoint.inVal);

	if (Diff > 0.0f && PrevPoint.InterpMode != CIM_Constant)
	{
		const float Alpha = (inVal - PrevPoint.inVal) / Diff;
		ASSERT(Alpha >= 0.0f && Alpha <= 1.0f);

		if (PrevPoint.InterpMode == CIM_Linear)
		{
			return FMath::Lerp(PrevPoint.outVal, NextPoint.outVal, Alpha);
		}
		else
		{
			return FMath::CubicInterp(PrevPoint.outVal, PrevPoint.LeaveTangent * Diff, NextPoint.outVal, NextPoint.ArriveTangent * Diff, Alpha);
		}
	}
	else
	{
		return points_[Index].outVal;
	}
}


template< class T >
T InterpCurve<T>::EvalDerivative(const float inVal, const T& default) const
{
	const Int32 NumPoints = points_.Size();
	const Int32 LastPoint = NumPoints - 1;

	// If no point in curve, return the default value we passed in.
	if (NumPoints == 0)
	{
		return default;
	}

	// Binary search to find index of lower bound of input value
	const Int32 Index = GetPointIndexForInputValue(inVal);

	// If before the first point, return its tangent value
	if (Index == -1)
	{
		return points_[0].LeaveTangent;
	}

	// If on or beyond the last point, return its tangent value.
	if (Index == LastPoint)
	{
		if (!isLooped_)
		{
			return points_[LastPoint].ArriveTangent;
		}
		else if (inVal >= points_[LastPoint].inVal + loopKeyOffset_)
		{
			// Looped spline: last point is the same as the first point
			return points_[0].ArriveTangent;
		}
	}

	// Somewhere within curve range - interpolate.
	ASSERT(Index >= 0 && ((isLooped_ && Index < NumPoints) || (!isLooped_ && Index < LastPoint)));
	const bool bLoopSegment = (isLooped_ && Index == LastPoint);
	const Int32 NextIndex = bLoopSegment ? 0 : (Index + 1);

	const auto& PrevPoint = points_[Index];
	const auto& NextPoint = points_[NextIndex];

	const float Diff = bLoopSegment ? loopKeyOffset_ : (NextPoint.inVal - PrevPoint.inVal);

	if (Diff > 0.0f && PrevPoint.InterpMode != CIM_Constant)
	{
		if (PrevPoint.InterpMode == CIM_Linear)
		{
			return (NextPoint.outVal - PrevPoint.outVal) / Diff;
		}
		else
		{
			const float Alpha = (inVal - PrevPoint.inVal) / Diff;
			ASSERT(Alpha >= 0.0f && Alpha <= 1.0f);

			return FMath::CubicInterpDerivative(PrevPoint.outVal, PrevPoint.LeaveTangent * Diff, NextPoint.outVal, NextPoint.ArriveTangent * Diff, Alpha) / Diff;
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
	const Int32 NumPoints = points_.Size();
	const Int32 LastPoint = NumPoints - 1;

	// If no point in curve, return the default value we passed in.
	if (NumPoints == 0)
	{
		return default;
	}

	// Binary search to find index of lower bound of input value
	const Int32 Index = GetPointIndexForInputValue(inVal);

	// If before the first point, return 0
	if (Index == -1)
	{
		return T(ForceInit);
	}

	// If on or beyond the last point, return 0
	if (Index == LastPoint)
	{
		if (!isLooped_ || (inVal >= points_[LastPoint].inVal + loopKeyOffset_))
		{
			return T(ForceInit);
		}
	}

	// Somewhere within curve range - interpolate.
	ASSERT(Index >= 0 && ((isLooped_ && Index < NumPoints) || (!isLooped_ && Index < LastPoint)));
	const bool bLoopSegment = (isLooped_ && Index == LastPoint);
	const Int32 NextIndex = bLoopSegment ? 0 : (Index + 1);

	const auto& PrevPoint = points_[Index];
	const auto& NextPoint = points_[NextIndex];

	const float Diff = bLoopSegment ? loopKeyOffset_ : (NextPoint.inVal - PrevPoint.inVal);

	if (Diff > 0.0f && PrevPoint.InterpMode != CIM_Constant)
	{
		if (PrevPoint.InterpMode == CIM_Linear)
		{
			// No change in tangent, return 0.
			return T(ForceInit);
		}
		else
		{
			const float Alpha = (inVal - PrevPoint.inVal) / Diff;
			ASSERT(Alpha >= 0.0f && Alpha <= 1.0f);

			return FMath::CubicInterpSecondDerivative(PrevPoint.outVal, PrevPoint.LeaveTangent * Diff, NextPoint.outVal, NextPoint.ArriveTangent * Diff, Alpha) / (Diff * Diff);
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
	const Int32 NumPoints = points_.Size();
	const Int32 NumSegments = isLooped_ ? NumPoints : NumPoints - 1;

	if (NumPoints > 1)
	{
		float BestDistanceSq;
		float BestResult = InaccurateFindNearestOnSegment(pointInSpace, 0, BestDistanceSq);
		float BestSegment = 0;
		for (Int32 Segment = 1; Segment < NumSegments; ++Segment)
		{
			float LocalDistanceSq;
			float LocalResult = InaccurateFindNearestOnSegment(pointInSpace, Segment, LocalDistanceSq);
			if (LocalDistanceSq < BestDistanceSq)
			{
				BestDistanceSq = LocalDistanceSq;
				BestResult = LocalResult;
				BestSegment = (float)Segment;
			}
		}
		outDistanceSq = BestDistanceSq;
		outSegment = BestSegment;
		return BestResult;
	}

	if (NumPoints == 1)
	{
		outDistanceSq = static_cast<float>((pointInSpace - points_[0].outVal).SizeSquared());
		outSegment = 0;
		return points_[0].inVal;
	}

	return 0.0f;
}


template< class T >
float InterpCurve<T>::InaccurateFindNearestOnSegment(const T& pointInSpace, Int32 ptIdx, float& outSquaredDistance) const		// LWC_TODO: Precision loss
{
	const Int32 NumPoints = points_.Size();
	const Int32 LastPoint = NumPoints - 1;
	const Int32 NextPtIdx = (isLooped_ && ptIdx == LastPoint) ? 0 : (ptIdx + 1);
	ASSERT(ptIdx >= 0 && ((isLooped_ && ptIdx < NumPoints) || (!isLooped_ && ptIdx < LastPoint)));

	const float NextInVal = (isLooped_ && ptIdx == LastPoint) ? (points_[LastPoint].inVal + loopKeyOffset_) : points_[NextPtIdx].inVal;

	if (CIM_Constant == points_[ptIdx].InterpMode)
	{
		const float Distance1 = static_cast<float>((points_[ptIdx].outVal - pointInSpace).SizeSquared());
		const float Distance2 = static_cast<float>((points_[NextPtIdx].outVal - pointInSpace).SizeSquared());
		if (Distance1 < Distance2)
		{
			outSquaredDistance = Distance1;
			return points_[ptIdx].inVal;
		}
		outSquaredDistance = Distance2;
		return NextInVal;
	}

	const float Diff = NextInVal - points_[ptIdx].inVal;
	if (CIM_Linear == points_[ptIdx].InterpMode)
	{
		// like in function: FMath::ClosestPointOnLine
		const float A = static_cast<float>((points_[ptIdx].outVal - pointInSpace) | (points_[NextPtIdx].outVal - points_[ptIdx].outVal));
		const float B = static_cast<float>((points_[NextPtIdx].outVal - points_[ptIdx].outVal).SizeSquared());
		const float V = FMath::Clamp(-A / B, 0.f, 1.f);
		outSquaredDistance = static_cast<float>((FMath::Lerp(points_[ptIdx].outVal, points_[NextPtIdx].outVal, V) - pointInSpace).SizeSquared());
		return V * Diff + points_[ptIdx].inVal;
	}

	{
		const Int32 PointsChecked = 3;
		const Int32 IterationNum = 3;
		const float Scale = 0.75;

		// Newton's methods is repeated 3 times, starting with t = 0, 0.5, 1.
		float ValuesT[PointsChecked];
		ValuesT[0] = 0.0f;
		ValuesT[1] = 0.5f;
		ValuesT[2] = 1.0f;

		T InitialPoints[PointsChecked];
		InitialPoints[0] = points_[ptIdx].outVal;
		InitialPoints[1] = FMath::CubicInterp(points_[ptIdx].outVal, points_[ptIdx].LeaveTangent * Diff, points_[NextPtIdx].outVal, points_[NextPtIdx].ArriveTangent * Diff, ValuesT[1]);
		InitialPoints[2] = points_[NextPtIdx].outVal;

		float DistancesSq[PointsChecked];

		for (Int32 point = 0; point < PointsChecked; ++point)
		{
			//Algorithm explanation: http://permalink.gmane.org/gmane.games.devel.sweng/8285
			T FoundPoint = InitialPoints[point];
			float LastMove = 1.0f;
			for (Int32 iter = 0; iter < IterationNum; ++iter)
			{
				const T LastBestTangent = FMath::CubicInterpDerivative(points_[ptIdx].outVal, points_[ptIdx].LeaveTangent * Diff, points_[NextPtIdx].outVal, points_[NextPtIdx].ArriveTangent * Diff, ValuesT[point]);
				const T Delta = (pointInSpace - FoundPoint);
				float Move = static_cast<float>((LastBestTangent | Delta) / LastBestTangent.SizeSquared());
				Move = FMath::Clamp(Move, -LastMove*Scale, LastMove*Scale);
				ValuesT[point] += Move;
				ValuesT[point] = FMath::Clamp(ValuesT[point], 0.0f, 1.0f);
				LastMove = FMath::Abs(Move);
				FoundPoint = FMath::CubicInterp(points_[ptIdx].outVal, points_[ptIdx].LeaveTangent * Diff, points_[NextPtIdx].outVal, points_[NextPtIdx].ArriveTangent * Diff, ValuesT[point]);
			}
			DistancesSq[point] = static_cast<float>((FoundPoint - pointInSpace).SizeSquared());
			ValuesT[point] = ValuesT[point] * Diff + points_[ptIdx].inVal;
		}

		if (DistancesSq[0] <= DistancesSq[1] && DistancesSq[0] <= DistancesSq[2])
		{
			outSquaredDistance = DistancesSq[0];
			return ValuesT[0];
		}
		if (DistancesSq[1] <= DistancesSq[2])
		{
			outSquaredDistance = DistancesSq[1];
			return ValuesT[1];
		}
		outSquaredDistance = DistancesSq[2];
		return ValuesT[2];
	}
}


template< class T >
void InterpCurve<T>::AutoSetTangents(float tension, bool stationaryEndpoints)
{
	const Int32 NumPoints = points_.Size();
	const Int32 LastPoint = NumPoints - 1;

	// Iterate over all points in this InterpCurve
	for (Int32 pointIndex = 0; pointIndex < NumPoints; pointIndex++)
	{
		const Int32 PrevIndex = (pointIndex == 0) ? (isLooped_ ? LastPoint : 0) : (pointIndex - 1);
		const Int32 NextIndex = (pointIndex == LastPoint) ? (isLooped_ ? 0 : LastPoint) : (pointIndex + 1);

		auto& ThisPoint = points_[pointIndex];
		const auto& PrevPoint = points_[PrevIndex];
		const auto& NextPoint = points_[NextIndex];

		if (ThisPoint.InterpMode == CIM_CurveAuto || ThisPoint.InterpMode == CIM_CurveAutoClamped)
		{
			if (stationaryEndpoints && (pointIndex == 0 || (pointIndex == LastPoint && !isLooped_)))
			{
				// Start and end points get zero tangents if stationaryEndpoints is true
				ThisPoint.ArriveTangent = T(ForceInit);
				ThisPoint.LeaveTangent = T(ForceInit);
			}
			else if (PrevPoint.IsCurveKey())
			{
				const bool bWantClamping = (ThisPoint.InterpMode == CIM_CurveAutoClamped);
				T Tangent;

				const float PrevTime = (isLooped_ && pointIndex == 0) ? (ThisPoint.inVal - loopKeyOffset_) : PrevPoint.inVal;
				const float NextTime = (isLooped_ && pointIndex == LastPoint) ? (ThisPoint.inVal + loopKeyOffset_) : NextPoint.inVal;

				ComputeCurveTangent(
					PrevTime,			// Previous time
					PrevPoint.outVal,	// Previous point
					ThisPoint.inVal,	// Current time
					ThisPoint.outVal,	// Current point
					NextTime,			// Next time
					NextPoint.outVal,	// Next point
					tension,			// tension
					bWantClamping,		// Want clamping?
					Tangent);			// Out

				ThisPoint.ArriveTangent = Tangent;
				ThisPoint.LeaveTangent = Tangent;
			}
			else
			{
				// Following on from a line or constant; set curve tangent equal to that so there are no discontinuities
				ThisPoint.ArriveTangent = PrevPoint.ArriveTangent;
				ThisPoint.LeaveTangent = PrevPoint.LeaveTangent;
			}
		}
		else if (ThisPoint.InterpMode == CIM_Linear)
		{
			T Tangent = NextPoint.outVal - ThisPoint.outVal;
			ThisPoint.ArriveTangent = Tangent;
			ThisPoint.LeaveTangent = Tangent;
		}
		else if (ThisPoint.InterpMode == CIM_Constant)
		{
			ThisPoint.ArriveTangent = T(ForceInit);
			ThisPoint.LeaveTangent = T(ForceInit);
		}
	}
}


template< class T >
void InterpCurve<T>::CalcBounds(T& outMin, T& outMax, const T& default) const
{
	const Int32 NumPoints = points_.Size();

	if (NumPoints == 0)
	{
		outMin = default;
		outMax = default;
	}
	else if (NumPoints == 1)
	{
		outMin = points_[0].outVal;
		outMax = points_[0].outVal;
	}
	else
	{
		outMin = points_[0].outVal;
		outMax = points_[0].outVal;

		const Int32 NumSegments = isLooped_ ? NumPoints : (NumPoints - 1);

		for (Int32 Index = 0; Index < NumSegments; Index++)
		{
			const Int32 NextIndex = (Index == NumPoints - 1) ? 0 : (Index + 1);
			CurveFindIntervalBounds(points_[Index], points_[NextIndex], outMin, outMax, 0.0f);
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
DEFINE_INTERPCURVE_WRAPPER_STRUCT(InterpCurveTwoVectors,  TwoVector3)
DEFINE_INTERPCURVE_WRAPPER_STRUCT(InterpCurveLinearColor, Color)

}
