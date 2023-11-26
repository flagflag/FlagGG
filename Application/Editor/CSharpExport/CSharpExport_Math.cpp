#include "CSharpExport.h"
#include "EditorExport.h"

#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>
#include <Math/Quaternion.h>
#include <Math/Color.h>
#include <Math/Ray.h>
#include <Math/Matrix4.h>
#include <Math/Plane.h>
#include <Log.h>

using namespace FlagGG;

// ========================================= Vector3 =============================================

static inline float Magnitude(const Vector3& inV) { return Sqrt(inV.DotProduct(inV)); }

static Vector3 OrthoNormalVectorFast(const Vector3& n)
{
	Vector3 res;
	if (Abs(n.z_) > F_OVERSQRT2)
	{
		// choose p in y-z plane
		float a = n.y_ * n.y_ + n.z_ * n.z_;
		float k = 1.0F / Sqrt(a);
		res.x_ = 0;
		res.y_ = -n.z_ * k;
		res.z_ = n.y_ * k;
	}
	else
	{
		// choose p in x-y plane
		float a = n.x_ * n.x_ + n.y_ * n.y_;
		float k = 1.0F / Sqrt(a);
		res.x_ = -n.y_ * k;
		res.y_ = n.x_ * k;
		res.z_ = 0;
	}
	return res;
}

static Vector3 SlerpImpl(const Vector3& lhs, const Vector3& rhs, float t)
{
	float lhsMag = Magnitude(lhs);
	float rhsMag = Magnitude(rhs);

	if (lhsMag < F_EPSILON || rhsMag < F_EPSILON)
		return lhs.Lerp(rhs, t);

	float lerpedMagnitude = Lerp(lhsMag, rhsMag, t);

	float dot = lhs.DotProduct(rhs) / (lhsMag * rhsMag);
	// direction is almost the same
	if (dot > 1.0F - F_EPSILON)
	{
		return lhs.Lerp(rhs, t);
	}
	// directions are almost opposite
	else if (dot < -1.0F + F_EPSILON)
	{
		Vector3 lhsNorm = lhs / lhsMag;
		Vector3 axis = OrthoNormalVectorFast(lhsNorm);
		// Matrix3 m;
		// m.SetAxisAngle(axis, PI * t);
		Matrix3 m = Quaternion(PI * t, axis).RotationMatrix();
		Vector3 slerped = m * lhsNorm;
		slerped *= lerpedMagnitude;
		return slerped;
	}
	// normal case
	else
	{
		Vector3 axis = lhs.CrossProduct(rhs);
		Vector3 lhsNorm = lhs / lhsMag;
		axis = axis.Normalized();
		float angle = Acos(dot) * t;

		// Matrix3 m;
		// m.SetAxisAngle(axis, angle);
		Matrix3 m = Quaternion(angle, axis).RotationMatrix();
		Vector3 slerped = m * lhsNorm;
		slerped *= lerpedMagnitude;
		return slerped;
	}
}

CSharp_API Vector3 MathExport_Slerp(const Vector3& lhs, const Vector3& rhs, float t)
{
	return SlerpImpl(lhs, rhs, Clamp(t, 0.f, 1.f));
}

CSharp_API Vector3 MathExport_SlerpUnclamped(const Vector3& lhs, const Vector3& rhs, float t)
{
	return SlerpImpl(lhs, rhs, t);
}

CSharp_API void MathExport_OrthoNormalize2(Vector3* inU, Vector3* inV)
{
	// compute u0
	float mag = Magnitude(*inU);
	if (mag > F_EPSILON)
		*inU /= mag;
	else
		*inU = Vector3(1.0F, 0.0F, 0.0F);

	// compute u1
	float dot0 = (*inU).DotProduct(*inV);
	*inV -= dot0 * *inU;
	mag = Magnitude(*inV);
	if (mag < F_EPSILON)
		*inV = OrthoNormalVectorFast(*inU);
	else
		*inV /= mag;
}

CSharp_API void MathExport_OrthoNormalize3(Vector3* inU, Vector3* inV, Vector3* inW)
{
	float mag = Magnitude(*inU);
	if (mag > F_EPSILON)
		*inU /= mag;
	else
		*inU = Vector3(1.0F, 0.0F, 0.0F);

	// compute u1
	float dot0 = (*inU).DotProduct(*inV);
	*inV -= dot0 * *inU;
	mag = Magnitude(*inV);
	if (mag > F_EPSILON)
		*inV /= mag;
	else
		*inV = OrthoNormalVectorFast(*inU);

	// compute u2
	float dot1 = (*inV).DotProduct(*inW);
	dot0 = (*inU).DotProduct(*inW);
	*inW -= dot0 * *inU + dot1 * *inV;
	mag = Magnitude(*inW);
	if (mag > F_EPSILON)
		*inW /= mag;
	else
		*inW = (*inU).CrossProduct(*inV);
}

static inline float SqrMagnitude(const Vector3& inV) { return inV.DotProduct(inV); }

static Vector3 MoveTowards(const Vector3& lhs, const Vector3& rhs, float clampedDistance)
{
	Vector3 delta = rhs - lhs;
	float sqrDelta = SqrMagnitude(delta);
	float sqrClampedDistance = clampedDistance * clampedDistance;
	if (sqrDelta > sqrClampedDistance)
	{
		float deltaMag = Sqrt(sqrDelta);
		if (deltaMag > F_EPSILON)
			return lhs + delta / deltaMag * clampedDistance;
		else
			return lhs;
	}
	else
		return rhs;
}

static inline float ClampedMove(float lhs, float rhs, float clampedDelta)
{
	float delta = rhs - lhs;
	if (delta > 0.0F)
		return lhs + Min(delta, clampedDelta);
	else
		return lhs - Min(-delta, clampedDelta);
}

CSharp_API Vector3 MathExport_RotateTowards(const Vector3& lhs, const Vector3& rhs, float angleMove, float magnitudeMove)
{
	float lhsMag = Magnitude(lhs);
	float rhsMag = Magnitude(rhs);

	// both vectors are non-zero
	if (lhsMag > F_EPSILON && rhsMag > F_EPSILON)
	{
		Vector3 lhsNorm = lhs / lhsMag;
		Vector3 rhsNorm = rhs / rhsMag;

		float dot = lhsNorm.DotProduct(rhsNorm);
		// direction is almost the same
		if (dot > 1.0F - F_EPSILON)
		{
			return MoveTowards(lhs, rhs, magnitudeMove);
		}
		// directions are almost opposite
		else if (dot < -1.0F + F_EPSILON)
		{
			Vector3 axis = OrthoNormalVectorFast(lhsNorm);
			// Matrix3 m;
			// m.SetAxisAngle(axis, angleMove);
			Matrix3 m = Quaternion(angleMove, axis).RotationMatrix();
			Vector3 rotated = m * lhsNorm;
			rotated *= ClampedMove(lhsMag, rhsMag, magnitudeMove);
			return rotated;
		}
		// normal case
		else
		{
			float angle = Acos(dot);
			Vector3 axis = lhsNorm.CrossProduct(rhsNorm).Normalized();
			// Matrix3 m;
			// m.SetAxisAngle(axis, std::min(angleMove, angle));
			Matrix3 m = Quaternion(Min(angleMove, angle), axis).RotationMatrix();
			Vector3 rotated = m * lhsNorm;
			rotated *= ClampedMove(lhsMag, rhsMag, magnitudeMove);
			return rotated;
		}
	}
	// at least one of the vectors is almost zero
	else
	{
		return MoveTowards(lhs, rhs, magnitudeMove);
	}
}

// ====================================== Mathf ===============================================

CSharp_API int MathExport_NextPowerOfTwo(int value)
{
	return NextPowerOfTwo(value);
}

CSharp_API int MathExport_ClosestPowerOfTwo(int v)
{
	int nextPower = NextPowerOfTwo(v);
	int prevPower = nextPower >> 1;
	if (v - prevPower < nextPower - v)
		return prevPower;
	else
		return nextPower;
}

CSharp_API bool MathExport_IsPowerOfTwo(int value)
{
	return IsPowerOfTwo(value);
}

CSharp_API float MathExport_GammaToLinearSpace(float value)
{
	if (value <= 0.04045F)
		return value / 12.92F;
	else if (value < 1.0F)
		return pow((value + 0.055F) / 1.055F, 2.4F);
	else if (value == 1.0F)
		return 1.0f;
	else
		return pow(value, 2.2f);
}

CSharp_API float MathExport_LinearToGammaSpace(float value)
{
	if (value <= 0.0F)
		return 0.0F;
	else if (value <= 0.0031308F)
		return 12.92F * value;
	else if (value < 1.0F)
		return 1.055F * powf(value, 0.4166667F) - 0.055F;
	else if (value == 1.0F)
		return 1.0f;
	else
		return powf(value, 2.2f);
}

CSharp_API Color MathExport_CorrelatedColorTemperatureToRGB(float temperature)
{
	float r, g, b;

	// Temperature must fall between 1000 and 40000 degrees
	// The fitting require to divide kelvin by 1000 (allow more precision)
	float kelvin = Clamp(temperature, 1000.f, 40000.f) / 1000.0f;
	float kelvin2 = kelvin * kelvin;

	// Using 6570 as a pivot is an approximation, pivot point for red is around 6580 and for blue and green around 6560.
	// Calculate each color in turn (Note, clamp is not really necessary as all value belongs to [0..1] but can help for extremum).
	// Red
	r = kelvin < 6.570f ? 1.0f : Clamp((1.35651f + 0.216422f * kelvin + 0.000633715f * kelvin2) / (-3.24223f + 0.918711f * kelvin), 0.0f, 1.0f);
	// Green
	g = kelvin < 6.570f ?
		Clamp((-399.809f + 414.271f * kelvin + 111.543f * kelvin2) / (2779.24f + 164.143f * kelvin + 84.7356f * kelvin2), 0.0f, 1.0f) :
		Clamp((1370.38f + 734.616f * kelvin + 0.689955f * kelvin2) / (-4625.69f + 1699.87f * kelvin), 0.0f, 1.0f);
	//Blue
	b = kelvin > 6.570f ? 1.0f : Clamp((348.963f - 523.53f * kelvin + 183.62f * kelvin2) / (2848.82f - 214.52f * kelvin + 78.8614f * kelvin2), 0.0f, 1.0f);

	return Color(r, g, b, 1.f);
}

CSharp_API unsigned short MathExport_FloatToHalf(float val)
{
	return 0u;
}

CSharp_API float MathExport_HalfToFloat(unsigned short val)
{
	return 0.f;
}

inline static float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

inline static float grad2(int hash, float x, float y)
{
	int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
	float u = h < 8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
		v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

static int p[] =
{
	151, 160, 137, 91, 90, 15,
	131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
	190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
	88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
	77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
	102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
	135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
	5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
	223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
	129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
	251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
	49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
	138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180,
	151, 160, 137, 91, 90, 15,
	131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
	190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
	88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
	77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
	102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
	135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
	5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
	223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
	129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
	251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
	49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
	138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

static float Noise(float x, float y)
{
	x = Abs(x);
	y = Abs(y);

	int floorX = int(x);
	int floorY = int(y);

	int X = floorX & 255;                  // FIND UNIT CUBE THAT
	int Y = floorY & 255;                  // CONTAINS POINT.
	x -= floorX;                                // FIND RELATIVE X,Y,Z
	y -= floorY;                                // OF POINT IN CUBE.
	float u = fade(std::min(x, 1.0f));          // COMPUTE FADE CURVES
	float v = fade(std::min(y, 1.0f));          // FOR EACH OF X,Y,Z.
	int A = p[X] + Y, AA = p[A], AB = p[A + 1],      // HASH COORDINATES OF
		B = p[X + 1] + Y, BA = p[B], BB = p[B + 1]; // THE 8 CUBE CORNERS,

	float res = Lerp(v, Lerp(u, grad2(p[AA], x, y),       // AND ADD
		grad2(p[BA], x - 1, y)),           // BLENDED
		Lerp(u, grad2(p[AB], x, y - 1),        // RESULTS
			grad2(p[BB], x - 1, y - 1)));    // FROM  8
	return res;
}

CSharp_API float MathExport_PerlinNoise(float x, float y)
{
	//-0.697 - 0.795 + 0.697
	float value = Noise(x, y);
	value = (value + 0.69F) / (0.793F + 0.69F);
	return value;
}

// ================================== Matrix4x4 ==========================================

CSharp_API Quaternion Matrix4x4Export_GetRotation(Matrix4* self)
{
	// TODO
	return Quaternion();
}

CSharp_API Vector3 Matrix4x4Export_GetLossyScale(Matrix4* self)
{
	// TODO
	return Vector3();
}

CSharp_API bool Matrix4x4Export_IsIdentity(Matrix4* self)
{
	// TODO
	return false;
}

CSharp_API float Matrix4x4Export_GetDeterminant(Matrix4* self)
{
	// TODO
	return 0.f;
}

struct FrustumPlanes
{
	float left;
	float right;
	float bottom;
	float top;
	float zNear;
	float zFar;
};

CSharp_API FrustumPlanes Matrix4x4Export_DecomposeProjection(Matrix4* self)
{
	// TODO
	return FrustumPlanes();
}
