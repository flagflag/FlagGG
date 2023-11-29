#pragma once

#include "Distribution.h"
#include "Distributions.h"

namespace FlagGG
{

class DistributionFloat;

struct FlagGG_API RawDistributionFloat : public RawDistribution
{
	// OBJECT_OVERRIDE(RawDistributionFloat, RawDistribution);
public:
	SharedPtr<DistributionFloat> distribution_;


	RawDistributionFloat()
		: minValue_(0)
		, maxValue_(0)
	{
	}

	/** Whether the distribution data has been cooked or the object itself is available */
	bool IsCreated();

#if WITH_EDITOR
	/**`
		* Initialize a raw distribution from the original Unreal distribution
		*/
	void Initialize();
#endif
			 
	/**
		* Gets a pointer to the raw distribution if you can just call FRawDistribution::GetValue1 on it, otherwise NULL 
		*/
	const RawDistribution* GetFastRawDistribution();

	/**
		* Get the value at the specified F
		*/
	float GetValue(float F = 0.0f, Object* Data = NULL, struct RandomStream* InRandomStream = NULL);

	/**
		* Get the min and max values
		*/
	void GetOutRange(float& MinOut, float& MaxOut);

	/**
		* Is this distribution a uniform type? (ie, does it have two values per entry?)
		*/
	inline bool IsUniform() { return lookupTable_.subEntryStride_ != 0; }

	void InitLookupTable();

	FORCEINLINE bool HasLookupTable(bool bInitializeIfNeeded = true)
	{
#if WITH_EDITOR
		if(bInitializeIfNeeded)
		{
			InitLookupTable();
		}
#endif
		return GDistributionType != 0 && !lookupTable_.IsEmpty();
	}

	FORCEINLINE bool OkForParallel()
	{
		HasLookupTable(); // initialize if required
		return true; // even if they stay distributions, this should probably be ok as long as nobody is changing them at runtime
		//return !Distribution || HasLookupTable();
	}

private:
	float minValue_;

	float maxValue_;
};

class FlagGG_API DistributionFloat : public Distribution
{
	OBJECT_OVERRIDE(DistributionFloat, Distribution);
public:

	/** Script-accessible way to query a float distribution */
	virtual float GetFloatValue(float F = 0);

	DistributionFloat()
		: canBeBaked_(true)
		, isDirty_(true) // make sure the FRawDistribution is initialized
	{
	}

	//@todo.CONSOLE: Currently, consoles need this? At least until we have some sort of cooking/packaging step!
	/**
	 * Return the operation used at runtime to calculate the final value
	 */
	virtual RawDistributionOperation GetOperation() const { return RDO_None; }

	/**
	 *  Returns the lock axes flag used at runtime to swizzle random stream values. Not used for distributions derived from DistributionFloat.
	 */
	virtual UInt8 GetLockFlag() const { return 0; }

	/**
	 * Fill out an array of floats and return the number of elements in the entry
	 *
	 * @param Time The time to evaluate the distribution
	 * @param Values An array of values to be filled out, guaranteed to be big enough for 4 values
	 * @return The number of elements (values) set in the array
	 */
	virtual UInt32 InitializeRawEntry(float time, float* values) const;

	/** @todo document */
	virtual float GetValue(float F = 0.f, Object* Data = NULL, struct RandomStream* InRandomStream = NULL) const;

	//~ Begin FCurveEdInterface Interface
	virtual void GetInRange(float& minIn, float& maxIn) const override;
	virtual void GetOutRange(float& minOut, float& maxOut) const override;
	//~ End FCurveEdInterface Interface

	/** @return true of this distribution can be baked into a FRawDistribution lookup table, otherwise false */
	virtual bool CanBeBaked() const
	{
		return canBeBaked_;
	}

	bool HasBakedSuccesfully() const
	{
		return bakedDataSuccesfully_;
	}

	/**
	 * Returns the number of values in the distribution. 1 for float.
	 */
	Int32 GetValueCount() const
	{
		return 1;
	}

public:
	/** Can this variable be baked out to a FRawDistribution? Should be true 99% of the time*/
	UInt8 canBeBaked_ : 1;

	/** Set internally when the distribution is updated so that that FRawDistribution can know to update itself*/
	UInt8 isDirty_ : 1;

protected:
	UInt8 bakedDataSuccesfully_ : 1;	//It's possible that even though we want to bake we are not able to because of content or code.
};

}
