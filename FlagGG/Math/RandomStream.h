// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/BaseTypes.h"
#include "Math/BoundingBox.h"
#include "Math/Vector3.h"
#include "Math/Matrix3.h"
#include "Container/Str.h"

namespace FlagGG
{

/**
* Implements a thread-safe SRand based RNG.
*
* Very bad quality in the lower bits. Don't use the modulus (%) operator.
*/
struct RandomStream
{
public:

	/**
		* Default constructor.
		*
		* The seed should be set prior to use.
		*/
	RandomStream()
		: initialSeed_(0)
		, seed_(0)
	{ }

	/**
		* Creates and initializes a new random stream from the specified seed value.
		*
		* @param InSeed The seed value.
		*/
	RandomStream(Int32 InSeed)
	{
		Initialize(InSeed);
	}

	/**
		* Creates and initializes a new random stream from the specified name.
		*
		* @note If NAME_None is provided, the stream will be seeded using the current time.
		* @param InName The name value from which the stream will be initialized.
		*/
	RandomStream(const String& InName)
	{
		Initialize(InName);
	}

public:

	/**
		* Initializes this random stream with the specified seed value.
		*
		* @param InSeed The seed value.
		*/
	void Initialize(Int32 InSeed)
	{
		initialSeed_ = InSeed;
		seed_ = UInt32(InSeed);
	}

	/**
		* Initializes this random stream using the specified name.
		*
		* @note If NAME_None is provided, the stream will be seeded using the current time.
		* @param InName The name value from which the stream will be initialized.
		*/
	void Initialize(const String& InName)
	{
		initialSeed_ = HashString(InName.CString());
		seed_ = UInt32(initialSeed_);
	}

	/**
		* Resets this random stream to the initial seed value.
		*/
	void Reset() const
	{
		seed_ = UInt32(initialSeed_);
	}

	Int32 GetInitialSeed() const
	{
		return initialSeed_;
	}

	/**
		* Generates a new random seed.
		*/
	void GenerateNewSeed()
	{
		Initialize(Rand());
	}

	/**
		* Returns a random float number in the range [0, 1).
		*
		* @return Random number.
		*/
	float GetFraction() const
	{
		MutateSeed();

		float Result;

		*(UInt32*)&Result = 0x3F800000U | (seed_ >> 9);

		return Result - 1.0f;
	}

	/**
		* Returns a random number between 0 and MAXUINT.
		*
		* @return Random number.
		*/
	UInt32 GetUnsignedInt() const
	{
		MutateSeed();

		return seed_;
	}

	/**
		* Returns a random vector of unit size.
		*
		* @return Random unit vector.
		*/
	Vector3 GetUnitVector() const
	{
		Vector3 Result;
		Real L;

		do
		{
			// Check random vectors in the unit sphere so result is statistically uniform.
			Result.x_ = GetFraction() * 2.f - 1.f;
			Result.y_ = GetFraction() * 2.f - 1.f;
			Result.z_ = GetFraction() * 2.f - 1.f;
			L = Result.LengthSquared();
		} while (L > 1.f || L < KINDA_SMALL_NUMBER);

		return Result.Normalized();
	}

	/**
		* Gets the current seed.
		*
		* @return Current seed.
		*/
	Int32 GetCurrentSeed() const
	{
		return Int32(seed_);
	}

	/**
		* Mirrors the random number API in FMath
		*
		* @return Random number.
		*/
	FORCEINLINE float FRand() const
	{
		return GetFraction();
	}

	/**
		* Helper function for rand implementations.
		*
		* @return A random number in [0..A)
		*/
	FORCEINLINE Int32 RandHelper(Int32 A) const
	{
		// GetFraction guarantees a result in the [0,1) range.
		return ((A > 0) ? FloorToInt(GetFraction() * float(A)) : 0);
	}

	/**
		* Helper function for rand implementations.
		*
		* @return A random number >= Min and <= Max
		*/
	FORCEINLINE Int32 RandRange(Int32 Min, Int32 Max) const
	{
		const Int32 Range = (Max - Min) + 1;

		return Min + RandHelper(Range);
	}

	/**
		* Helper function for rand implementations.
		*
		* @return A random number >= Min and <= Max
		*/
	FORCEINLINE Real FRandRange(Real InMin, Real InMax) const
	{
		return InMin + (InMax - InMin) * FRand();
	}

	/**
		* Returns a random vector of unit size.
		*
		* @return Random unit vector.
		*/
	FORCEINLINE Vector3 VRand() const
	{
		return GetUnitVector();
	}

	FORCEINLINE Vector3 RandPointInBox(const BoundingBox& Box) const
	{
		return Vector3(FRandRange(Box.min_.x_, Box.max_.x_),
			FRandRange(Box.min_.y_, Box.max_.y_),
			FRandRange(Box.min_.z_, Box.max_.z_));
	}

	///**
	//	* Returns a random unit vector, uniformly distributed, within the specified cone.
	//	*
	//	* @param Dir The center direction of the cone
	//	* @param ConeHalfAngleRad Half-angle of cone, in radians.
	//	* @return Normalized vector within the specified cone.
	//	*/
	//FORCEINLINE Vector3 VRandCone(Vector3 const& Dir, float ConeHalfAngleRad) const
	//{
	//	if (ConeHalfAngleRad > 0.f)
	//	{
	//		float const RandU = FRand();
	//		float const RandV = FRand();

	//		// Get spherical coords that have an even distribution over the unit sphere
	//		// Method described at http://mathworld.wolfram.com/SpherePointPicking.html	
	//		float Theta = 2.f * PI * RandU;
	//		float Phi = Acos((2.f * RandV) - 1.f);

	//		// restrict phi to [0, ConeHalfAngleRad]
	//		// this gives an even distribution of points on the surface of the cone
	//		// centered at the origin, pointing upward (z), with the desired angle
	//		Phi = Mod(Phi, ConeHalfAngleRad);

	//		// get axes we need to rotate around
	//		FMatrix const DirMat = FRotationMatrix(Dir.Rotation());
	//		// note the axis translation, since we want the variation to be around X
	//		Vector3 const DirZ = DirMat.GetUnitAxis(EAxis::X);
	//		Vector3 const DirY = DirMat.GetUnitAxis(EAxis::Y);

	//		Vector3 Result = Dir.RotateAngleAxis(Phi * 180.f / PI, DirY);
	//		Result = Result.RotateAngleAxis(Theta * 180.f / PI, DirZ);

	//		// ensure it's a unit vector (might not have been passed in that way)
	//		Result = Result.GetSafeNormal();

	//		return Result;
	//	}
	//	else
	//	{
	//		return Dir.GetSafeNormal();
	//	}
	//}

	///**
	//	* Returns a random unit vector, uniformly distributed, within the specified cone.
	//	*
	//	* @param Dir The center direction of the cone
	//	* @param HorizontalConeHalfAngleRad Horizontal half-angle of cone, in radians.
	//	* @param VerticalConeHalfAngleRad Vertical half-angle of cone, in radians.
	//	* @return Normalized vector within the specified cone.
	//	*/
	//FORCEINLINE Vector3 VRandCone(Vector3 const& Dir, float HorizontalConeHalfAngleRad, float VerticalConeHalfAngleRad) const
	//{
	//	if ((VerticalConeHalfAngleRad > 0.f) && (HorizontalConeHalfAngleRad > 0.f))
	//	{
	//		float const RandU = FRand();
	//		float const RandV = FRand();

	//		// Get spherical coords that have an even distribution over the unit sphere
	//		// Method described at http://mathworld.wolfram.com/SpherePointPicking.html	
	//		float Theta = 2.f * PI * RandU;
	//		float Phi = Acos((2.f * RandV) - 1.f);

	//		// restrict phi to [0, ConeHalfAngleRad]
	//		// where ConeHalfAngleRad is now a function of Theta
	//		// (specifically, radius of an ellipse as a function of angle)
	//		// function is ellipse function (x/a)^2 + (y/b)^2 = 1, converted to polar coords
	//		float ConeHalfAngleRad = Square(Cos(Theta) / VerticalConeHalfAngleRad) + Square(Sin(Theta) / HorizontalConeHalfAngleRad);
	//		ConeHalfAngleRad = Sqrt(1.f / ConeHalfAngleRad);

	//		// clamp to make a cone instead of a sphere
	//		Phi = Mod(Phi, ConeHalfAngleRad);

	//		// get axes we need to rotate around
	//		FMatrix const DirMat = FRotationMatrix(Dir.Rotation());
	//		// note the axis translation, since we want the variation to be around X
	//		Vector3 const DirZ = DirMat.GetUnitAxis(EAxis::X);
	//		Vector3 const DirY = DirMat.GetUnitAxis(EAxis::Y);

	//		Vector3 Result = Dir.RotateAngleAxis(Phi * 180.f / PI, DirY);
	//		Result = Result.RotateAngleAxis(Theta * 180.f / PI, DirZ);

	//		// ensure it's a unit vector (might not have been passed in that way)
	//		Result = Result.GetSafeNormal();

	//		return Result;
	//	}
	//	else
	//	{
	//		return Dir.GetSafeNormal();
	//	}
	//}

	/**
		* Exports the RandomStreams value to a string.
		*
		* @param ValueStr Will hold the string value.
		* @param DefaultValue The default value.
		* @param Parent Not used.
		* @param PortFlags Not used.
		* @param ExportRootScope Not used.
		* @return true on success, false otherwise.
		* @see ImportTextItem
		*/
	FlagGG_API bool ExportTextItem(String& ValueStr, RandomStream const& DefaultValue, class Object* Parent, Int32 PortFlags, class Object* ExportRootScope) const;

	/**
		* Get a textual representation of the RandomStream.
		*
		* @return Text describing the RandomStream.
		*/
	String ToString() const
	{
		char tempBuffer[CONVERSION_BUFFER_LENGTH];
		sprintf(tempBuffer, "RandomStream(initialSeed_=%i, seed_=%u)", initialSeed_, seed_);
		return tempBuffer;
	}

protected:

	/**
		* Mutates the current seed into the next seed.
		*/
	void MutateSeed() const
	{
		seed_ = (seed_ * 196314165U) + 907633515U;
	}

private:

	// Holds the initial seed.
	Int32 initialSeed_;

	// Holds the current seed. This should be an UInt32 so that any shift to obtain top bits
	// is a logical shift, rather than an arithmetic shift (which smears down the negative bit).
	mutable UInt32 seed_;
};

}
