#include "InterpCurvePoint.h"

namespace FlagGG
{

float ClampFloatTangent(float prevPointVal, float prevTime, float curPointVal, float curTime, float nextPointVal, float nextTime)
{
	const float PrevToNextTimeDiff = Max<float>(KINDA_SMALL_NUMBER, nextTime - prevTime);
	const float PrevToCurTimeDiff = Max<float>(KINDA_SMALL_NUMBER, curTime - prevTime);
	const float CurToNextTimeDiff = Max<float>(KINDA_SMALL_NUMBER, nextTime - curTime);

	float OutTangentVal = 0.0f;

	const float PrevToNextHeightDiff = nextPointVal - prevPointVal;
	const float PrevToCurHeightDiff = curPointVal - prevPointVal;
	const float CurToNextHeightDiff = nextPointVal - curPointVal;

	// Check to see if the current point is crest
	if ((PrevToCurHeightDiff >= 0.0f && CurToNextHeightDiff <= 0.0f) ||
		(PrevToCurHeightDiff <= 0.0f && CurToNextHeightDiff >= 0.0f))
	{
		// Neighbor points are both both on the same side, so zero out the tangent
		OutTangentVal = 0.0f;
	}
	else
	{
		// The three points form a slope

		// Constants
		const float ClampThreshold = 0.333f;

		// Compute height deltas
		const float CurToNextTangent = CurToNextHeightDiff / CurToNextTimeDiff;
		const float PrevToCurTangent = PrevToCurHeightDiff / PrevToCurTimeDiff;
		const float PrevToNextTangent = PrevToNextHeightDiff / PrevToNextTimeDiff;

		// Default to not clamping
		const float UnclampedTangent = PrevToNextTangent;
		float ClampedTangent = UnclampedTangent;

		const float LowerClampThreshold = ClampThreshold;
		const float UpperClampThreshold = 1.0f - ClampThreshold;

		// @todo: Would we get better results using percentange of TIME instead of HEIGHT?
		const float CurHeightAlpha = PrevToCurHeightDiff / PrevToNextHeightDiff;

		if (PrevToNextHeightDiff > 0.0f)
		{
			if (CurHeightAlpha < LowerClampThreshold)
			{
				// 1.0 = maximum clamping (flat), 0.0 = minimal clamping (don't touch)
				const float ClampAlpha = 1.0f - CurHeightAlpha / ClampThreshold;
				const float LowerClamp = Lerp(PrevToNextTangent, PrevToCurTangent, ClampAlpha);
				ClampedTangent = Min(ClampedTangent, LowerClamp);
			}

			if (CurHeightAlpha > UpperClampThreshold)
			{
				// 1.0 = maximum clamping (flat), 0.0 = minimal clamping (don't touch)
				const float ClampAlpha = (CurHeightAlpha - UpperClampThreshold) / ClampThreshold;
				const float UpperClamp = Lerp(PrevToNextTangent, CurToNextTangent, ClampAlpha);
				ClampedTangent = Min(ClampedTangent, UpperClamp);
			}
		}
		else
		{

			if (CurHeightAlpha < LowerClampThreshold)
			{
				// 1.0 = maximum clamping (flat), 0.0 = minimal clamping (don't touch)
				const float ClampAlpha = 1.0f - CurHeightAlpha / ClampThreshold;
				const float LowerClamp = Lerp(PrevToNextTangent, PrevToCurTangent, ClampAlpha);
				ClampedTangent = Max(ClampedTangent, LowerClamp);
			}

			if (CurHeightAlpha > UpperClampThreshold)
			{
				// 1.0 = maximum clamping (flat), 0.0 = minimal clamping (don't touch)
				const float ClampAlpha = (CurHeightAlpha - UpperClampThreshold) / ClampThreshold;
				const float UpperClamp = Lerp(PrevToNextTangent, CurToNextTangent, ClampAlpha);
				ClampedTangent = Max(ClampedTangent, UpperClamp);
			}
		}

		OutTangentVal = ClampedTangent;
	}

	return OutTangentVal;
}

template<typename T>
static void FindBounds(T& outMin, T& outMax, T start, T startLeaveTan, float startT, T end, T endArriveTan, float endT, bool curve)
{
	outMin = Min(start, end);
	outMax = Max(start, end);

	// Do we need to consider extermeties of a curve?
	if (curve)
	{
		// Scale tangents based on time interval, so this code matches the behaviour in FInterpCurve::Eval
		T Diff = endT - startT;
		startLeaveTan *= Diff;
		endArriveTan *= Diff;

		const T a = 6.f * start + 3.f * startLeaveTan + 3.f * endArriveTan - 6.f * end;
		const T b = -6.f * start - 4.f * startLeaveTan - 2.f * endArriveTan + 6.f * end;
		const T c = startLeaveTan;

		const T Discriminant = (b * b) - (4.f * a * c);
		if (Discriminant > 0.f && !IsNearlyZero(a)) // Solving doesn't work if a is zero, which usually indicates co-incident start and end, and zero tangents anyway
		{
			const T SqrtDisc = Sqrt(Discriminant);

			const T x0 = (-b + SqrtDisc) / (2.f * a); // x0 is the 'Alpha' ie between 0 and 1
			const T t0 = startT + x0 * (endT - startT); // Then t0 is the actual 'time' on the curve
			if (t0 > startT && t0 < endT)
			{
				const T Val = CubicInterp(start, startLeaveTan, end, endArriveTan, x0);

				outMin = Min(outMin, Val);
				outMax = Max(outMax, Val);
			}

			const T x1 = (-b - SqrtDisc) / (2.f * a);
			const T t1 = startT + x1 * (endT - startT);
			if (t1 > startT && t1 < endT)
			{
				const T Val = CubicInterp(start, startLeaveTan, end, endArriveTan, x1);

				outMin = Min(outMin, Val);
				outMax = Max(outMax, Val);
			}
		}
	}
}

void CurveFloatFindIntervalBounds( const InterpCurvePoint<float>& start, const InterpCurvePoint<float>& end, float& currentMin, float& currentMax )
{
	const bool bIsCurve = start.IsCurveKey();

	float outMin, outMax;

	FindBounds(outMin, outMax, start.outVal_, start.leaveTangent_, start.inVal_, end.outVal_, end.arriveTangent_, end.inVal_, bIsCurve);

	currentMin = Min( currentMin, outMin );
	currentMax = Max( currentMax, outMax );
}

void CurveVector2DFindIntervalBounds(const InterpCurvePoint<Vector2>& start, const InterpCurvePoint<Vector2>& end, Vector2& currentMin, Vector2& currentMax)
{
	const bool bIsCurve = start.IsCurveKey();

	Real outMin, outMax;

	FindBounds(outMin, outMax, start.outVal_.x_, start.leaveTangent_.x_, start.inVal_, end.outVal_.x_, end.arriveTangent_.x_, end.inVal_, bIsCurve);
	currentMin.x_ = Min(currentMin.x_, outMin);
	currentMax.x_ = Max(currentMax.x_, outMax);

	FindBounds(outMin, outMax, start.outVal_.y_, start.leaveTangent_.y_, start.inVal_, end.outVal_.y_, end.arriveTangent_.y_, end.inVal_, bIsCurve);
	currentMin.y_ = Min(currentMin.y_, outMin);
	currentMax.y_ = Max(currentMax.y_, outMax);
}

void CurveVectorFindIntervalBounds(const InterpCurvePoint<Vector3>& start, const InterpCurvePoint<Vector3>& end, Vector3& currentMin, Vector3& currentMax)
{
	const bool bIsCurve = start.IsCurveKey();

	Real outMin, outMax;

	FindBounds(outMin, outMax, start.outVal_.x_, start.leaveTangent_.x_, start.inVal_, end.outVal_.x_, end.arriveTangent_.x_, end.inVal_, bIsCurve);
	currentMin.x_ = Min(currentMin.x_, outMin);
	currentMax.x_ = Max(currentMax.x_, outMax);

	FindBounds(outMin, outMax, start.outVal_.y_, start.leaveTangent_.y_, start.inVal_, end.outVal_.y_, end.arriveTangent_.y_, end.inVal_, bIsCurve);
	currentMin.y_ = Min(currentMin.y_, outMin);
	currentMax.y_ = Max(currentMax.y_, outMax);

	FindBounds(outMin, outMax, start.outVal_.z_, start.leaveTangent_.z_, start.inVal_, end.outVal_.z_, end.arriveTangent_.z_, end.inVal_, bIsCurve);
	currentMin.z_ = Min(currentMin.z_, outMin);
	currentMax.z_ = Max(currentMax.z_, outMax);
}

void CurveTwoVectorsFindIntervalBounds(const InterpCurvePoint<TwoVectors>& start, const InterpCurvePoint<TwoVectors>& end, TwoVectors& currentMin, TwoVectors& currentMax)
{
	const bool bIsCurve = start.IsCurveKey();

	Real outMin, outMax;

	// Do the first curve
	FindBounds(outMin, outMax, start.outVal_.v1_.x_, start.leaveTangent_.v1_.x_, start.inVal_, end.outVal_.v1_.x_, end.arriveTangent_.v1_.x_, end.inVal_, bIsCurve);
	currentMin.v1_.x_ = Min(currentMin.v1_.x_, outMin);
	currentMax.v1_.x_ = Max(currentMax.v1_.x_, outMax);

	FindBounds(outMin, outMax, start.outVal_.v1_.y_, start.leaveTangent_.v1_.y_, start.inVal_, end.outVal_.v1_.y_, end.arriveTangent_.v1_.y_, end.inVal_, bIsCurve);
	currentMin.v1_.y_ = Min(currentMin.v1_.y_, outMin);
	currentMax.v1_.y_ = Max(currentMax.v1_.y_, outMax);

	FindBounds(outMin, outMax, start.outVal_.v1_.z_, start.leaveTangent_.v1_.z_, start.inVal_, end.outVal_.v1_.z_, end.arriveTangent_.v1_.z_, end.inVal_, bIsCurve);
	currentMin.v1_.z_ = Min(currentMin.v1_.z_, outMin);
	currentMax.v1_.z_ = Max(currentMax.v1_.z_, outMax);

	// Do the second curve
	FindBounds(outMin, outMax, start.outVal_.v2_.x_, start.leaveTangent_.v2_.x_, start.inVal_, end.outVal_.v2_.x_, end.arriveTangent_.v2_.x_, end.inVal_, bIsCurve);
	currentMin.v2_.x_ = Min(currentMin.v2_.x_, outMin);
	currentMax.v2_.x_ = Max(currentMax.v2_.x_, outMax);

	FindBounds(outMin, outMax, start.outVal_.v2_.y_, start.leaveTangent_.v2_.y_, start.inVal_, end.outVal_.v2_.y_, end.arriveTangent_.v2_.y_, end.inVal_, bIsCurve);
	currentMin.v2_.y_ = Min(currentMin.v2_.y_, outMin);
	currentMax.v2_.y_ = Max(currentMax.v2_.y_, outMax);

	FindBounds(outMin, outMax, start.outVal_.v2_.z_, start.leaveTangent_.v2_.z_, start.inVal_, end.outVal_.v2_.z_, end.arriveTangent_.v2_.z_, end.inVal_, bIsCurve);
	currentMin.v2_.z_ = Min(currentMin.v2_.z_, outMin);
	currentMax.v2_.z_ = Max(currentMax.v2_.z_, outMax);
}

void CurveLinearColorFindIntervalBounds(const InterpCurvePoint<Color>& start, const InterpCurvePoint<Color>& end, Color& currentMin, Color& currentMax)
{
	const bool bIsCurve = start.IsCurveKey();

	float outMin, outMax;

	FindBounds(outMin, outMax, start.outVal_.r_, start.leaveTangent_.r_, start.inVal_, end.outVal_.r_, end.arriveTangent_.r_, end.inVal_, bIsCurve);
	currentMin.r_ = Min(currentMin.r_, outMin);
	currentMax.r_ = Max(currentMax.r_, outMax);

	FindBounds(outMin, outMax, start.outVal_.g_, start.leaveTangent_.g_, start.inVal_, end.outVal_.g_, end.arriveTangent_.g_, end.inVal_, bIsCurve);
	currentMin.g_ = Min(currentMin.g_, outMin);
	currentMax.g_ = Max(currentMax.g_, outMax);

	FindBounds(outMin, outMax, start.outVal_.b_, start.leaveTangent_.b_, start.inVal_, end.outVal_.b_, end.arriveTangent_.b_, end.inVal_, bIsCurve);
	currentMin.b_ = Min(currentMin.b_, outMin);
	currentMax.b_ = Max(currentMax.b_, outMax);

	FindBounds(outMin, outMax, start.outVal_.a_, start.leaveTangent_.a_, start.inVal_, end.outVal_.a_, end.arriveTangent_.a_, end.inVal_, bIsCurve);
	currentMin.a_ = Min(currentMin.a_, outMin);
	currentMax.a_ = Max(currentMax.a_, outMax);
}

}
