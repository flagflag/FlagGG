// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Math/Vector3.h"

namespace FlagGG
{

/**
 * A pair of 3D vectors.
 */
struct TwoVectors
{
public:

	/** Holds the first vector. */
	Vector3	v1_;

	/** Holds the second vector. */
	Vector3	v2_;

public:

	/** Default constructor. */
	FORCEINLINE	TwoVectors();

	/**
	 * Creates and initializes a new instance with the specified vectors.
	 *
	 * @param In1 The first Vector.
	 * @param In2 The second Vector.
	 */
	FORCEINLINE	TwoVectors( Vector3 In1, Vector3 In2 );

	/**
	 * Constructor.
	 *
	 * @param EForceInit Force Init Enum
	 */
	explicit FORCEINLINE TwoVectors( EForceInit );

public:

	/**
	 * Gets result of addition of two pairs of vectors.
	 *
	 * @param V The pair to add.
	 * @return Result of addition.
	 */
	FORCEINLINE TwoVectors operator+( const TwoVectors& V ) const;

	/**
	 * Gets result of subtraction of two pairs of vectors.
	 *
	 * @param V The pair to subtract.
	 * @return Result of subtraction.
	 */
	FORCEINLINE TwoVectors operator-( const TwoVectors& V ) const;

	/**
	 * Gets result of scaling pair of vectors.
	 *
	 * @param Scale The scaling factor.
	 * @return Result of Scaling.
	 */
	FORCEINLINE TwoVectors operator*( float Scale ) const;

	/**
	 * Gets result of dividing pair of vectors.
	 *
	 * @param Scale What to divide by.
	 * @return Result of division.
	 */
	TwoVectors operator/( float Scale ) const;

	/**
	 * Gets result of multiplying two pairs of vectors.
	 *
	 * @param V The pair to multiply with.
	 * @return Result of multiplication.
	 */
	FORCEINLINE TwoVectors operator*( const TwoVectors& V ) const;

	/**
	 * Gets result of division of two pairs of vectors.
	 *
	 * @param V The pair to divide by.
	 * @return Result of division.
	 */
	FORCEINLINE TwoVectors operator/( const TwoVectors& V ) const;

	// Binary comparison operators.

	/**
	 * Checks two pairs for equality.
	 *
	 * @param V The other pair.
	 * @return true if the two pairs are equal, false otherwise..
	 */
	bool operator==( const TwoVectors& V ) const;

	/**
	 * Checks two pairs for inequality.
	 *
	 * @param V The other pair.
	 * @return true if the two pairs are different, false otherwise..
	 */
	bool operator!=( const TwoVectors& V ) const;

	/**
	 * Error-tolerant comparison.
	 *
	 * @param V The other pair.
	 * @param Tolerance Error Tolerance.
	 * @return true if two pairs are equal within specified tolerance, false otherwise..
	 */
	bool Equals( const TwoVectors& V, float Tolerance = KINDA_SMALL_NUMBER ) const;

	// Unary operators.

	/**
	 * Get a negated copy of the pair.
	 *
	 * @return A negated copy of the pair.
	 */
	FORCEINLINE TwoVectors operator-() const;

	// Assignment operators.

	/**
	 * Add a pair to this.
	 *
	 * @param The pair to add.
	 * @return Copy of the pair after addition.
	 */
	FORCEINLINE TwoVectors operator+=( const TwoVectors& V );

	/**
	 * Subtract a pair from this.
	 *
	 * @param The pair to subtract.
	 * @return Copy of the pair after subtraction.
	 */
	FORCEINLINE TwoVectors operator-=( const TwoVectors& V );

	/**
	 * Scale the pair.
	 *
	 * @param Scale What to scale by.
	 * @return Copy of the pair after scaling.
	 */
	FORCEINLINE TwoVectors operator*=( float Scale );

	/**
	 * Divide the pair.
	 *
	 * @param What to divide by.
	 * @return Copy of the pair after division.
	 */
	TwoVectors operator/=( float V );

	/**
	 * Multiply the pair by another.
	 *
	 * @param The other pair.
	 * @return Copy of the pair after multiplication.
	 */
	TwoVectors operator*=( const TwoVectors& V );

	/**
	 * Divide the pair by another.
	 *
	 * @param The other pair.
	 * @return Copy of the pair after multiplication.
	 */
	TwoVectors operator/=( const TwoVectors& V );

	/**
	 * Get a specific component from the pair.
	 *
	 * @param i The index of the component, even indices are for the first vector,
	 *			odd ones are for the second. Returns index 5 if out of range.
	 * @return Reference to the specified component.
	 */
    Real& operator[]( Int32 i );

public:

	/**
	 * Get the maximum value of all the vector coordinates.
	 *
	 * @return The maximum value of all the vector coordinates.
	 */
	Real GetMax() const;

	/**
	 * Get the minimum value of all the vector coordinates.
	 *
	 * @return The minimum value of all the vector coordinates.
	 */
	Real GetMin() const;

	/**
	 * Get a textual representation of this two-vector.
	 *
	 * @return A string describing the two-vector.
	 */
	String ToString() const;

public:

	/**
	 * Serializes the two-vector.
	 *
	 * @param Ar The archive to serialize into.
	 * @param TwoVectors The two-vector to serialize.
	 * @return Reference to the Archive after serialization.
	 */
	//friend FArchive& operator<<( FArchive& Ar, TwoVectors& TwoVectors )
	//{
	//	return Ar << TwoVectors.v1_ << TwoVectors.v2_;
	//}

	//bool Serialize( FArchive& Ar )
	//{
	//	Ar << *this;
	//	return true;
	//}
};


/* TwoVectors inline functions
 *****************************************************************************/

FORCEINLINE TwoVectors operator*( float Scale, const TwoVectors& V )
{
	return V.operator*( Scale );
}


FORCEINLINE	TwoVectors::TwoVectors() :
	v1_(Vector3::ZERO),
	v2_(Vector3::ZERO)
{ }


FORCEINLINE TwoVectors::TwoVectors( Vector3 In1, Vector3 In2 )
	: v1_(In1)
	, v2_(In2)
{ }


FORCEINLINE TwoVectors::TwoVectors( EForceInit )
	: v1_(Vector3::ZERO)
	, v2_(Vector3::ZERO)
{ }


FORCEINLINE TwoVectors TwoVectors::operator+( const TwoVectors& V ) const
{
	return TwoVectors(
		Vector3(v1_ + V.v1_),
		Vector3(v2_ + V.v2_)
	);
}


FORCEINLINE TwoVectors TwoVectors::operator-( const TwoVectors& V ) const
{
	return TwoVectors( 
		Vector3(v1_ - V.v1_),
		Vector3(v2_ - V.v2_)
	);
}


FORCEINLINE TwoVectors TwoVectors::operator*( float Scale ) const
{
	return TwoVectors(
		Vector3(v1_ * Scale),
		Vector3(v2_ * Scale)
	);
}


FORCEINLINE TwoVectors TwoVectors::operator/( float Scale ) const
{
	const float RScale = 1.f / Scale;

	return TwoVectors(
		Vector3(v1_ * RScale), 
		Vector3(v2_ * RScale)
	);
}


FORCEINLINE TwoVectors TwoVectors::operator*( const TwoVectors& V ) const
{
	return TwoVectors(
		Vector3(v1_ * V.v1_),
		Vector3(v2_ * V.v2_)
		);
}


FORCEINLINE TwoVectors TwoVectors::operator/( const TwoVectors& V ) const
{
	return TwoVectors(
		Vector3(v1_ / V.v1_),
		Vector3(v2_ / V.v2_)
	);
}


FORCEINLINE bool TwoVectors::operator==( const TwoVectors& V ) const
{
	return ((v1_ == V.v1_) && (v2_ == V.v2_));
}


FORCEINLINE bool TwoVectors::operator!=( const TwoVectors& V ) const
{
	return ((v1_ != V.v1_) || (v2_ != V.v2_));
}


FORCEINLINE bool TwoVectors::Equals( const TwoVectors& V, float Tolerance ) const
{
	return v1_.Equals(V.v1_, Tolerance) && v2_.Equals(V.v2_, Tolerance);
}


FORCEINLINE TwoVectors TwoVectors::operator-() const
{
	return TwoVectors(
		Vector3(-v1_),
		Vector3(-v2_)
	);
}


FORCEINLINE TwoVectors TwoVectors::operator+=( const TwoVectors& V )
{
	v1_ += V.v1_;
	v2_ += V.v2_;

	return *this;
}


FORCEINLINE TwoVectors TwoVectors::operator-=( const TwoVectors& V )
{
	v1_ -= V.v1_;
	v2_ -= V.v2_;

	return *this;
}


FORCEINLINE TwoVectors TwoVectors::operator*=( float Scale )
{
	v1_ *= Scale;
	v2_ *= Scale;

	return *this;
}


FORCEINLINE TwoVectors TwoVectors::operator/=( float V )
{
	const float RV = 1.f/V;

	v1_ *= RV;
	v2_ *= RV;

	return *this;
}


FORCEINLINE TwoVectors TwoVectors::operator*=( const TwoVectors& V )
{
	v1_ *= V.v1_;
	v2_ *= V.v2_;
	return *this;
}


FORCEINLINE TwoVectors TwoVectors::operator/=( const TwoVectors& V )
{
	v1_ /= V.v1_;
	v2_ /= V.v2_;

	return *this;
}


FORCEINLINE Real TwoVectors::GetMax() const
{
	const Real MaxMax = Max(Max(v1_.x_, v1_.y_), v1_.z_);
	const Real MaxMin = Max(Max(v2_.x_, v2_.y_), v2_.z_);

	return Max(MaxMax, MaxMin);
}


FORCEINLINE Real TwoVectors::GetMin() const
{
	const Real MinMax = Min(Min(v1_.x_, v1_.y_), v1_.z_);
	const Real MinMin = Min(Min(v2_.x_, v2_.y_), v2_.z_);

	return Min(MinMax, MinMin);
}


FORCEINLINE Real& TwoVectors::operator[]( Int32 i )
{
	ASSERT(i > -1);
	ASSERT(i < 6);

	switch(i)
	{
	case 0:		return v1_.x_;
	case 1:		return v2_.x_;
	case 2:		return v1_.y_;
	case 3:		return v2_.y_;
	case 4:		return v1_.z_;
	default:	return v2_.z_;
	}
}


FORCEINLINE String TwoVectors::ToString() const
{
	char tempBuffer[CONVERSION_BUFFER_LENGTH];
	sprintf(tempBuffer, "V1=(%s) V2=(%s)", v1_.ToString().CString(), v2_.ToString().CString());
	return tempBuffer;
}

}
