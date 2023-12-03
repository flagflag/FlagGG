#include "ParticleModuleSpawn.h"
#include "Math/Distributions/DistributionFloatConstant.h"
#include "Math/Distributions/DistributionFloatConstantCurve.h"

namespace FlagGG
{

ParticleModuleSpawnBase::ParticleModuleSpawnBase()
	: processSpawnRate_(true)
	, processBurstList_(true)
{

}

/*-----------------------------------------------------------------------------
	Abstract base modules used for categorization.
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	ParticleModuleSpawn implementation.
-----------------------------------------------------------------------------*/
ParticleModuleSpawn::ParticleModuleSpawn()
{
	processSpawnRate_ = true;
	LODDuplicate_ = false;
	applyGlobalSpawnRateScale_ = true;
}

void ParticleModuleSpawn::InitializeDefaults()
{
	if(!rate_.IsCreated())
	{
		DistributionFloatConstant* requiredDistributionSpawnRate = new DistributionFloatConstant();
		requiredDistributionSpawnRate->constant_ = 20.0f;
		rate_.distribution_ = requiredDistributionSpawnRate;
	}

	if(!rateScale_.IsCreated())
	{
		DistributionFloatConstant* requiredDistributionSpawnRateScale = new DistributionFloatConstant();
		requiredDistributionSpawnRateScale->constant_ = 1.0f;
		rateScale_.distribution_ = requiredDistributionSpawnRateScale;
	}

	if(!burstScale_.IsCreated())
	{
		DistributionFloatConstant* burstScaleDistribution = new DistributionFloatConstant();
		burstScaleDistribution->constant_ = 1.0f;
		burstScale_.distribution_ = burstScaleDistribution;
	}
}

bool ParticleModuleSpawn::GetSpawnAmount(ParticleEmitterInstance* Owner, 
	Int32 Offset, float OldLeftover, float DeltaTime, Int32& Number, float& InRate)
{
	ASSERT(Owner);
	return false;
}

bool ParticleModuleSpawn::GenerateLODModuleValues(ParticleModule* sourceModule, float percentage, ParticleLODLevel* LODLevel)
{
	// Convert the module values
	ParticleModuleSpawn* spawnSource = Cast<ParticleModuleSpawn>(sourceModule);
	if (!spawnSource)
	{
		return false;
	}

	bool bResult	= true;
	if (/*FPlatformProperties::HasEditorOnlyData()*/true)
	{
		//SpawnRate
		// TODO: Make sure these functions are never called on console, or when the UDistributions are missing
		if (ConvertFloatDistribution(rate_.distribution_, spawnSource->rate_.distribution_, percentage) == false)
		{
			bResult	= false;
		}

		//ParticleBurstMethod
		//BurstList
		ASSERT(burstList_.Size() == spawnSource->burstList_.Size());
		for (Int32 burstIndex = 0; burstIndex < spawnSource->burstList_.Size(); burstIndex++)
		{
			ParticleBurst* sourceBurst	= &(spawnSource->burstList_[burstIndex]);
			ParticleBurst* burst		= &(burstList_[burstIndex]);

			burst->time_ = sourceBurst->time_;
			// Don't drop below 1...
			if (burst->count_ > 0)
			{
				burst->count_ = FloorToInt(sourceBurst->count_ * (percentage / 100.0f));
				if (burst->count_ == 0)
				{
					burst->count_ = 1;
				}
			}
		}
	}
	return bResult;
}

float ParticleModuleSpawn::GetMaximumSpawnRate()
{
	float minSpawn, maxSpawn;
	float minScale, maxScale;

	rate_.GetOutRange(minSpawn, maxSpawn);
	rateScale_.GetOutRange(minScale, maxScale);

	return (maxSpawn * maxScale);
}

float ParticleModuleSpawn::GetEstimatedSpawnRate()
{
	float minSpawn, maxSpawn;
	float minScale, maxScale;

	rate_.GetOutRange(minSpawn, maxSpawn);
	rateScale_.GetOutRange(minScale, maxScale);

	DistributionFloatConstantCurve* rateScaleCurve = Cast<DistributionFloatConstantCurve>(rateScale_.distribution_);
	if (rateScaleCurve != NULL)
	{
		// We need to walk the curve and determine the average
		Int32 keyCount = rateScaleCurve->GetNumKeys();
		if (keyCount > 1)
		{
			float summedAverage = 0.0f;
			float lastKeyIn = rateScaleCurve->GetKeyIn(keyCount - 1);
			float prevKeyIn = Max<float>(0.0f, rateScaleCurve->GetKeyIn(0));
			float totalTime = Max<float>(1.0f, lastKeyIn - prevKeyIn);
			float prevKeyOut = rateScaleCurve->GetKeyOut(0, 0);
			for (Int32 keyIndex = 1; keyIndex < keyCount; keyIndex++)
			{
				float KeyIn = rateScaleCurve->GetKeyIn(keyIndex);
				float KeyOut = rateScaleCurve->GetKeyOut(0, keyIndex);

				float delta = (KeyIn - prevKeyIn) / totalTime;
				float avg = (KeyOut + prevKeyOut) / 2.0f;
				summedAverage += delta * avg;

				prevKeyIn = KeyIn;
				prevKeyOut = KeyOut;
			}

			maxScale = summedAverage;
		}
	}

	// We need to estimate the value for curves to prevent short spikes from inflating the value... 
	DistributionFloatConstantCurve* rateCurve = Cast<DistributionFloatConstantCurve>(rate_.distribution_);
	if (rateCurve != NULL)
	{
		// We need to walk the curve and determine the average
		Int32 keyCount = rateCurve->GetNumKeys();
		if (keyCount > 1)
		{
			float summedAverage = 0.0f;
			float lastKeyIn = rateCurve->GetKeyIn(keyCount - 1);
			float prevKeyIn = Max<float>(0.0f, rateCurve->GetKeyIn(0));
			float totalTime = Max<float>(1.0f, lastKeyIn - prevKeyIn);
			float prevKeyOut = rateCurve->GetKeyOut(0, 0);
			for (Int32 keyIndex = 1; keyIndex < keyCount; keyIndex++)
			{
				float KeyIn = rateCurve->GetKeyIn(keyIndex);
				float KeyOut = rateCurve->GetKeyOut(0, keyIndex);

				float Delta = (KeyIn - prevKeyIn) / totalTime;
				float Avg = ((KeyOut + prevKeyOut) * maxScale) / 2.0f;
				summedAverage += Delta * Avg;

				prevKeyIn = KeyIn;
				prevKeyOut = KeyOut;
			}

			maxSpawn = summedAverage;// / (Max<float>(1.0f, lastKeyIn));
			return maxSpawn;
		}
	}

	return (maxSpawn * maxScale);
}

Int32 ParticleModuleSpawn::GetMaximumBurstCount()
{
	// Note that this does not take into account entries could be outside of the emitter duration!
	Int32 maxBurst = 0;
	for (Int32 burstIndex = 0; burstIndex < burstList_.Size(); burstIndex++)
	{
		maxBurst += burstList_[burstIndex].count_;
	}
	return maxBurst;
}

float ParticleModuleSpawn::GetGlobalRateScale() const
{
	/*static const auto EmitterRateScaleCVar = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("r.EmitterSpawnRateScale"));
	return (bApplyGlobalSpawnRateScale && EmitterRateScaleCVar) ? EmitterRateScaleCVar->GetValueOnAnyThread() : 1.0f;*/
	return 1.0f;
}

}
