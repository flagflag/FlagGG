#pragma once

#include "Math/Distributions/Distribution.h"
#include "Math/Distributions/Distributions.h"

namespace FlagGG
{

enum DistributionVectorLockFlags
{
	EDVLF_None,
	EDVLF_XY,
	EDVLF_XZ,
	EDVLF_YZ,
	EDVLF_XYZ,
	EDVLF_MAX,
};

enum DistributionVectorMirrorFlags
{
	EDVMF_Same,
	EDVMF_Different,
	EDVMF_Mirror,
	EDVMF_MAX,
};

class DistributionVector;

struct FlagGG_API RawDistributionVector : public RawDistribution
{
public:
	/** Whether the distribution data has been cooked or the object itself is available */
	bool IsCreated();

	RawDistributionVector()
		: minValue_(0)
		, maxValue_(0)
		, minValueVec_(Vector3::ZERO)
		, maxValueVec_(Vector3::ZERO)
	{
	}


#if WITH_EDITOR
	/**
	* Initialize a raw distribution from the original Unreal distribution
	*/
	void Initialize();
#endif

	/**
	* Gets a pointer to the raw distribution if you can just call FRawDistribution::GetValue3 on it, otherwise NULL
	*/
	const RawDistribution* GetFastRawDistribution();

	/**
	* Get the value at the specified F
	*/
	Vector3 GetValue(float f = 0.0f, Object* data = NULL, Int32 lastExtreme = 0, struct RandomStream* randomStream = NULL);

	/**
	* Get the min and max values
	*/
	void GetOutRange(float& MinOut, float& MaxOut);

	/**
	* Get the min and max values
	*/
	void GetRange(Vector3& MinOut, Vector3& MaxOut);

	/**
	* Is this distribution a uniform type? (ie, does it have two values per entry?)
	*/
	inline bool IsUniform() { return lookupTable_.subEntryStride_ != 0; }

	void InitLookupTable();

	FORCEINLINE bool HasLookupTable(bool bInitializeIfNeeded = true)
	{
#if WITH_EDITOR
		if (bInitializeIfNeeded)
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

	SharedPtr<DistributionVector> distribution_;

private:
	float minValue_;

	float maxValue_;

	Vector3 minValueVec_;

	Vector3 maxValueVec_;
};

class FlagGG_API DistributionVector : public Distribution
{
	OBJECT_OVERRIDE(DistributionVector, Distribution);
public:
	/** Script-accessible way to query a FVector distribution */
	virtual Vector3 GetVectorValue(float f = 0);


	DistributionVector()
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
	 * Returns the lock axes flag used at runtime to swizzle random stream values.
	 */
	virtual UInt8 GetLockFlag() const { return 0; }

	/**
	 * Fill out an array of vectors and return the number of elements in the entry
	 *
	 * @param Time The time to evaluate the distribution
	 * @param Values An array of values to be filled out, guaranteed to be big enough for 2 vectors
	 * @return The number of elements (values) set in the array
	 */
	virtual UInt32 InitializeRawEntry(float time, float* values) const;

	virtual Vector3	GetValue(float f = 0.f, Object* data = NULL, Int32 lastExtreme = 0, struct RandomStream* randomStream = NULL) const;

	//~ Begin CurveEdInterface Interface
	virtual void GetInRange(float& minIn, float& maxIn) const override;
	virtual void GetOutRange(float& minOut, float& maxOut) const override;
	virtual	void GetRange(Vector3& outMin, Vector3& outMax) const;
	//~ End CurveEdInterface Interface

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
	 * Returns the number of values in the distribution. 3 for vector.
	 */
	Int32 GetValueCount() const
	{
		return 3;
	}

public:
	/** Can this variable be baked out to a FRawDistribution? Should be true 99% of the time*/
	Int8 canBeBaked_ : 1;

	/** Set internally when the distribution is updated so that that FRawDistribution can know to update itself*/
	Int8 isDirty_ : 1;

protected:
	Int8 bakedDataSuccesfully_ : 1;	//It's possible that even though we want to bake we are not able to because of content or code.
};

}
