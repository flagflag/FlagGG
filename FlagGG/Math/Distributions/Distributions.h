#pragma once

#include "Core/BaseMacro.h"
#include "Core/BaseTypes.h"
#include "Core/CryAssert.h"
#include "Math/Vector4.h"
#include "Math/Color.h"
#include "Math/RandomStream.h"
#include "Container/Vector.h"
#include "IOFrame/Buffer/IOBuffer.h"

/** Usually checkSlow for performance. */
#define checkDistribution ASSERT

namespace FlagGG
{

class DistributionFloat;

/**
 * Operation to perform when looking up a value.
 */
enum RawDistributionOperation
{
	RDO_Uninitialized,
	RDO_None,
	RDO_Random,
	RDO_Extreme,
};

/**
 * Lookup table used to sample distributions at runtime.
 */
struct DistributionLookupTable
{	
	/** Default constructor. */
	DistributionLookupTable()
		: timeScale_(0.0f)
		, timeBias_(0.0f)
		, op_(RDO_Uninitialized)
		, entryCount_(0)
		, entryStride_(0)
		, subEntryStride_(0)
		, lockFlag_(0)
	{}

	/**
	 * Empties the table of all values.
	 */
	void Empty()
	{
		timeScale_ = 0.0f;
		timeBias_ = 0.0f;
		op_ = RDO_Uninitialized;
		entryCount_ = 0;
		entryStride_ = 0;
		subEntryStride_ = 0;
		lockFlag_ = 0;
	}

	/**
	 * Returns true if the lookup table contains no values.
	 */
	FORCEINLINE bool IsEmpty() const
	{
		return values_.Size() == 0 || entryCount_ == 0;
	}

	/**
	 * Computes the number of Values per entry in the table.
	 */
	FORCEINLINE float GetValuesPerEntry() const
	{
		return (float)(entryStride_ - subEntryStride_);
	}

	/**
	 * Compute the number of values contained in the table.
	 */
	FORCEINLINE float GetValueCount() const
	{
		return (float)(values_.Size());
	}

	/**
	 * Get the entry for time and the one after it for interpolating (along with 
	 * an alpha for interpolation)
	 * 
	 * @param time The time we are looking to retrieve
	 * @param entry1 Out variable that is the first (or only) entry
	 * @param entry2 Out variable that is the second entry (for interpolating)
	 * @param lerpAlpha Out variable that is the alpha for interpolating between entry1 and entry2
	 */
	FORCEINLINE void GetEntry(float time, const float*& entry1, const float*& entry2, float& lerpAlpha) const
	{
		// make time relative to start time
		time -= timeBias_;
		time *= timeScale_;
		time = time >= 0.f ? time : 0.0f;

		// calculate the alpha to lerp between entry1 and entry2
		lerpAlpha = Fract(time);

		// get the entries to lerp between
		const UInt32 Index = FloorToInt(time); // TruncToInt
		const UInt32 index1 = Min<UInt32>(Index + 0, entryCount_ - 1) * entryStride_;
		const UInt32 index2 = Min<UInt32>(Index + 1, entryCount_ - 1) * entryStride_;
		entry1 = &values_[index1];
		entry2 = &values_[index2];
	}

	/**
	 * Get the range of values produced by the table.
	 * @note: in the case of a constant curve, this will not be exact!
	 * @param OutMinValues - The smallest values produced by this table.
	 * @param OutMaxValues - The largest values produced by this table.
	 */
	void GetRange(float* outMinValues, float* outMaxValues)
	{
		if (entryCount_ > 0)
		{
			const Int32 valuesPerEntry = (Int32)GetValuesPerEntry();
			const float* entry = values_.Buffer();

			// Initialize to the first entry in the table.
			for (Int32 valueIndex = 0; valueIndex < valuesPerEntry; ++valueIndex)
			{
				outMinValues[valueIndex] = entry[valueIndex];
				outMaxValues[valueIndex] = entry[valueIndex + subEntryStride_];
			}

			// Iterate over each entry updating the minimum and maximum values.
			for (Int32 entryIndex = 1; entryIndex < entryCount_; ++entryIndex)
			{
				entry += entryStride_;
				for (Int32 valueIndex = 0; valueIndex < valuesPerEntry; ++valueIndex)
				{
					outMinValues[valueIndex] = Min(outMinValues[valueIndex], entry[valueIndex]);
					outMaxValues[valueIndex] = Max(outMaxValues[valueIndex], entry[valueIndex + subEntryStride_]);
				}
			}
		}
	}

	/** time between values in the lookup table */
	float timeScale_;
	/** Absolute time of the first value */
	float timeBias_;
	/** Values in the table. */
	PODVector<float> values_;
	/** Operation for which the table was built. */
	UInt8 op_;
	/** Number of entries in the table. */
	UInt8 entryCount_;
	/** Number of values between entries [1,8]. */
	UInt8 entryStride_;
	/** Number of values between sub-entries [0,4]. */
	UInt8 subEntryStride_;
	/** Lock axes flag for vector distributions. */
	UInt8 lockFlag_;
};

// Helper macro for retrieving random value
#define DIST_GET_RANDOM_VALUE(RandStream)		((RandStream == NULL) ? SRand() : RandStream->GetFraction())

/**
 * Raw distribution used to quickly sample distributions at runtime.
 */
struct FlagGG_API RawDistribution
{
	/** Default constructor. */
	RawDistribution()
	{
	}

	/**
	 * Serialization.
	 * @param Ar - The archive with which to serialize.
	 * @returns true if serialization was successful.
	 */
	bool Serialize(IOFrame::Buffer::IOBuffer* buffer);

	/**
	 * Calcuate the float or vector value at the given time 
	 * @param time The time to evaluate
	 * @param value An array of (1 or 3) FLOATs to receive the values
	 * @param numCoords The number of floats in the value array
	 * @param extreme For distributions that use one of the extremes, this is which extreme to use
	 */
	void GetValue(float time, float* value, Int32 numCoords, Int32 extreme, struct RandomStream* randomStream) const;

	// prebaked versions of these
	void GetValue1(float time, float* value, Int32 extreme, struct RandomStream* randomStream) const;
	void GetValue3(float time, float* value, Int32 extreme, struct RandomStream* randomStream) const;
	inline void GetValue1None(float time, float* inValue) const
	{
		float* value = inValue;
		const float* entry1;
		const float* entry2;
		float lerpAlpha = 0.0f;
		lookupTable_.GetEntry(time, entry1, entry2, lerpAlpha);
		const float* newEntry1 = entry1;
		const float* newEntry2 = entry2;
		value[0] = Lerp(newEntry1[0], newEntry2[0], lerpAlpha);
	}
	inline void GetValue3None(float time, float* inValue) const
	{
		float* value = inValue;
		const float* entry1;
		const float* entry2;
		float lerpAlpha = 0.0f;
		lookupTable_.GetEntry(time, entry1, entry2, lerpAlpha);
		const float* newEntry1 = entry1;
		const float* newEntry2 = entry2;
		float T0 = Lerp(newEntry1[0], newEntry2[0], lerpAlpha);
		float T1 = Lerp(newEntry1[1], newEntry2[1], lerpAlpha);
		float T2 = Lerp(newEntry1[2], newEntry2[2], lerpAlpha);
		value[0] = T0;
		value[1] = T1;
		value[2] = T2;
	}
	void GetValue1Extreme(float time, float* value, Int32 extreme, struct RandomStream* randomStream) const;
	void GetValue3Extreme(float time, float* value, Int32 extreme, struct RandomStream* randomStream) const;
	void GetValue1Random(float time, float* value, struct RandomStream* randomStream) const;
	void GetValue3Random(float time, float* value, struct RandomStream* randomStream) const;

	FORCEINLINE bool IsSimple() 
	{
		return lookupTable_.op_ == RDO_None;
	}

	/**
	 * Return the UDistribution* variable if the given StructProperty
	 * points to a FRawDistribution* struct
	 * @param Property Some UStructProperyy
	 * @param Data Memory that owns the property
	 * @return The UDisitribution* object if this is a FRawDistribution* struct, 
	 *         or NULL otherwise
	 */
	// static UObject* TryGetDistributionObjectFromRawDistributionProperty(FStructProperty* Property, uint8* Data);

protected:

	/** Lookup table of values */
	DistributionLookupTable lookupTable_;
};

/*-----------------------------------------------------------------------------
	Type safe distributions.

	Like FRawDistribution but typesafe and not tied directly to UObjects.
-----------------------------------------------------------------------------*/

/**
 * Raw distribution from which one float can be looked up per entry.
 */
class FloatDistribution
{
public:
	/** Default constructor. */
	FloatDistribution();

	/**
	 * Samples a value from the distribution.
	 * @param time - time at which to sample the distribution.
	 * @param outValue - Upon return contains the sampled value.
	 */
	FORCEINLINE void GetValue(float time, float* RESTRICT outValue) const
	{
		checkDistribution(lookupTable_.GetValuesPerEntry() == 1);

		const float* entry1;
		const float* entry2;
		float alpha;

		lookupTable_.GetEntry(time, entry1, entry2, alpha);
		outValue[0] = Lerp(entry1[0], entry2[0], alpha);
	}

	/**
	 * Samples a value randomly distributed between two values.
	 * @param time - time at which to sample the distribution.
	 * @param outValue - Upon return contains the sampled value.
	 * @param RandomStream - Random stream from which to retrieve random fractions.
	 */
	FORCEINLINE void GetRandomValue(float time, float* RESTRICT outValue, RandomStream& randomStream) const
	{
		checkDistribution(lookupTable_.GetValuesPerEntry() == 1);

		const float* entry1;
		const float* entry2;
		float alpha;
		float randomAlpha;
		float minValue, maxValue;
		const Int32 subEntryStride = lookupTable_.subEntryStride_;

		lookupTable_.GetEntry(time, entry1, entry2, alpha);
		minValue = Lerp(entry1[0], entry2[0], alpha);
		maxValue = Lerp(entry1[subEntryStride], entry2[subEntryStride], alpha);
		randomAlpha = randomStream.GetFraction();
		outValue[0] = Lerp(minValue, maxValue, randomAlpha);
	}

	/**
	 * Computes the range of the distribution.
	 * @param outMin - The minimum value in the distribution.
	 * @param outMax - The maximum value in the distribution.
	 */
	void GetRange(float* outMin, float* outMax)
	{
		lookupTable_.GetRange(outMin, outMax);
	}

private:
	/** The lookup table. */
	DistributionLookupTable lookupTable_;

	//@todo.CONSOLE: These are needed until we have a cooker/package in place!
	/** This distribution is built from composable distributions. */
	friend class ComposableDistribution;
};

/**
 * Raw distribution from which three floats can be looked up per entry.
 */
class VectorDistribution
{
public:
	/** Default constructor. */
	VectorDistribution();

	/**
	 * Samples a value from the distribution.
	 * @param time - time at which to sample the distribution.
	 * @param outValue - Upon return contains the sampled value.
	 */
	FORCEINLINE void GetValue(float time, float* RESTRICT outValue) const
	{
		checkDistribution(lookupTable_.GetValuesPerEntry() == 3);

		const float* entry1;
		const float* entry2;
		float alpha;

		lookupTable_.GetEntry(time, entry1, entry2, alpha);
		outValue[0] = Lerp(entry1[0], entry2[0], alpha);
		outValue[1] = Lerp(entry1[1], entry2[1], alpha);
		outValue[2] = Lerp(entry1[2], entry2[2], alpha);
	}

	/**
	 * Samples a value randomly distributed between two values.
	 * @param time - time at which to sample the distribution.
	 * @param outValue - Upon return contains the sampled value.
	 * @param RandomStream - Random stream from which to retrieve random fractions.
	 */
	FORCEINLINE void GetRandomValue(float time, float* RESTRICT outValue, RandomStream& randomStream) const
	{
		checkDistribution(lookupTable_.GetValuesPerEntry() == 3);

		const float* entry1;
		const float* entry2;
		float alpha;
		float randomAlpha[3];
		float minValue[3], maxValue[3];
		const Int32 subEntryStride = lookupTable_.subEntryStride_;

		lookupTable_.GetEntry(time, entry1, entry2, alpha);

		minValue[0] = Lerp(entry1[0], entry2[0], alpha);
		minValue[1] = Lerp(entry1[1], entry2[1], alpha);
		minValue[2] = Lerp(entry1[2], entry2[2], alpha);

		maxValue[0] = Lerp(entry1[subEntryStride + 0], entry2[subEntryStride + 0], alpha);
		maxValue[1] = Lerp(entry1[subEntryStride + 1], entry2[subEntryStride + 1], alpha);
		maxValue[2] = Lerp(entry1[subEntryStride + 2], entry2[subEntryStride + 2], alpha);

		randomAlpha[0] = randomStream.GetFraction();
		randomAlpha[1] = randomStream.GetFraction();
		randomAlpha[2] = randomStream.GetFraction();

		outValue[0] = Lerp(minValue[0], maxValue[0], randomAlpha[0]);
		outValue[1] = Lerp(minValue[1], maxValue[1], randomAlpha[1]);
		outValue[2] = Lerp(minValue[2], maxValue[2], randomAlpha[2]);
	}

	/**
	 * Computes the range of the distribution.
	 * @param outMin - The minimum value in the distribution.
	 * @param outMax - The maximum value in the distribution.
	 */
	void GetRange(Vector3* outMin, Vector3* outMax)
	{
		lookupTable_.GetRange((float*)outMin, (float*)outMax);
	}

private:
	/** The lookup table. */
	DistributionLookupTable lookupTable_;

	/** This distribution is built from composable distributions. */
	friend class ComposableDistribution;
};

/**
 * Raw distribution from which four floats can be looked up per entry.
 */
class Vector4Distribution
{
public:
	/** Default constructor. */
	Vector4Distribution();

	/**
	 * Samples a value from the distribution.
	 * @param time - time at which to sample the distribution.
	 * @param outValue - Upon return contains the sampled value.
	 */
	FORCEINLINE void GetValue(float time, float* RESTRICT outValue) const
	{
		checkDistribution(lookupTable_.GetValuesPerEntry() == 4);

		const float* entry1;
		const float* entry2;
		float alpha;

		lookupTable_.GetEntry(time, entry1, entry2, alpha);
		outValue[0] = Lerp(entry1[0], entry2[0], alpha);
		outValue[1] = Lerp(entry1[1], entry2[1], alpha);
		outValue[2] = Lerp(entry1[2], entry2[2], alpha);
		outValue[3] = Lerp(entry1[3], entry2[3], alpha);
	}

	/**
	 * Samples a value randomly distributed between two values.
	 * @param time - time at which to sample the distribution.
	 * @param outValue - Upon return contains the sampled value.
	 * @param RandomStream - Random stream from which to retrieve random fractions.
	 */
	FORCEINLINE void GetRandomValue(float time, float* RESTRICT outValue, RandomStream& randomStream) const
	{
		checkDistribution(lookupTable_.GetValuesPerEntry() == 4);

		const float* entry1;
		const float* entry2;
		float alpha;
		float randomAlpha[4];
		float minValue[4], maxValue[4];
		const Int32 subEntryStride = lookupTable_.subEntryStride_;

		lookupTable_.GetEntry(time, entry1, entry2, alpha);

		minValue[0] = Lerp(entry1[0], entry2[0], alpha);
		minValue[1] = Lerp(entry1[1], entry2[1], alpha);
		minValue[2] = Lerp(entry1[2], entry2[2], alpha);
		minValue[3] = Lerp(entry1[3], entry2[3], alpha);

		maxValue[0] = Lerp(entry1[subEntryStride + 0], entry2[subEntryStride + 0], alpha);
		maxValue[1] = Lerp(entry1[subEntryStride + 1], entry2[subEntryStride + 1], alpha);
		maxValue[2] = Lerp(entry1[subEntryStride + 2], entry2[subEntryStride + 2], alpha);
		maxValue[3] = Lerp(entry1[subEntryStride + 3], entry2[subEntryStride + 3], alpha);

		randomAlpha[0] = randomStream.GetFraction();
		randomAlpha[1] = randomStream.GetFraction();
		randomAlpha[2] = randomStream.GetFraction();
		randomAlpha[3] = randomStream.GetFraction();

		outValue[0] = Lerp(minValue[0], maxValue[0], randomAlpha[0]);
		outValue[1] = Lerp(minValue[1], maxValue[1], randomAlpha[1]);
		outValue[2] = Lerp(minValue[2], maxValue[2], randomAlpha[2]);
		outValue[3] = Lerp(minValue[3], maxValue[3], randomAlpha[3]);
	}

	/**
	 * Computes the range of the distribution.
	 * @param outMin - The minimum value in the distribution.
	 * @param outMax - The maximum value in the distribution.
	 */
	void GetRange(Vector4* outMin, Vector4* outMax)
	{
		lookupTable_.GetRange((float*)outMin, (float*)outMax);
	}

private:
	/** The lookup table. */
	DistributionLookupTable lookupTable_;

	/** This distribution is built from composable distributions. */
	friend class ComposableDistribution;
};

/*-----------------------------------------------------------------------------
	Composable distributions.

	These classes allow code to compose multiple distributions and produce a
	final, optimized raw distribution.
-----------------------------------------------------------------------------*/

//#if WITH_EDITORONLY_DATA

/**
 * Base class for composable distributions. Responsible for building the final
 * distributions that can be sampled.
 */
class ComposableDistribution
{
public:
	/**
	 * Constructs a type-safe floating point distribution.
	 * @param outDistribution - The distribution to build.
	 * @param X - The source distribution.
	 */
	static void BuildFloat(class FloatDistribution& outDistribution, const class ComposableFloatDistribution& X);

	/**
	 * Constructs a type-safe vector distribution.
	 * @param outDistribution - The distribution to build.
	 * @param XYZ - The source distribution.
	 */
	static void BuildVector(class VectorDistribution& outDistribution, const class ComposableVectorDistribution& XYZ);

	/**
	 * Constructs a type-safe 4-vector distribution.
	 * @param outDistribution - The distribution to build.
	 * @param XYZ - The source distribution for the X, Y, and Z components.
	 * @param W - The source distribution for the W component.
	 */
	static void BuildVector4(class Vector4Distribution& outDistribution, const class ComposableVectorDistribution& XYZ, const class ComposableFloatDistribution& W);

	/**
	 * Constructs a type-safe 4-vector distribution.
	 * @param outDistribution - The distribution to build.
	 * @param XY - The source distribution for the X and Y components.
	 * @param Z - The source distribution for the Z component.
	 * @param W - The source distribution for the W component.
	 */
	static void BuildVector4(
		class Vector4Distribution& outDistribution,
		const class ComposableVectorDistribution& XY,
		const class ComposableFloatDistribution& Z,
		const class ComposableFloatDistribution& W);

	/**
	 * Constructs a type-safe 4-vector distribution.
	 * @param outDistribution - The distribution to build.
	 * @param X - The source distribution for the X component.
	 * @param Y - The source distribution for the Y component.
	 * @param Z - The source distribution for the Z component.
	 * @param W - The source distribution for the W component.
	 */
	static void BuildVector4(
		class Vector4Distribution& outDistribution,
		const class ComposableFloatDistribution& X,
		const class ComposableFloatDistribution& Y,
		const class ComposableFloatDistribution& Z,
		const class ComposableFloatDistribution& W);

	/**
	 * Quantizes a 4-component vector distribution.
	 * @param outQuantizedSamples - The quantized samples in the distribution.
	 * @param OutScale - Scale to apply to quantized samples.
	 * @param OutBias - Bias to apply to quantized samples.
	 */
	static void QuantizeVector4(
		PODVector<Color>& outQuantizedSamples,
		Vector4& OutScale,
		Vector4& OutBias,
		const class Vector4Distribution& Distribution);

protected:
	/** The lookup table. */
	DistributionLookupTable lookupTable_;

	/** Constructor. Protected to prevent instantiating the base class. */
	ComposableDistribution() {}

	/** Destructor. */
	~ComposableDistribution() {}

	/** Copy constructor. Protected to only allow type-safe copying. */
	ComposableDistribution(const ComposableDistribution& Other)
		: lookupTable_(Other.lookupTable_)
	{
	}

	/** Assignment operator. Protected to only allow type-safe copying. */
	const ComposableDistribution& operator=(const ComposableDistribution& Other)
	{
		lookupTable_ = Other.lookupTable_;
		return *this;
	}
};

/**
 * A composable floating point distribution.
 */
class ComposableFloatDistribution : public ComposableDistribution
{
public:
	/** Default constructor. */
	ComposableFloatDistribution();

	/** Copy constructor. */
	ComposableFloatDistribution(const ComposableFloatDistribution& other)
		: ComposableDistribution(other)
	{
	}

	/** Assignment operator. */
	const ComposableFloatDistribution& operator=(const ComposableFloatDistribution& other)
	{
		ComposableDistribution::operator=(other);
		return *this;
	}

	/**
	 * Initializes this distribution from that specified.
	 */
	void Initialize(const DistributionFloat* floatDistribution);

	/**
	 * Initializes this distribution with a constant value.
	 */
	void InitializeWithConstant(float value);

	/**
	 * Scales each sample in the distribution by a constant.
	 */
	void ScaleByConstant(float scale);

	/**
	 * Scale each sample in the distribution by the corresponding sample in another distribution.
	 */
	void ScaleByDistribution(const DistributionFloat* floatDistribution);

	/**
	 * Adds each sample in the distribution to the corresponding sample in another distribution.
	 */
	void AddDistribution(const DistributionFloat* floatDistribution);

	/**
	 * Normalizes each value to [0,1] and return a scale and bias to reconstruct the original values.
	 */
	void Normalize(float* outScale, float* outBias);

	/**
	 * Resamples the distribution to include only information from [minIn,maxIn].
	 */
	void Resample(float minIn, float maxIn);
};

class ComposableVectorDistribution : public ComposableDistribution
{
public:
	/** Default constructor. */
	ComposableVectorDistribution();

	/** Copy constructor. */
	ComposableVectorDistribution(const ComposableVectorDistribution& other)
		: ComposableDistribution(other)
	{
	}

	/** Assignment operator. */
	const ComposableVectorDistribution& operator=(const ComposableVectorDistribution& other)
	{
		ComposableDistribution::operator=(other);
		return *this;
	}

	/**
	 * Initializes this distribution from that specified.
	 */
	void Initialize(const class DistributionVector* vectorDistribution);

	/**
	 * Initializes this distribution with a constant value.
	 */
	void InitializeWithConstant(const Vector3& value);

	/**
	 * Scales each sample in the distribution by a constant.
	 */
	void ScaleByConstant(float scale);

	/**
	 * Scales each sample in the distribution by a constant vector.
	 */
	void ScaleByConstantVector(const Vector3& scale);

	/**
	 * Adds a constant vector to each sample in the distribution.
	 */
	void AddConstantVector(const Vector3& value);

	/**
	 * Scale each sample in the distribution by the corresponding sample in another distribution.
	 */
	void ScaleByDistribution(const class DistributionFloat* floatDistribution);

	/**
	 * Scale each sample in the distribution by the corresponding sample in another distribution.
	 */
	void ScaleByVectorDistribution(const class DistributionVector* vectorDistribution);

	/**
	 * Adds each sample in the distribution to the corresponding sample in another distribution.
	 */
	void AddDistribution(const class DistributionVector* vectorDistribution);

	/**
	 * Splats the specified channel to all channels in the distribution.
	 */
	void Splat(Int32 channelIndex);

	/**
	 * Resamples the distribution to include only information from [minIn,maxIn].
	 */
	void Resample(float minIn, float maxIn);
};
//#endif // #if WITH_EDITORONLY_DATA

}
