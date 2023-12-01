#include "Math/Distributions/Distribution.h"
#include "Math/Distributions/Distributions.h"
#include "Math/Distributions/DistributionFloat.h"
#include "Math/Distributions/DistributionFloatConstant.h"
#include "Math/Distributions/DistributionFloatConstantCurve.h"
#include "Math/Distributions/DistributionFloatParameterBase.h"
#include "Math/Distributions/DistributionFloatParticleParameter.h"
#include "Math/Distributions/DistributionFloatUniform.h"
#include "Math/Distributions/DistributionFloatUniformCurve.h"
#include "Math/Distributions/DistributionVector.h"
#include "Math/Distributions/DistributionVectorConstant.h"
#include "Math/Distributions/DistributionVectorConstantCurve.h"
#include "Math/Distributions/DistributionVectorParameterBase.h"
#include "Math/Distributions/DistributionVectorParticleParameter.h"
#include "Math/Distributions/DistributionVectorUniform.h"
#include "Math/Distributions/DistributionVectorUniformCurve.h"

namespace FlagGG
{

static bool IsInGameThread() { return true; }

static bool IsInAsyncLoadingThread() { return false; }

// Moving UDistributions to PostInitProps to not be default sub-objects:
// Small enough value to be rounded to 0.0 in the editor 
// but significant enough to properly detect uninitialized defaults.
const float Distribution::defaultValue_ = 1.2345E-20f;

FlagGG_API UInt32 GDistributionType = 1;

// The error threshold used when optimizing lookup table sample counts.
#define LOOKUP_TABLE_ERROR_THRESHOLD (0.05f)

// The maximum number of values to store in a lookup table.
#define LOOKUP_TABLE_MAX_SAMPLES (128)

// Distribution will bake out (if CanBeBaked returns true)
#define DISTRIBUTIONS_BAKES_OUT 1

// The maximum number of samples must be a power of two.
static_assert((LOOKUP_TABLE_MAX_SAMPLES& (LOOKUP_TABLE_MAX_SAMPLES - 1)) == 0, "Lookup table max samples is not a power of two.");

// Log category for distributions.
// DEFINE_LOG_CATEGORY_STATIC(LogDistributions, Warning, Warning);

/*-----------------------------------------------------------------------------
	Lookup table related functions.
------------------------------------------------------------------------------*/

//@todo.CONSOLE: Until we have cooking or something in place, these need to be exposed.
/**
 * Builds a lookup table that returns a constant value.
 * @param OutTable - The table to build.
 * @param ValuesPerEntry - The number of values per entry in the table.
 * @param Values - The values to place in the table.
 */
static void BuildConstantLookupTable(DistributionLookupTable* OutTable, Int32 ValuesPerEntry, const float* Values)
{
	ASSERT(IsInGameThread() || IsInAsyncLoadingThread());
	ASSERT(OutTable != NULL);
	ASSERT(Values != NULL);

	OutTable->values_.Resize(ValuesPerEntry);
	OutTable->op_ = RDO_None;
	OutTable->entryCount_ = 1;
	OutTable->entryStride_ = ValuesPerEntry;
	OutTable->subEntryStride_ = 0;
	OutTable->timeBias_ = 0.0f;
	OutTable->timeScale_ = 0.0f;
	for (Int32 ValueIndex = 0; ValueIndex < ValuesPerEntry; ++ValueIndex)
	{
		OutTable->values_[ValueIndex] = Values[ValueIndex];
	}
}

/**
 * Builds a lookup table that returns zero.
 * @param OutTable - The table to build.
 * @param ValuesPerEntry - The number of values per entry in the table.
 */
static void BuildZeroLookupTable(DistributionLookupTable* OutTable, Int32 ValuesPerEntry)
{
	ASSERT(OutTable != NULL);
	ASSERT(ValuesPerEntry >= 1 && ValuesPerEntry <= 4);

	float Zero[4] = { 0 };
	BuildConstantLookupTable(OutTable, ValuesPerEntry, Zero);
}

/**
 * Builds a lookup table from a distribution.
 * @param OutTable - The table to build.
 * @param Distribution - The distribution for which to build a lookup table.
 */
template <typename DistributionType>
void BuildLookupTable(DistributionLookupTable* OutTable, const DistributionType& Distribution)
{
	ASSERT(IsInGameThread() || IsInAsyncLoadingThread());
	ASSERT(Distribution);

	// Always clear the table.
	OutTable->Empty();

	// Nothing to do if we don't have a distribution.
	if (!Distribution->CanBeBaked())
	{
		BuildZeroLookupTable(OutTable, Distribution->GetValueCount());
		return;
	}

	// Always build a lookup table of maximal size. This can/will be optimized later.
	const Int32 EntryCount = LOOKUP_TABLE_MAX_SAMPLES;

	// Determine the domain of the distribution.
	float MinIn, MaxIn;
	Distribution->GetInRange(MinIn, MaxIn);
	const float TimeScale = (MaxIn - MinIn) / (float)(EntryCount - 1);

	// Get the operation to use, and calculate the number of values needed for that operation.
	const UInt8 Op = Distribution->GetOperation();
	const Int32 ValuesPerEntry = Distribution->GetValueCount();
	const UInt32 EntryStride = ((Op == RDO_None) ? 1 : 2) * (UInt32)ValuesPerEntry;

	// Get the lock flag to use.
	const UInt8 LockFlag = Distribution->GetLockFlag();

	// Allocate a lookup table of the appropriate size.
	OutTable->op_ = Op;
	OutTable->entryCount_ = EntryCount;
	OutTable->entryStride_ = EntryStride;
	OutTable->subEntryStride_ = (Op == RDO_None) ? 0 : ValuesPerEntry;
	OutTable->timeScale_ = (TimeScale > 0.0f) ? (1.0f / TimeScale) : 0.0f;
	OutTable->timeBias_ = MinIn;
	OutTable->values_.Resize(EntryCount * EntryStride);
	for (auto& value : OutTable->values_) value = 0.f;
	OutTable->lockFlag_ = LockFlag;

	// Sample the distribution.
	for (UInt32 SampleIndex = 0; SampleIndex < EntryCount; SampleIndex++)
	{
		const float Time = MinIn + SampleIndex * TimeScale;
		float Values[8];
		Distribution->InitializeRawEntry(Time, Values);
		for (UInt32 ValueIndex = 0; ValueIndex < EntryStride; ValueIndex++)
		{
			OutTable->values_[SampleIndex * EntryStride + ValueIndex] = Values[ValueIndex];
		}
	}
}

/**
 * Appends one lookup table to another.
 * @param Table - Table which contains the first set of components [1-3].
 * @param OtherTable - Table to append which contains a single component.
 */
static void AppendLookupTable(DistributionLookupTable* Table, const DistributionLookupTable& OtherTable)
{
	ASSERT(IsInGameThread() || IsInAsyncLoadingThread());
	ASSERT(Table != NULL);
	ASSERT(Table->GetValuesPerEntry() >= 1 && Table->GetValuesPerEntry() <= 3);
	ASSERT(OtherTable.GetValuesPerEntry() == 1);

	// Copy the input table.
	DistributionLookupTable TableCopy = *Table;

	// Compute the domain of the composed distribution.
	const float OneOverTimeScale = (TableCopy.timeScale_ == 0.0f) ? 0.0f : 1.0f / TableCopy.timeScale_;
	const float OneOverOtherTimeScale = (OtherTable.timeScale_ == 0.0f) ? 0.0f : 1.0f / OtherTable.timeScale_;
	const float MinIn = Min(TableCopy.timeBias_, OtherTable.timeBias_);
	const float MaxIn = Max(TableCopy.timeBias_ + (TableCopy.entryCount_ - 1) * OneOverTimeScale, OtherTable.timeBias_ + (OtherTable.entryCount_ - 1) * OneOverOtherTimeScale);

	const Int32 InValuesPerEntry = TableCopy.GetValuesPerEntry();
	const Int32 OtherValuesPerEntry = 1;
	const Int32 NewValuesPerEntry = InValuesPerEntry + OtherValuesPerEntry;
	const UInt8 NewOp = (TableCopy.op_ == RDO_None) ? OtherTable.op_ : TableCopy.op_;
	const Int32 NewEntryCount = LOOKUP_TABLE_MAX_SAMPLES;
	const Int32 NewStride = (NewOp == RDO_None) ? NewValuesPerEntry : NewValuesPerEntry * 2;
	const float NewTimeScale = (MaxIn - MinIn) / (float)(NewEntryCount - 1);

	// Now build the new lookup table.
	Table->op_ = NewOp;
	Table->entryCount_ = NewEntryCount;
	Table->entryStride_ = NewStride;
	Table->subEntryStride_ = (NewOp == RDO_None) ? 0 : NewValuesPerEntry;
	Table->timeScale_ = (NewTimeScale > 0.0f) ? 1.0f / NewTimeScale : 0.0f;
	Table->timeBias_ = MinIn;
	Table->values_.Resize(NewEntryCount * NewStride);
	for (auto& value : Table->values_) value = 0.f;
	for (Int32 SampleIndex = 0; SampleIndex < NewEntryCount; ++SampleIndex)
	{
		const float* InEntry1;
		const float* InEntry2;
		const float* OtherEntry1;
		const float* OtherEntry2;
		float InLerpAlpha;
		float OtherLerpAlpha;

		const float Time = MinIn + SampleIndex * NewTimeScale;
		TableCopy.GetEntry(Time, InEntry1, InEntry2, InLerpAlpha);
		OtherTable.GetEntry(Time, OtherEntry1, OtherEntry2, OtherLerpAlpha);

		// Store sub-entry 1.
		for (Int32 ValueIndex = 0; ValueIndex < InValuesPerEntry; ++ValueIndex)
		{
			Table->values_[SampleIndex * NewStride + ValueIndex] =
				Lerp(InEntry1[ValueIndex], InEntry2[ValueIndex], InLerpAlpha);
		}
		Table->values_[SampleIndex * NewStride + InValuesPerEntry] =
			Lerp(OtherEntry1[0], OtherEntry2[0], OtherLerpAlpha);

		// Store sub-entry 2 if needed. 
		if (NewOp != RDO_None)
		{
			InEntry1 += TableCopy.subEntryStride_;
			InEntry2 += TableCopy.subEntryStride_;
			OtherEntry1 += OtherTable.subEntryStride_;
			OtherEntry2 += OtherTable.subEntryStride_;

			for (Int32 ValueIndex = 0; ValueIndex < InValuesPerEntry; ++ValueIndex)
			{
				Table->values_[SampleIndex * NewStride + NewValuesPerEntry + ValueIndex] =
					Lerp(InEntry1[ValueIndex], InEntry2[ValueIndex], InLerpAlpha);
			}
			Table->values_[SampleIndex * NewStride + NewValuesPerEntry + InValuesPerEntry] =
				Lerp(OtherEntry1[0], OtherEntry2[0], OtherLerpAlpha);
		}
	}
}

/**
 * Keeps only the first components of each entry in the table.
 * @param Table - Table to slice.
 * @param ChannelsToKeep - The number of channels to keep.
 */
static void SliceLookupTable(DistributionLookupTable* Table, Int32 ChannelsToKeep)
{
	ASSERT(IsInGameThread() || IsInAsyncLoadingThread());
	ASSERT(Table != NULL);
	ASSERT(Table->GetValuesPerEntry() >= ChannelsToKeep);

	// If the table only has the requested number of channels there is nothing to do.
	if (Table->GetValuesPerEntry() == ChannelsToKeep)
	{
		return;
	}

	// Copy the table.
	DistributionLookupTable OldTable = *Table;

	// The new table will have the same number of entries as the input table.
	const Int32 NewEntryCount = OldTable.entryCount_;
	const Int32 NewStride = (OldTable.op_ == RDO_None) ? (ChannelsToKeep) : (2 * ChannelsToKeep);
	Table->op_ = OldTable.op_;
	Table->entryCount_ = NewEntryCount;
	Table->entryStride_ = NewStride;
	Table->subEntryStride_ = (OldTable.op_ == RDO_None) ? (0) : (ChannelsToKeep);
	Table->timeBias_ = OldTable.timeBias_;
	Table->timeScale_ = OldTable.timeScale_;
	Table->values_.Resize(NewEntryCount * NewStride);
	for (auto& value : Table->values_) value = 0.f;

	// Copy values over.
	for (Int32 EntryIndex = 0; EntryIndex < NewEntryCount; ++EntryIndex)
	{
		const float* RESTRICT SrcValues = &OldTable.values_[EntryIndex * OldTable.entryStride_];
		float* RESTRICT DestValues = &Table->values_[EntryIndex * Table->entryStride_];
		for (Int32 ValueIndex = 0; ValueIndex < ChannelsToKeep; ++ValueIndex)
		{
			DestValues[ValueIndex] = SrcValues[ValueIndex];
		}
		if (OldTable.subEntryStride_ > 0)
		{
			SrcValues += OldTable.subEntryStride_;
			DestValues += Table->subEntryStride_;
			for (Int32 ValueIndex = 0; ValueIndex < ChannelsToKeep; ++ValueIndex)
			{
				DestValues[ValueIndex] = SrcValues[ValueIndex];
			}
		}
	}
}

/**
 * Scales each value in the lookup table by a constant.
 * @param InTable - Table to be scaled.
 * @param Scale - The amount to scale by.
 */
static void ScaleLookupTableByConstant(DistributionLookupTable* Table, float Scale)
{
	ASSERT(IsInGameThread() || IsInAsyncLoadingThread());
	ASSERT(Table != NULL);

	for (Int32 ValueIndex = 0; ValueIndex < Table->values_.Size(); ++ValueIndex)
	{
		Table->values_[ValueIndex] *= Scale;
	}
}

/**
 * Scales each value in the lookup table by a constant.
 * @param InTable - Table to be scaled.
 * @param Scale - The amount to scale by.
 * @param ValueCount - The number of scale values.
 */
static void ScaleLookupTableByConstants(DistributionLookupTable* Table, const float* Scale, Int32 ValueCount)
{
	ASSERT(IsInGameThread() || IsInAsyncLoadingThread());
	ASSERT(Table != NULL);
	ASSERT(ValueCount == Table->GetValuesPerEntry());

	const Int32 EntryCount = Table->entryCount_;
	const Int32 SubEntryCount = (Table->subEntryStride_ > 0) ? 2 : 1;
	const Int32 Stride = Table->entryStride_;
	const Int32 SubEntryStride = Table->subEntryStride_;
	float* RESTRICT Values = &(Table->values_[0]);

	for (Int32 Index = 0; Index < EntryCount; ++Index)
	{
		float* RESTRICT EntryValues = Values;
		for (Int32 SubEntryIndex = 0; SubEntryIndex < SubEntryCount; ++SubEntryIndex)
		{
			for (Int32 ValueIndex = 0; ValueIndex < ValueCount; ++ValueIndex)
			{
				EntryValues[ValueIndex] *= Scale[ValueIndex];
			}
			EntryValues += SubEntryStride;
		}
		Values += Stride;
	}
}

/**
 * Adds a constant to each value in the lookup table.
 * @param InTable - Table to be scaled.
 * @param Addend - The amount to add by.
 * @param ValueCount - The number of values per entry.
 */
static void AddConstantToLookupTable(DistributionLookupTable* Table, const float* Addend, Int32 ValueCount)
{
	ASSERT(IsInGameThread() || IsInAsyncLoadingThread());
	ASSERT(Table != NULL);
	ASSERT(ValueCount == Table->GetValuesPerEntry());

	const Int32 EntryCount = Table->entryCount_;
	const Int32 SubEntryCount = (Table->subEntryStride_ > 0) ? 2 : 1;
	const Int32 Stride = Table->entryStride_;
	const Int32 SubEntryStride = Table->subEntryStride_;
	float* RESTRICT Values = &(Table->values_[0]);

	for (Int32 Index = 0; Index < EntryCount; ++Index)
	{
		float* RESTRICT EntryValues = Values;
		for (Int32 SubEntryIndex = 0; SubEntryIndex < SubEntryCount; ++SubEntryIndex)
		{
			for (Int32 ValueIndex = 0; ValueIndex < ValueCount; ++ValueIndex)
			{
				EntryValues[ValueIndex] += Addend[ValueIndex];
			}
			EntryValues += SubEntryStride;
		}
		Values += Stride;
	}
}

/**
 * Scales one lookup table by another.
 * @param Table - The table to scale.
 * @param OtherTable - The table to scale by. Must have one value per entry OR the same values per entry as Table.
 */
static void ScaleLookupTableByLookupTable(DistributionLookupTable* Table, const DistributionLookupTable& OtherTable)
{
	ASSERT(IsInGameThread() || IsInAsyncLoadingThread());
	ASSERT(Table != NULL);
	ASSERT(OtherTable.GetValuesPerEntry() == 1 || OtherTable.GetValuesPerEntry() == Table->GetValuesPerEntry());

	// Copy the original table.
	DistributionLookupTable InTable = *Table;

	// Compute the domain of the composed distribution.
	const float OneOverTimeScale = (InTable.timeScale_ == 0.0f) ? 0.0f : 1.0f / InTable.timeScale_;
	const float OneOverOtherTimeScale = (OtherTable.timeScale_ == 0.0f) ? 0.0f : 1.0f / OtherTable.timeScale_;
	const float MinIn = Min(InTable.timeBias_, OtherTable.timeBias_);
	const float MaxIn = Max(InTable.timeBias_ + (InTable.entryCount_ - 1) * OneOverTimeScale, OtherTable.timeBias_ + (OtherTable.entryCount_ - 1) * OneOverOtherTimeScale);

	const Int32 ValuesPerEntry = InTable.GetValuesPerEntry();
	const Int32 OtherValuesPerEntry = OtherTable.GetValuesPerEntry();
	const UInt8 NewOp = (InTable.op_ == RDO_None) ? OtherTable.op_ : InTable.op_;
	const Int32 NewEntryCount = LOOKUP_TABLE_MAX_SAMPLES;
	const Int32 NewStride = (NewOp == RDO_None) ? ValuesPerEntry : ValuesPerEntry * 2;
	const float NewTimeScale = (MaxIn - MinIn) / (float)(NewEntryCount - 1);

	// Now build the new lookup table.
	Table->op_ = NewOp;
	Table->entryCount_ = NewEntryCount;
	Table->entryStride_ = NewStride;
	Table->subEntryStride_ = (NewOp == RDO_None) ? 0 : ValuesPerEntry;
	Table->timeScale_ = (NewTimeScale > 0.0f) ? 1.0f / NewTimeScale : 0.0f;
	Table->timeBias_ = MinIn;
	Table->values_.Resize(NewEntryCount * NewStride);
	for (auto& value : Table->values_) value = 0.f;
	for (Int32 SampleIndex = 0; SampleIndex < NewEntryCount; ++SampleIndex)
	{
		const float* InEntry1;
		const float* InEntry2;
		const float* OtherEntry1;
		const float* OtherEntry2;
		float InLerpAlpha;
		float OtherLerpAlpha;

		const float Time = MinIn + SampleIndex * NewTimeScale;
		InTable.GetEntry(Time, InEntry1, InEntry2, InLerpAlpha);
		OtherTable.GetEntry(Time, OtherEntry1, OtherEntry2, OtherLerpAlpha);

		// Scale sub-entry 1.
		for (Int32 ValueIndex = 0; ValueIndex < ValuesPerEntry; ++ValueIndex)
		{
			Table->values_[SampleIndex * NewStride + ValueIndex] =
				Lerp(InEntry1[ValueIndex], InEntry2[ValueIndex], InLerpAlpha) *
				Lerp(OtherEntry1[ValueIndex % OtherValuesPerEntry], OtherEntry2[ValueIndex % OtherValuesPerEntry], OtherLerpAlpha);
		}

		// Scale sub-entry 2 if needed. 
		if (NewOp != RDO_None)
		{
			InEntry1 += InTable.subEntryStride_;
			InEntry2 += InTable.subEntryStride_;
			OtherEntry1 += OtherTable.subEntryStride_;
			OtherEntry2 += OtherTable.subEntryStride_;

			for (Int32 ValueIndex = 0; ValueIndex < ValuesPerEntry; ++ValueIndex)
			{
				Table->values_[SampleIndex * NewStride + ValuesPerEntry + ValueIndex] =
					Lerp(InEntry1[ValueIndex], InEntry2[ValueIndex], InLerpAlpha) *
					Lerp(OtherEntry1[ValueIndex % OtherValuesPerEntry], OtherEntry2[ValueIndex % OtherValuesPerEntry], OtherLerpAlpha);
			}
		}
	}
}

/**
 * Adds the values in one lookup table by another.
 * @param Table - The table to which to add values.
 * @param OtherTable - The table from which to add values. Must have one value per entry OR the same values per entry as Table.
 */
static void AddLookupTableToLookupTable(DistributionLookupTable* Table, const DistributionLookupTable& OtherTable)
{
	ASSERT(IsInGameThread() || IsInAsyncLoadingThread());
	ASSERT(Table != NULL);
	ASSERT(OtherTable.GetValuesPerEntry() == 1 || OtherTable.GetValuesPerEntry() == Table->GetValuesPerEntry());

	// Copy the original table.
	DistributionLookupTable InTable = *Table;

	// Compute the domain of the composed distribution.
	const float OneOverTimeScale = (InTable.timeScale_ == 0.0f) ? 0.0f : 1.0f / InTable.timeScale_;
	const float OneOverOtherTimeScale = (OtherTable.timeScale_ == 0.0f) ? 0.0f : 1.0f / OtherTable.timeScale_;
	const float MinIn = Min(InTable.timeBias_, OtherTable.timeBias_);
	const float MaxIn = Max(InTable.timeBias_ + (InTable.entryCount_ - 1) * OneOverTimeScale, OtherTable.timeBias_ + (OtherTable.entryCount_ - 1) * OneOverOtherTimeScale);

	const Int32 ValuesPerEntry = InTable.GetValuesPerEntry();
	const Int32 OtherValuesPerEntry = OtherTable.GetValuesPerEntry();
	const UInt8 NewOp = (InTable.op_ == RDO_None) ? OtherTable.op_ : InTable.op_;
	const Int32 NewEntryCount = LOOKUP_TABLE_MAX_SAMPLES;
	const Int32 NewStride = (NewOp == RDO_None) ? ValuesPerEntry : ValuesPerEntry * 2;
	const float NewTimeScale = (MaxIn - MinIn) / (float)(NewEntryCount - 1);

	// Now build the new lookup table.
	Table->op_ = NewOp;
	Table->entryCount_ = NewEntryCount;
	Table->entryStride_ = NewStride;
	Table->subEntryStride_ = (NewOp == RDO_None) ? 0 : ValuesPerEntry;
	Table->timeScale_ = (NewTimeScale > 0.0f) ? 1.0f / NewTimeScale : 0.0f;
	Table->timeBias_ = MinIn;
	Table->values_.Resize(NewEntryCount * NewStride);
	for (auto& value : Table->values_) value = 0.f;
	for (Int32 SampleIndex = 0; SampleIndex < NewEntryCount; ++SampleIndex)
	{
		const float* InEntry1;
		const float* InEntry2;
		const float* OtherEntry1;
		const float* OtherEntry2;
		float InLerpAlpha;
		float OtherLerpAlpha;

		const float Time = MinIn + SampleIndex * NewTimeScale;
		InTable.GetEntry(Time, InEntry1, InEntry2, InLerpAlpha);
		OtherTable.GetEntry(Time, OtherEntry1, OtherEntry2, OtherLerpAlpha);

		// Scale sub-entry 1.
		for (Int32 ValueIndex = 0; ValueIndex < ValuesPerEntry; ++ValueIndex)
		{
			Table->values_[SampleIndex * NewStride + ValueIndex] =
				Lerp(InEntry1[ValueIndex], InEntry2[ValueIndex], InLerpAlpha) +
				Lerp(OtherEntry1[ValueIndex % OtherValuesPerEntry], OtherEntry2[ValueIndex % OtherValuesPerEntry], OtherLerpAlpha);
		}

		// Scale sub-entry 2 if needed. 
		if (NewOp != RDO_None)
		{
			InEntry1 += InTable.subEntryStride_;
			InEntry2 += InTable.subEntryStride_;
			OtherEntry1 += OtherTable.subEntryStride_;
			OtherEntry2 += OtherTable.subEntryStride_;

			for (Int32 ValueIndex = 0; ValueIndex < ValuesPerEntry; ++ValueIndex)
			{
				Table->values_[SampleIndex * NewStride + ValuesPerEntry + ValueIndex] =
					Lerp(InEntry1[ValueIndex], InEntry2[ValueIndex], InLerpAlpha) +
					Lerp(OtherEntry1[ValueIndex % OtherValuesPerEntry], OtherEntry2[ValueIndex % OtherValuesPerEntry], OtherLerpAlpha);
			}
		}
	}
}

/**
 * Computes the L2 norm between the samples in ValueCount dimensional space.
 * @param Values1 - Array of ValueCount values.
 * @param Values2 - Array of ValueCount values.
 * @param ValueCount - The number of values in the two arrays.
 * @returns the L2 norm of the difference of the vectors represented by the two float arrays.
 */
static float ComputeSampleDistance(const float* Values1, const float* Values2, Int32 ValueCount)
{
	float Dist = 0.0f;
	for (Int32 ValueIndex = 0; ValueIndex < ValueCount; ++ValueIndex)
	{
		const float Diff = Values1[ValueIndex] - Values2[ValueIndex];
		Dist += (Diff * Diff);
	}
	return Sqrt(Dist);
}

/**
 * Computes the chordal distance between the curves represented by the two tables.
 * @param Table1 - The first table to compare.
 * @param Table2 - The second table to compare.
 * @param MinIn - The time at which to begin comparing.
 * @param MaxIn - The time at which to stop comparing.
 * @param SampleCount - The number of samples to use.
 * @returns the chordal distance representing the error introduced by substituting one table for the other.
 */
static float ComputeLookupTableError(const DistributionLookupTable& InTable1, const DistributionLookupTable& InTable2, float MinIn, float MaxIn, Int32 SampleCount)
{
	ASSERT(InTable1.entryStride_ == InTable2.entryStride_);
	ASSERT(InTable1.subEntryStride_ == InTable2.subEntryStride_);
	ASSERT(SampleCount > 0);

	const DistributionLookupTable* Table1 = (InTable2.entryCount_ > InTable1.entryCount_) ? &InTable2 : &InTable1;
	const DistributionLookupTable* Table2 = (Table1 == &InTable1) ? &InTable2 : &InTable1;
	const Int32 ValuesPerEntry = Table1->GetValuesPerEntry();
	const float TimeStep = (MaxIn - MinIn) / (SampleCount - 1);

	float Values1[4] = { 0 };
	float Values2[4] = { 0 };
	float Error = 0.0f;
	float Time = MinIn;
	for (Int32 SampleIndex = 0; SampleIndex < SampleCount; ++SampleIndex, Time += TimeStep)
	{
		const float* Table1Entry1 = NULL;
		const float* Table1Entry2 = NULL;
		float Table1LerpAlpha = 0.0f;
		const float* Table2Entry1 = NULL;
		const float* Table2Entry2 = NULL;
		float Table2LerpAlpha = 0.0f;

		Table1->GetEntry(Time, Table1Entry1, Table1Entry2, Table1LerpAlpha);
		Table2->GetEntry(Time, Table2Entry1, Table2Entry2, Table2LerpAlpha);
		for (Int32 ValueIndex = 0; ValueIndex < ValuesPerEntry; ++ValueIndex)
		{
			Values1[ValueIndex] = Lerp(Table1Entry1[ValueIndex], Table1Entry2[ValueIndex], Table1LerpAlpha);
			Values2[ValueIndex] = Lerp(Table2Entry1[ValueIndex], Table2Entry2[ValueIndex], Table2LerpAlpha);
		}
		Error = Max<float>(Error, ComputeSampleDistance(Values1, Values2, ValuesPerEntry));

		if (Table1->subEntryStride_ > 0)
		{
			Table1Entry1 += Table1->subEntryStride_;
			Table1Entry2 += Table1->subEntryStride_;
			Table2Entry1 += Table2->subEntryStride_;
			Table2Entry2 += Table2->subEntryStride_;
			for (Int32 ValueIndex = 0; ValueIndex < ValuesPerEntry; ++ValueIndex)
			{
				Values1[ValueIndex] = Lerp(Table1Entry1[ValueIndex], Table1Entry2[ValueIndex], Table1LerpAlpha);
				Values2[ValueIndex] = Lerp(Table2Entry1[ValueIndex], Table2Entry2[ValueIndex], Table2LerpAlpha);
			}
			Error = Max<float>(Error, ComputeSampleDistance(Values1, Values2, ValuesPerEntry));
		}
	}
	return Error;
}

/**
 * Resamples a lookup table.
 * @param OutTable - The resampled table.
 * @param InTable - The table to be resampled.
 * @param MinIn - The time at which to begin resampling.
 * @param MaxIn - The time at which to stop resampling.
 * @param SampleCount - The number of samples to use.
 */
static void ResampleLookupTable(DistributionLookupTable* OutTable, const DistributionLookupTable& InTable, float MinIn, float MaxIn, Int32 SampleCount)
{
	ASSERT(IsInGameThread() || IsInAsyncLoadingThread());
	const Int32 Stride = InTable.entryStride_;
	const float OneOverTimeScale = (InTable.timeScale_ == 0.0f) ? 0.0f : 1.0f / InTable.timeScale_;
	const float timeScale_ = (SampleCount > 1) ? ((MaxIn - MinIn) / (float)(SampleCount - 1)) : 0.0f;

	// Build the resampled table.
	OutTable->op_ = InTable.op_;
	OutTable->entryCount_ = SampleCount;
	OutTable->entryStride_ = InTable.entryStride_;
	OutTable->subEntryStride_ = InTable.subEntryStride_;
	OutTable->timeBias_ = MinIn;
	OutTable->timeScale_ = (timeScale_ > 0.0f) ? (1.0f / timeScale_) : 0.0f;
	OutTable->values_.Resize(SampleCount * InTable.entryStride_);
	for (auto& value : OutTable->values_) value = 0.f;

	// Resample entries in the table.
	for (Int32 SampleIndex = 0; SampleIndex < SampleCount; ++SampleIndex)
	{
		const float* Entry1 = NULL;
		const float* Entry2 = NULL;
		float LerpAlpha = 0.0f;
		const float Time = MinIn + timeScale_ * SampleIndex;
		InTable.GetEntry(Time, Entry1, Entry2, LerpAlpha);
		for (Int32 ValueIndex = 0; ValueIndex < Stride; ++ValueIndex)
		{
			OutTable->values_[SampleIndex * Stride + ValueIndex] =
				Lerp(Entry1[ValueIndex], Entry2[ValueIndex], LerpAlpha);
		}
	}
}

/**
 * Optimizes a lookup table using the minimum number of samples required to represent the distribution.
 * @param Table - The lookup table to optimize.
 * @param ErrorThreshold - Threshold at which the lookup table is considered good enough.
 */
static void OptimizeLookupTable(DistributionLookupTable* Table, float ErrorThreshold)
{
	ASSERT(IsInGameThread() || IsInAsyncLoadingThread());
	ASSERT(Table != NULL);
	ASSERT((Table->entryCount_ & (Table->entryCount_ - 1)) == 0);

	// Domain for the table.
	const float OneOverTimeScale = (Table->timeScale_ == 0.0f) ? 0.0f : 1.0f / Table->timeScale_;
	const float MinIn = Table->timeBias_;
	const float MaxIn = Table->timeBias_ + (Table->entryCount_ - 1) * OneOverTimeScale;

	// Duplicate the table.
	DistributionLookupTable OriginalTable = *Table;

	// Resample the lookup table until error is reduced to an acceptable level.
	const Int32 MinSampleCount = 1;
	const Int32 MaxSampleCount = LOOKUP_TABLE_MAX_SAMPLES;
	for (Int32 SampleCount = MinSampleCount; SampleCount < MaxSampleCount; SampleCount <<= 1)
	{
		ResampleLookupTable(Table, OriginalTable, MinIn, MaxIn, SampleCount);
		if (ComputeLookupTableError(*Table, OriginalTable, MinIn, MaxIn, LOOKUP_TABLE_MAX_SAMPLES) < ErrorThreshold)
		{
			return;
		}
	}

	// The original table is optimal.
	*Table = OriginalTable;
}

void RawDistribution::GetValue1(float Time, float* Value, Int32 Extreme, struct RandomStream* InRandomStream) const
{
	switch (lookupTable_.op_)
	{
	case RDO_None:
		GetValue1None(Time, Value);
		break;
	case RDO_Extreme:
		GetValue1Extreme(Time, Value, Extreme, InRandomStream);
		break;
	case RDO_Random:
		GetValue1Random(Time, Value, InRandomStream);
		break;
	default: // compiler complains
		// ASSERT(0);
		*Value = 0.0f;
		break;
	}
}

void RawDistribution::GetValue3(float Time, float* Value, Int32 Extreme, struct RandomStream* InRandomStream) const
{
	switch (lookupTable_.op_)
	{
	case RDO_None:
		GetValue3None(Time, Value);
		break;
	case RDO_Extreme:
		GetValue3Extreme(Time, Value, Extreme, InRandomStream);
		break;
	case RDO_Random:
		GetValue3Random(Time, Value, InRandomStream);
		break;
	}
}

void RawDistribution::GetValue1Extreme(float Time, float* InValue, Int32 Extreme, struct RandomStream* InRandomStream) const
{
	float* RESTRICT Value = InValue;
	const float* Entry1;
	const float* Entry2;
	float LerpAlpha = 0.0f;
	float RandValue = DIST_GET_RANDOM_VALUE(InRandomStream);
	lookupTable_.GetEntry(Time, Entry1, Entry2, LerpAlpha);
	const float* RESTRICT NewEntry1 = Entry1;
	const float* RESTRICT NewEntry2 = Entry2;
	Int32 InitialElement = ((Extreme > 0) || ((Extreme == 0) && (RandValue > 0.5f)));
	Value[0] = Lerp(NewEntry1[InitialElement + 0], NewEntry2[InitialElement + 0], LerpAlpha);
}

void RawDistribution::GetValue3Extreme(float Time, float* InValue, Int32 Extreme, struct RandomStream* InRandomStream) const
{
	float* RESTRICT Value = InValue;
	const float* Entry1;
	const float* Entry2;
	float LerpAlpha = 0.0f;
	float RandValue = DIST_GET_RANDOM_VALUE(InRandomStream);
	lookupTable_.GetEntry(Time, Entry1, Entry2, LerpAlpha);
	const float* RESTRICT NewEntry1 = Entry1;
	const float* RESTRICT NewEntry2 = Entry2;
	Int32 InitialElement = ((Extreme > 0) || ((Extreme == 0) && (RandValue > 0.5f)));
	InitialElement *= 3;
	float T0 = Lerp(NewEntry1[InitialElement + 0], NewEntry2[InitialElement + 0], LerpAlpha);
	float T1 = Lerp(NewEntry1[InitialElement + 1], NewEntry2[InitialElement + 1], LerpAlpha);
	float T2 = Lerp(NewEntry1[InitialElement + 2], NewEntry2[InitialElement + 2], LerpAlpha);
	Value[0] = T0;
	Value[1] = T1;
	Value[2] = T2;
}

void RawDistribution::GetValue1Random(float Time, float* InValue, struct RandomStream* InRandomStream) const
{
	float* RESTRICT Value = InValue;
	const float* Entry1;
	const float* Entry2;
	float LerpAlpha = 0.0f;
	float RandValue = DIST_GET_RANDOM_VALUE(InRandomStream);
	lookupTable_.GetEntry(Time, Entry1, Entry2, LerpAlpha);
	const float* RESTRICT NewEntry1 = Entry1;
	const float* RESTRICT NewEntry2 = Entry2;
	float Value1, Value2;
	Value1 = Lerp(NewEntry1[0], NewEntry2[0], LerpAlpha);
	Value2 = Lerp(NewEntry1[1 + 0], NewEntry2[1 + 0], LerpAlpha);
	Value[0] = Value1 + (Value2 - Value1) * RandValue;
}

void RawDistribution::GetValue3Random(float Time, float* InValue, struct RandomStream* InRandomStream) const
{
	float* RESTRICT Value = InValue;
	const float* Entry1;
	const float* Entry2;
	float LerpAlpha = 0.0f;
	Vector3 RandValues;

	RandValues[0] = DIST_GET_RANDOM_VALUE(InRandomStream);
	RandValues[1] = DIST_GET_RANDOM_VALUE(InRandomStream);
	RandValues[2] = DIST_GET_RANDOM_VALUE(InRandomStream);
	switch (lookupTable_.lockFlag_)
	{
	case EDVLF_XY:
		RandValues.y_ = RandValues.x_;
		break;
	case EDVLF_XZ:
		RandValues.z_ = RandValues.x_;
		break;
	case EDVLF_YZ:
		RandValues.z_ = RandValues.y_;
		break;
	case EDVLF_XYZ:
		RandValues.y_ = RandValues.x_;
		RandValues.z_ = RandValues.x_;
		break;
	}

	lookupTable_.GetEntry(Time, Entry1, Entry2, LerpAlpha);
	const float* RESTRICT NewEntry1 = Entry1;
	const float* RESTRICT NewEntry2 = Entry2;
	float X0, X1;
	float Y0, Y1;
	float Z0, Z1;
	X0 = Lerp(NewEntry1[0], NewEntry2[0], LerpAlpha);
	Y0 = Lerp(NewEntry1[1], NewEntry2[1], LerpAlpha);
	Z0 = Lerp(NewEntry1[2], NewEntry2[2], LerpAlpha);
	X1 = Lerp(NewEntry1[3 + 0], NewEntry2[3 + 0], LerpAlpha);
	Y1 = Lerp(NewEntry1[3 + 1], NewEntry2[3 + 1], LerpAlpha);
	Z1 = Lerp(NewEntry1[3 + 2], NewEntry2[3 + 2], LerpAlpha);
	Value[0] = X0 + (X1 - X0) * RandValues[0];
	Value[1] = Y0 + (Y1 - Y0) * RandValues[1];
	Value[2] = Z0 + (Z1 - Z0) * RandValues[2];
}

void RawDistribution::GetValue(float Time, float* Value, Int32 NumCoords, Int32 Extreme, struct RandomStream* InRandomStream) const
{
	ASSERT(NumCoords == 3 || NumCoords == 1);
	switch (lookupTable_.op_)
	{
	case RDO_None:
		if (NumCoords == 1)
		{
			GetValue1None(Time, Value);
		}
		else
		{
			GetValue3None(Time, Value);
		}
		break;
	case RDO_Extreme:
		if (NumCoords == 1)
		{
			GetValue1Extreme(Time, Value, Extreme, InRandomStream);
		}
		else
		{
			GetValue3Extreme(Time, Value, Extreme, InRandomStream);
		}
		break;
	case RDO_Random:
		if (NumCoords == 1)
		{
			GetValue1Random(Time, Value, InRandomStream);
		}
		else
		{
			GetValue3Random(Time, Value, InRandomStream);
		}
		break;
	}
}

#if WITH_EDITOR

void RawDistributionFloat::Initialize()
{
	// Nothing to do if we don't have a distribution.
	if (Distribution == NULL)
	{
		return;
	}

	// does this FRawDist need updating? (if UDist is dirty or somehow the distribution wasn't dirty, but we have no data)
	bool bNeedsUpdating = false;
	if (Distribution->isDirty_ || (lookupTable_.IsEmpty() && Distribution->CanBeBaked()))
	{
		if (!Distribution->isDirty_)
		{
			UE_LOG(LogDistributions, Log, TEXT("Somehow Distribution %s wasn't dirty, but its RawDistribution wasn't ever initialized!"), *Distribution->GetFullName());
		}
		bNeedsUpdating = true;
	}
	// only initialize if we need to
	if (!bNeedsUpdating)
	{
		return;
	}
	if (!GIsEditor && !IsInGameThread() && !IsInAsyncLoadingThread())
	{
		return;
	}
	ASSERT(IsInGameThread() || IsInAsyncLoadingThread());

	// always empty out the lookup table
	lookupTable_.Empty();

	// distribution is no longer dirty (if it was)
	// template objects aren't marked as dirty, because any UDists that uses this as an archetype, 
	// aren't the default values, and has already been saved, needs to know to build the FDist
	if (!Distribution->IsTemplate())
	{
		Distribution->isDirty_ = false;
	}

	// if the distribution can't be baked out, then we do nothing here
	if (!Distribution->CanBeBaked())
	{
		return;
	}

	// Build and optimize the lookup table.
	BuildLookupTable(&lookupTable_, Distribution);
	OptimizeLookupTable(&lookupTable_, LOOKUP_TABLE_ERROR_THRESHOLD);

	// fill out our min/max
	Distribution->GetOutRange(MinValue, MaxValue);
}
#endif // WITH_EDITOR

bool RawDistributionFloat::IsCreated()
{
	return HasLookupTable(/*bInitializeIfNeeded=*/ false) || (distribution_ != nullptr);
}

bool RawDistributionVector::IsCreated()
{
	return HasLookupTable(/*bInitializeIfNeeded=*/ false) || (distribution_ != nullptr);
}

float RawDistributionFloat::GetValue(float F, Object* Data, struct RandomStream* InRandomStream)
{
	if (!HasLookupTable())
	{
		if (!distribution_)
		{
			return 0.0f;
		}
		return distribution_->GetValue(F, Data, InRandomStream);
	}

	// if we get here, we better have been initialized!
	ASSERT(!lookupTable_.IsEmpty());

	float Value;
	RawDistribution::GetValue1(F, &Value, 0, InRandomStream);
	return Value;
}

const RawDistribution* RawDistributionFloat::GetFastRawDistribution()
{
	if (!IsSimple() || !HasLookupTable())
	{
		return 0;
	}

	// if we get here, we better have been initialized!
	ASSERT(!lookupTable_.IsEmpty());

	return this;
}

void RawDistributionFloat::GetOutRange(float& MinOut, float& MaxOut)
{
	if (!HasLookupTable() && distribution_)
	{
		ASSERT(distribution_);
		distribution_->GetOutRange(MinOut, MaxOut);
	}
	else
	{
		MinOut = minValue_;
		MaxOut = maxValue_;
	}
}

void RawDistributionFloat::InitLookupTable()
{
#if WITH_EDITOR
	// make sure it's up to date
	if (distribution_)
	{
		if (GIsEditor || distribution_->isDirty_)
		{
			distribution_->ConditionalPostLoad();
			Initialize();
		}
	}
#endif
}

#if WITH_EDITOR
template <typename RawDistributionType>
bool HasBakedDistributionDataHelper(const Distribution* GivenDistribution)
{
	if (Object* Outer = GivenDistribution->GetOuter())
	{
		for (TFieldIterator<FProperty> It(Outer->GetClass()); It; ++It)
		{
			FProperty* Property = *It;

			if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
			{
				Object* Distribution = RawDistribution::TryGetDistributionObjectFromRawDistributionProperty(StructProp, reinterpret_cast<UInt8*>(Outer));
				if (Distribution == GivenDistribution)
				{
					if (RawDistributionType* RawDistributionVector = StructProp->ContainerPtrToValuePtr<RawDistributionType>(reinterpret_cast<UInt8*>(Outer)))
					{
						if (RawDistributionVector->HasLookupTable(false))
						{
							return true;	//We have baked data
						}
					}

					return false;
				}
			}
			else if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
			{
				if (FStructProperty* InnerStructProp = CastField<FStructProperty>(ArrayProp->Inner))
				{
					FScriptArrayHelper ArrayHelper(ArrayProp, Property->ContainerPtrToValuePtr<void>(Outer));
					for (Int32 Idx = 0; Idx < ArrayHelper.Size(); ++Idx)
					{
						for (FProperty* ArrayProperty = InnerStructProp->Struct->PropertyLink; ArrayProperty; ArrayProperty = ArrayProperty->PropertyLinkNext)
						{
							if (FStructProperty* ArrayStructProp = CastField<FStructProperty>(ArrayProperty))
							{
								Object* Distribution = RawDistribution::TryGetDistributionObjectFromRawDistributionProperty(ArrayStructProp, ArrayHelper.GetRawPtr(Idx));
								if (Distribution == GivenDistribution)
								{
									if (RawDistributionType* RawDistributionVector = ArrayStructProp->ContainerPtrToValuePtr<RawDistributionType>(ArrayHelper.GetRawPtr(Idx)))
									{
										if (RawDistributionVector->HasLookupTable(false))
										{
											return true;	//We have baked data
										}
									}

									return false;
								}
							}
						}
					}
				}
			}
		}
	}

	return false;
}
#endif

float DistributionFloat::GetValue(float F, Object* Data, struct RandomStream* InRandomStream) const
{
	return 0.0;
}

float DistributionFloat::GetFloatValue(float F)
{
	return GetValue(F);
}

void DistributionFloat::GetInRange(float& MinIn, float& MaxIn) const
{
	MinIn = 0.0f;
	MaxIn = 0.0f;
}

void DistributionFloat::GetOutRange(float& MinOut, float& MaxOut) const
{
	MinOut = 0.0f;
	MaxOut = 0.0f;
}

//@todo.CONSOLE
UInt32 DistributionFloat::InitializeRawEntry(float Time, float* Values) const
{
	Values[0] = GetValue(Time);
	return 1;
}

#if WITH_EDITOR
void RawDistributionFloat::Initialize()
{
	// Nothing to do if we don't have a distribution.
	if (distribution_ == NULL)
	{
		return;
	}

	// fill out our min/max
	distribution_->GetOutRange(MinValue, MaxValue);
	distribution_->GetRange(MinValueVec, MaxValueVec);

	// does this FRawDist need updating? (if UDist is dirty or somehow the distribution wasn't dirty, but we have no data)
	bool bNeedsUpdating = false;
	if (distribution_->isDirty_ || (lookupTable_.IsEmpty() && distribution_->CanBeBaked()))
	{
		if (!distribution_->isDirty_)
		{
			UE_LOG(LogDistributions, Log, TEXT("Somehow Distribution %s wasn't dirty, but its RawDistribution wasn't ever initialized!"), *Distribution->GetFullName());
		}
		bNeedsUpdating = true;
	}

	// only initialize if we need to
	if (!bNeedsUpdating)
	{
		return;
	}
	ASSERT(IsInGameThread() || IsInAsyncLoadingThread());

	// always empty out the lookup table
	lookupTable_.Empty();

	// distribution is no longer dirty (if it was)
	// template objects aren't marked as dirty, because any UDists that uses this as an archetype, 
	// aren't the default values, and has already been saved, needs to know to build the FDist
	if (!distribution_->IsTemplate())
	{
		distribution_->isDirty_ = false;
	}

	// if the distribution can't be baked out, then we do nothing here
	if (!distribution_->CanBeBaked())
	{
		return;
	}

	// Build and optimize the lookup table.
	BuildLookupTable(&lookupTable_, distribution_);
	const float MinIn = lookupTable_.timeBias_;
	const float MaxIn = MinIn + (lookupTable_.entryCount_ - 1) * (lookupTable_.timeScale_ == 0.0f ? 0.0f : (1.0f / lookupTable_.timeScale_));
	OptimizeLookupTable(&lookupTable_, LOOKUP_TABLE_ERROR_THRESHOLD);

}
#endif

Vector3 RawDistributionVector::GetValue(float F, Object* Data, Int32 Extreme, struct RandomStream* InRandomStream)
{
	if (!HasLookupTable())
	{
		if (!distribution_)
		{
			return Vector3::ZERO;
		}
		return distribution_->GetValue(F, Data, Extreme, InRandomStream);
	}

	// if we get here, we better have been initialized!
	ASSERT(!lookupTable_.IsEmpty());

	Vector3 Value;
	RawDistribution::GetValue3(F, &Value.x_, Extreme, InRandomStream);
	return (Vector3)Value;
}

const RawDistribution* RawDistributionVector::GetFastRawDistribution()
{
	if (!IsSimple() || !HasLookupTable())
	{
		return 0;
	}

	// if we get here, we better have been initialized!
	ASSERT(!lookupTable_.IsEmpty());

	return this;
}

void RawDistributionVector::GetOutRange(float& MinOut, float& MaxOut)
{
	if (!HasLookupTable() && distribution_)
	{
		ASSERT(distribution_);
		distribution_->GetOutRange(MinOut, MaxOut);
	}
	else
	{
		MinOut = minValue_;
		MaxOut = maxValue_;
	}
}

void RawDistributionVector::GetRange(Vector3& MinOut, Vector3& MaxOut)
{
	if (distribution_)
	{
		ASSERT(distribution_);
		distribution_->GetRange(MinOut, MaxOut);
	}
	else
	{
		MinOut = minValueVec_;
		MaxOut = maxValueVec_;
	}
}

void RawDistributionVector::InitLookupTable()
{
#if WITH_EDITOR
	// make sure it's up to date
	if (GIsEditor || (Distribution && Distribution->isDirty_))
	{
		Initialize();
	}
#endif
}

Vector3 DistributionVector::GetValue(float F, Object* Data, Int32 Extreme, struct RandomStream* InRandomStream) const
{
	return Vector3::ZERO;
}

Vector3 DistributionVector::GetVectorValue(float F)
{
	return GetValue(F);
}

void DistributionVector::GetInRange(float& MinIn, float& MaxIn) const
{
	MinIn = 0.0f;
	MaxIn = 0.0f;
}

void DistributionVector::GetOutRange(float& MinOut, float& MaxOut) const
{
	MinOut = 0.0f;
	MaxOut = 0.0f;
}

void DistributionVector::GetRange(Vector3& OutMin, Vector3& OutMax) const
{
	OutMin = Vector3::ZERO;
	OutMax = Vector3::ZERO;
}

//@todo.CONSOLE
UInt32 DistributionVector::InitializeRawEntry(float Time, float* Values) const
{
	Vector3 Value = GetValue(Time);
	Values[0] = Value.x_;
	Values[1] = Value.y_;
	Values[2] = Value.z_;
	return 3;
}

FloatDistribution::FloatDistribution()
{
	BuildZeroLookupTable(&lookupTable_, 1);
}


VectorDistribution::VectorDistribution()
{
	BuildZeroLookupTable(&lookupTable_, 3);
}


Vector4Distribution::Vector4Distribution()
{
	BuildZeroLookupTable(&lookupTable_, 4);
}

//@todo.CONSOLE
void ComposableDistribution::BuildFloat(FloatDistribution& OutDistribution, const ComposableFloatDistribution& X)
{
	checkDistribution(X.lookupTable_.GetValuesPerEntry() == 1);

	OutDistribution.lookupTable_ = X.lookupTable_;
	OptimizeLookupTable(&OutDistribution.lookupTable_, LOOKUP_TABLE_ERROR_THRESHOLD);
}

void ComposableDistribution::BuildVector(VectorDistribution& OutDistribution, const ComposableVectorDistribution& XYZ)
{
	checkDistribution(XYZ.lookupTable_.GetValuesPerEntry() == 3);

	OutDistribution.lookupTable_ = XYZ.lookupTable_;
	OptimizeLookupTable(&OutDistribution.lookupTable_, LOOKUP_TABLE_ERROR_THRESHOLD);
}

void ComposableDistribution::BuildVector4(Vector4Distribution& OutDistribution, const class ComposableVectorDistribution& XYZ, const class ComposableFloatDistribution& W)
{
	checkDistribution(XYZ.lookupTable_.GetValuesPerEntry() == 3);
	checkDistribution(W.lookupTable_.GetValuesPerEntry() == 1);

	OutDistribution.lookupTable_ = XYZ.lookupTable_;
	AppendLookupTable(&OutDistribution.lookupTable_, W.lookupTable_);
	OptimizeLookupTable(&OutDistribution.lookupTable_, LOOKUP_TABLE_ERROR_THRESHOLD);
}


void ComposableDistribution::BuildVector4(
	class Vector4Distribution& OutDistribution,
	const class ComposableVectorDistribution& XY0,
	const class ComposableFloatDistribution& Z,
	const class ComposableFloatDistribution& W)
{
	checkDistribution(XY0.lookupTable_.GetValuesPerEntry() == 3);
	checkDistribution(Z.lookupTable_.GetValuesPerEntry() == 1);
	checkDistribution(W.lookupTable_.GetValuesPerEntry() == 1);

	OutDistribution.lookupTable_ = XY0.lookupTable_;
	SliceLookupTable(&OutDistribution.lookupTable_, 2);
	AppendLookupTable(&OutDistribution.lookupTable_, Z.lookupTable_);
	AppendLookupTable(&OutDistribution.lookupTable_, W.lookupTable_);
	OptimizeLookupTable(&OutDistribution.lookupTable_, LOOKUP_TABLE_ERROR_THRESHOLD);
}

void ComposableDistribution::BuildVector4(
	Vector4Distribution& OutDistribution,
	const class ComposableFloatDistribution& X,
	const class ComposableFloatDistribution& Y,
	const class ComposableFloatDistribution& Z,
	const class ComposableFloatDistribution& W)
{
	checkDistribution(X.lookupTable_.GetValuesPerEntry() == 1);
	checkDistribution(Y.lookupTable_.GetValuesPerEntry() == 1);
	checkDistribution(Z.lookupTable_.GetValuesPerEntry() == 1);
	checkDistribution(W.lookupTable_.GetValuesPerEntry() == 1);

	OutDistribution.lookupTable_ = X.lookupTable_;
	AppendLookupTable(&OutDistribution.lookupTable_, Y.lookupTable_);
	AppendLookupTable(&OutDistribution.lookupTable_, Z.lookupTable_);
	AppendLookupTable(&OutDistribution.lookupTable_, W.lookupTable_);
	OptimizeLookupTable(&OutDistribution.lookupTable_, LOOKUP_TABLE_ERROR_THRESHOLD);
}


void ComposableDistribution::QuantizeVector4(
	PODVector<Color>& OutQuantizedSamples,
	Vector4& OutScale,
	Vector4& OutBias,
	const Vector4Distribution& Distribution)
{
	Vector4 Mins(+FLT_MAX, +FLT_MAX, +FLT_MAX, +FLT_MAX);
	Vector4 Maxs(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);
	const DistributionLookupTable& Table = Distribution.lookupTable_;
	const Int32 EntryCount = Table.entryCount_;
	const Int32 EntryStride = Table.entryStride_;
	const float* RESTRICT Values = Table.values_.Buffer();

	// First find the minimum and maximum values for each channel at each sample.
	for (Int32 EntryIndex = 0; EntryIndex < EntryCount; ++EntryIndex)
	{
		Mins.x_ = Min<float>(Mins.x_, Values[0]);
		Mins.y_ = Min<float>(Mins.y_, Values[1]);
		Mins.z_ = Min<float>(Mins.z_, Values[2]);
		Mins.w_ = Min<float>(Mins.w_, Values[3]);

		Maxs.x_ = Max<float>(Maxs.x_, Values[0]);
		Maxs.y_ = Max<float>(Maxs.y_, Values[1]);
		Maxs.z_ = Max<float>(Maxs.z_, Values[2]);
		Maxs.w_ = Max<float>(Maxs.w_, Values[3]);

		Values += EntryStride;
	}

	// Compute scale and bias.
	const Vector4 Scale(Maxs - Mins);
	const Vector4 InvScale(
		(Scale.x_ > KINDA_SMALL_NUMBER ? (1.0f / Scale.x_) : 0.0f) * 255.0f,
		(Scale.y_ > KINDA_SMALL_NUMBER ? (1.0f / Scale.y_) : 0.0f) * 255.0f,
		(Scale.z_ > KINDA_SMALL_NUMBER ? (1.0f / Scale.z_) : 0.0f) * 255.0f,
		(Scale.w_ > KINDA_SMALL_NUMBER ? (1.0f / Scale.w_) : 0.0f) * 255.0f
	);
	const Vector4 Bias(Mins);

	// If there is only one entry in the table, we don't need any samples at all.
	if (EntryCount == 1)
	{
		OutScale = Scale;
		OutBias = Bias;
		return;
	}

	// Now construct the quantized samples.
	OutQuantizedSamples.Resize(EntryCount);
	Color* RESTRICT QuantizedValues = &OutQuantizedSamples[0];
	Values = Table.values_.Buffer();
	for (Int32 EntryIndex = 0; EntryIndex < EntryCount; ++EntryIndex)
	{
		QuantizedValues->r_ = Clamp<Int32>(FloorToInt((Values[0] - Bias.x_) * InvScale.x_), 0, 255) / 255.f;
		QuantizedValues->g_ = Clamp<Int32>(FloorToInt((Values[1] - Bias.y_) * InvScale.y_), 0, 255) / 255.f;
		QuantizedValues->b_ = Clamp<Int32>(FloorToInt((Values[2] - Bias.z_) * InvScale.z_), 0, 255) / 255.f;
		QuantizedValues->a_ = Clamp<Int32>(FloorToInt((Values[3] - Bias.w_) * InvScale.w_), 0, 255) / 255.f;
		Values += EntryStride;
		QuantizedValues++;
	}
	OutScale = Scale;
	OutBias = Bias;
}

ComposableFloatDistribution::ComposableFloatDistribution()
{
	BuildZeroLookupTable(&lookupTable_, 1);
}

void ComposableFloatDistribution::Initialize(const DistributionFloat* FloatDistribution)
{
	checkDistribution(lookupTable_.GetValuesPerEntry() == 1);
	if (FloatDistribution != NULL && FloatDistribution->CanBeBaked())
	{
		BuildLookupTable(&lookupTable_, FloatDistribution);
	}
	else
	{
		BuildZeroLookupTable(&lookupTable_, 1);
	}
	checkDistribution(lookupTable_.GetValuesPerEntry() == 1);
}

void ComposableFloatDistribution::InitializeWithConstant(float Value)
{
	checkDistribution(lookupTable_.GetValuesPerEntry() == 1);
	BuildConstantLookupTable(&lookupTable_, 1, &Value);
	checkDistribution(lookupTable_.GetValuesPerEntry() == 1);
}

void ComposableFloatDistribution::ScaleByConstant(float Scale)
{
	checkDistribution(lookupTable_.GetValuesPerEntry() == 1);
	ScaleLookupTableByConstant(&lookupTable_, Scale);
	checkDistribution(lookupTable_.GetValuesPerEntry() == 1);
}

void ComposableFloatDistribution::ScaleByDistribution(const DistributionFloat* FloatDistribution)
{
	checkDistribution(lookupTable_.GetValuesPerEntry() == 1);
	if (FloatDistribution && FloatDistribution->CanBeBaked())
	{
		DistributionLookupTable TableToScaleBy;
		BuildLookupTable(&TableToScaleBy, FloatDistribution);
		ScaleLookupTableByLookupTable(&lookupTable_, TableToScaleBy);
	}
	else
	{
		BuildZeroLookupTable(&lookupTable_, 1);
	}
	checkDistribution(lookupTable_.GetValuesPerEntry() == 1);
}

void ComposableFloatDistribution::AddDistribution(const DistributionFloat* FloatDistribution)
{
	checkDistribution(lookupTable_.GetValuesPerEntry() == 1);
	if (FloatDistribution && FloatDistribution->CanBeBaked())
	{
		DistributionLookupTable TableToAdd;
		BuildLookupTable(&TableToAdd, FloatDistribution);
		AddLookupTableToLookupTable(&lookupTable_, TableToAdd);
	}
	checkDistribution(lookupTable_.GetValuesPerEntry() == 1);
}

void ComposableFloatDistribution::Normalize(float* OutScale, float* OutBias)
{
	float MinValue, MaxValue;
	float InvScale, InvBias;

	lookupTable_.GetRange(&MinValue, &MaxValue);
	*OutBias = MinValue;
	InvBias = -MinValue;
	*OutScale = MaxValue - MinValue;
	InvScale = (Abs(MaxValue - MinValue) > SMALL_NUMBER) ? (1.0f / (MaxValue - MinValue)) : 1.0f;

	AddConstantToLookupTable(&lookupTable_, &InvBias, 1);
	ScaleLookupTableByConstant(&lookupTable_, InvScale);
}

void ComposableFloatDistribution::Resample(float MinIn, float MaxIn)
{
	DistributionLookupTable OldTable = lookupTable_;
	ResampleLookupTable(&lookupTable_, OldTable, MinIn, MaxIn, LOOKUP_TABLE_MAX_SAMPLES);
}


ComposableVectorDistribution::ComposableVectorDistribution()
{
	const float Zero[3] = { 0 };
	BuildZeroLookupTable(&lookupTable_, 3);
}

void ComposableVectorDistribution::Initialize(const DistributionVector* VectorDistribution)
{
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
	if (VectorDistribution != NULL && VectorDistribution->CanBeBaked())
	{
		BuildLookupTable(&lookupTable_, VectorDistribution);
	}
	else
	{
		BuildZeroLookupTable(&lookupTable_, 3);
	}
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
}

void ComposableVectorDistribution::InitializeWithConstant(const Vector3& Value)
{
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
	Vector3 AsFloat = (Vector3)Value;
	BuildConstantLookupTable(&lookupTable_, 3, (float*)&AsFloat);
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
}

void ComposableVectorDistribution::ScaleByConstant(float Scale)
{
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
	ScaleLookupTableByConstant(&lookupTable_, Scale);
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
}

void ComposableVectorDistribution::ScaleByConstantVector(const Vector3& Scale)
{
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
	Vector3 AsFloat = (Vector3)Scale;
	ScaleLookupTableByConstants(&lookupTable_, (const float*)&AsFloat, 3);
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
}

void ComposableVectorDistribution::AddConstantVector(const Vector3& Value)
{
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
	Vector3 AsFloat = (Vector3)Value;
	AddConstantToLookupTable(&lookupTable_, (const float*)&AsFloat, 3);
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
}

void ComposableVectorDistribution::ScaleByDistribution(const DistributionFloat* FloatDistribution)
{
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
	if (FloatDistribution && FloatDistribution->CanBeBaked())
	{
		DistributionLookupTable TableToScaleBy;
		BuildLookupTable(&TableToScaleBy, FloatDistribution);
		ScaleLookupTableByLookupTable(&lookupTable_, TableToScaleBy);
	}
	else
	{
		BuildZeroLookupTable(&lookupTable_, 3);
	}
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
}

void ComposableVectorDistribution::ScaleByVectorDistribution(const DistributionVector* VectorDistribution)
{
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
	if (VectorDistribution && VectorDistribution->CanBeBaked())
	{
		DistributionLookupTable TableToScaleBy;
		BuildLookupTable(&TableToScaleBy, VectorDistribution);
		ScaleLookupTableByLookupTable(&lookupTable_, TableToScaleBy);
	}
	else
	{
		BuildZeroLookupTable(&lookupTable_, 3);
	}
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
}

void ComposableVectorDistribution::AddDistribution(const DistributionVector* VectorDistribution)
{
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
	if (VectorDistribution && VectorDistribution->CanBeBaked())
	{
		DistributionLookupTable TableToAdd;
		BuildLookupTable(&TableToAdd, VectorDistribution);
		AddLookupTableToLookupTable(&lookupTable_, TableToAdd);
	}
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
}

void ComposableVectorDistribution::Splat(Int32 ChannelIndex)
{
	ASSERT(ChannelIndex >= 0 && ChannelIndex <= 3);
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);

	const Int32 ValueCount = lookupTable_.values_.Size();
	for (Int32 Index = 0; Index < ValueCount; Index += 3)
	{
		float* Entry = &lookupTable_.values_[Index];
		const float ValueToSplat = Entry[ChannelIndex];
		for (Int32 ValueIndex = 0; ValueIndex < 3; ++ValueIndex)
		{
			Entry[ValueIndex] = ValueToSplat;
		}
	}
	checkDistribution(lookupTable_.GetValuesPerEntry() == 3);
}

void ComposableVectorDistribution::Resample(float MinIn, float MaxIn)
{
	DistributionLookupTable OldTable = lookupTable_;
	ResampleLookupTable(&lookupTable_, OldTable, MinIn, MaxIn, LOOKUP_TABLE_MAX_SAMPLES);
}

float DistributionFloatConstant::GetValue(float F, Object* Data, struct RandomStream* InRandomStream) const
{
	return constant_;
}


Int32 DistributionFloatConstant::GetNumKeys() const
{
	return 1;
}

Int32 DistributionFloatConstant::GetNumSubCurves() const
{
	return 1;
}

float DistributionFloatConstant::GetKeyIn(Int32 KeyIndex)
{
	ASSERT(KeyIndex == 0);
	return 0.f;
}

float DistributionFloatConstant::GetKeyOut(Int32 SubIndex, Int32 KeyIndex)
{
	ASSERT(SubIndex == 0);
	ASSERT(KeyIndex == 0);
	return constant_;
}

Color DistributionFloatConstant::GetKeyColor(Int32 SubIndex, Int32 KeyIndex, const Color& CurveColor)
{
	// There can be only be one sub-curve for this distribution.
	ASSERT(SubIndex == 0);
	// There can be only be one key for this distribution.
	ASSERT(KeyIndex == 0);

	// Always return RED since there is only one key
	return Color::RED;
}

void DistributionFloatConstant::GetInRange(float& MinIn, float& MaxIn) const
{
	MinIn = 0.f;
	MaxIn = 0.f;
}

void DistributionFloatConstant::GetOutRange(float& MinOut, float& MaxOut) const
{
	MinOut = constant_;
	MaxOut = constant_;
}

InterpCurveMode DistributionFloatConstant::GetKeyInterpMode(Int32 KeyIndex) const
{
	ASSERT(KeyIndex == 0);
	return CIM_Constant;
}

void DistributionFloatConstant::GetTangents(Int32 SubIndex, Int32 KeyIndex, float& ArriveTangent, float& LeaveTangent) const
{
	ASSERT(SubIndex == 0);
	ASSERT(KeyIndex == 0);
	ArriveTangent = 0.f;
	LeaveTangent = 0.f;
}

float DistributionFloatConstant::EvalSub(Int32 SubIndex, float InVal)
{
	ASSERT(SubIndex == 0);
	return constant_;
}

Int32 DistributionFloatConstant::CreateNewKey(float KeyIn)
{
	return 0;
}

void DistributionFloatConstant::DeleteKey(Int32 KeyIndex)
{
	ASSERT(KeyIndex == 0);
}

Int32 DistributionFloatConstant::SetKeyIn(Int32 KeyIndex, float NewInVal)
{
	ASSERT(KeyIndex == 0);
	return 0;
}

void DistributionFloatConstant::SetKeyOut(Int32 SubIndex, Int32 KeyIndex, float NewOutVal)
{
	ASSERT(SubIndex == 0);
	ASSERT(KeyIndex == 0);
	constant_ = NewOutVal;

	isDirty_ = true;
}

void DistributionFloatConstant::SetKeyInterpMode(Int32 KeyIndex, InterpCurveMode NewMode)
{
	ASSERT(KeyIndex == 0);
}

void DistributionFloatConstant::SetTangents(Int32 SubIndex, Int32 KeyIndex, float ArriveTangent, float LeaveTangent)
{
	ASSERT(SubIndex == 0);
	ASSERT(KeyIndex == 0);
}

float DistributionFloatConstantCurve::GetValue(float F, Object* Data, struct RandomStream* InRandomStream) const
{
	return constantCurve_.Eval(F, 0.f);
}


Int32 DistributionFloatConstantCurve::GetNumKeys() const
{
	return constantCurve_.points_.Size();
}

Int32 DistributionFloatConstantCurve::GetNumSubCurves() const
{
	return 1;
}

float DistributionFloatConstantCurve::GetKeyIn(Int32 KeyIndex)
{
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());
	return constantCurve_.points_[KeyIndex].inVal_;
}

float DistributionFloatConstantCurve::GetKeyOut(Int32 SubIndex, Int32 KeyIndex)
{
	ASSERT(SubIndex == 0);
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());
	return constantCurve_.points_[KeyIndex].outVal_;
}

Color DistributionFloatConstantCurve::GetKeyColor(Int32 SubIndex, Int32 KeyIndex, const Color& CurveColor)
{
	// There can be only be one sub-curve for this distribution.
	ASSERT(SubIndex == 0);
	// There can be only be one key for this distribution.
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());

	// Always return RED since there is only one sub-curve.
	return Color::RED;
}

void DistributionFloatConstantCurve::GetInRange(float& MinIn, float& MaxIn) const
{
	if (constantCurve_.points_.Size() == 0)
	{
		MinIn = 0.f;
		MaxIn = 0.f;
	}
	else
	{
		float Min = BIG_NUMBER;
		float Max = -BIG_NUMBER;
		for (Int32 Index = 0; Index < constantCurve_.points_.Size(); Index++)
		{
			float Value = constantCurve_.points_[Index].inVal_;
			if (Value < Min)
			{
				Min = Value;
			}
			if (Value > Max)
			{
				Max = Value;
			}
		}
		MinIn = Min;
		MaxIn = Max;
	}
}

void DistributionFloatConstantCurve::GetOutRange(float& MinOut, float& MaxOut) const
{
	constantCurve_.CalcBounds(MinOut, MaxOut, 0.f);
}

InterpCurveMode DistributionFloatConstantCurve::GetKeyInterpMode(Int32 KeyIndex) const
{
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());
	return constantCurve_.points_[KeyIndex].interpMode_;
}

void DistributionFloatConstantCurve::GetTangents(Int32 SubIndex, Int32 KeyIndex, float& ArriveTangent, float& LeaveTangent) const
{
	ASSERT(SubIndex == 0);
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());
	ArriveTangent = constantCurve_.points_[KeyIndex].arriveTangent_;
	LeaveTangent = constantCurve_.points_[KeyIndex].leaveTangent_;
}

float DistributionFloatConstantCurve::EvalSub(Int32 SubIndex, float InVal)
{
	ASSERT(SubIndex == 0);
	return constantCurve_.Eval(InVal, 0.f);
}

Int32 DistributionFloatConstantCurve::CreateNewKey(float KeyIn)
{
	float NewKeyOut = constantCurve_.Eval(KeyIn, 0.f);
	Int32 NewPointIndex = constantCurve_.AddPoint(KeyIn, NewKeyOut);
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;

	return NewPointIndex;
}

void DistributionFloatConstantCurve::DeleteKey(Int32 KeyIndex)
{
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());
	constantCurve_.points_.Erase(KeyIndex);
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;
}

Int32 DistributionFloatConstantCurve::SetKeyIn(Int32 KeyIndex, float NewInVal)
{
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());
	Int32 NewPointIndex = constantCurve_.MovePoint(KeyIndex, NewInVal);
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;

	return NewPointIndex;
}

void DistributionFloatConstantCurve::SetKeyOut(Int32 SubIndex, Int32 KeyIndex, float NewOutVal)
{
	ASSERT(SubIndex == 0);
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());
	constantCurve_.points_[KeyIndex].outVal_ = NewOutVal;
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;
}

void DistributionFloatConstantCurve::SetKeyInterpMode(Int32 KeyIndex, InterpCurveMode NewMode)
{
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());
	constantCurve_.points_[KeyIndex].interpMode_ = NewMode;
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;
}

void DistributionFloatConstantCurve::SetTangents(Int32 SubIndex, Int32 KeyIndex, float ArriveTangent, float LeaveTangent)
{
	ASSERT(SubIndex == 0);
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());
	constantCurve_.points_[KeyIndex].arriveTangent_ = ArriveTangent;
	constantCurve_.points_[KeyIndex].leaveTangent_ = LeaveTangent;

	isDirty_ = true;
}

float DistributionFloatUniform::GetValue(float F, Object* Data, struct RandomStream* InRandomStream) const
{
	return max_ + (min_ - max_) * DIST_GET_RANDOM_VALUE(InRandomStream);
}

//@todo.CONSOLE
RawDistributionOperation DistributionFloatUniform::GetOperation() const
{
	if (min_ == max_)
	{
		// This may as well be a constant - don't bother doing the SRand scaling on it.
		return RDO_None;
	}
	return RDO_Random;
}

UInt32 DistributionFloatUniform::InitializeRawEntry(float Time, float* Values) const
{
	Values[0] = min_;
	Values[1] = max_;
	return 2;
}

Int32 DistributionFloatUniform::GetNumKeys() const
{
	return 1;
}

Int32 DistributionFloatUniform::GetNumSubCurves() const
{
	return 2;
}

Color DistributionFloatUniform::GetSubCurveButtonColor(Int32 SubCurveIndex, bool bIsSubCurveHidden) const
{
	// Check for array out of bounds because it will crash the program
	ASSERT(SubCurveIndex >= 0);
	ASSERT(SubCurveIndex < GetNumSubCurves());

	Color ButtonColor;

	switch (SubCurveIndex)
	{
	case 0:
		// Red
		ButtonColor = bIsSubCurveHidden ? Color(32, 0, 0) : Color::RED;
		break;
	case 1:
		// Green
		ButtonColor = bIsSubCurveHidden ? Color(0, 32, 0) : Color::GREEN;
		break;
	default:
		// A bad sub-curve index was given. 
		ASSERT(false);
		break;
	}

	return ButtonColor;
}

float DistributionFloatUniform::GetKeyIn(Int32 KeyIndex)
{
	ASSERT(KeyIndex == 0);
	return 0.f;
}

float DistributionFloatUniform::GetKeyOut(Int32 SubIndex, Int32 KeyIndex)
{
	ASSERT(SubIndex == 0 || SubIndex == 1);
	ASSERT(KeyIndex == 0);
	return (SubIndex == 0) ? min_ : max_;
}

Color DistributionFloatUniform::GetKeyColor(Int32 SubIndex, Int32 KeyIndex, const Color& CurveColor)
{
	// There can only be as many as two sub-curves for this distribution.
	ASSERT(SubIndex == 0 || SubIndex == 1);
	// There can be only be one key for this distribution.
	ASSERT(KeyIndex == 0);

	Color KeyColor;

	if (0 == SubIndex)
	{
		KeyColor = Color::RED;
	}
	else
	{
		KeyColor = Color::GREEN;
	}

	return KeyColor;
}

void DistributionFloatUniform::GetInRange(float& MinIn, float& MaxIn) const
{
	MinIn = 0.f;
	MaxIn = 0.f;
}

void DistributionFloatUniform::GetOutRange(float& MinOut, float& MaxOut) const
{
	MinOut = min_;
	MaxOut = max_;
}

InterpCurveMode DistributionFloatUniform::GetKeyInterpMode(Int32 KeyIndex) const
{
	ASSERT(KeyIndex == 0);
	return CIM_Constant;
}

void DistributionFloatUniform::GetTangents(Int32 SubIndex, Int32 KeyIndex, float& ArriveTangent, float& LeaveTangent) const
{
	ASSERT(SubIndex == 0 || SubIndex == 1);
	ASSERT(KeyIndex == 0);
	ArriveTangent = 0.f;
	LeaveTangent = 0.f;
}

float DistributionFloatUniform::EvalSub(Int32 SubIndex, float InVal)
{
	ASSERT(SubIndex == 0 || SubIndex == 1);
	return (SubIndex == 0) ? min_ : max_;
}

Int32 DistributionFloatUniform::CreateNewKey(float KeyIn)
{
	return 0;
}

void DistributionFloatUniform::DeleteKey(Int32 KeyIndex)
{
	ASSERT(KeyIndex == 0);
}

Int32 DistributionFloatUniform::SetKeyIn(Int32 KeyIndex, float NewInVal)
{
	ASSERT(KeyIndex == 0);
	return 0;
}

void DistributionFloatUniform::SetKeyOut(Int32 SubIndex, Int32 KeyIndex, float NewOutVal)
{
	ASSERT(SubIndex == 0 || SubIndex == 1);
	ASSERT(KeyIndex == 0);

	// We ensure that we can't move the Min past the Max.
	if (SubIndex == 0)
	{
		min_ = Min<float>(NewOutVal, max_);
	}
	else
	{
		max_ = Max<float>(NewOutVal, min_);
	}

	isDirty_ = true;
}

void DistributionFloatUniform::SetKeyInterpMode(Int32 KeyIndex, InterpCurveMode NewMode)
{
	ASSERT(KeyIndex == 0);
}

void DistributionFloatUniform::SetTangents(Int32 SubIndex, Int32 KeyIndex, float ArriveTangent, float LeaveTangent)
{
	ASSERT(SubIndex == 0 || SubIndex == 1);
	ASSERT(KeyIndex == 0);
}


float DistributionFloatUniformCurve::GetValue(float F, Object* Data, struct RandomStream* InRandomStream) const
{
	Vector2 Val = constantCurve_.Eval(F, Vector2(0.f, 0.f));
	return Val.x_ + (Val.y_ - Val.x_) * DIST_GET_RANDOM_VALUE(InRandomStream);
}

//@todo.CONSOLE
RawDistributionOperation DistributionFloatUniformCurve::GetOperation() const
{
	if (constantCurve_.points_.Size() == 1)
	{
		// Only a single point - so see if Min == Max
		const InterpCurvePoint<Vector2>& Value = constantCurve_.points_[0];
		if (Value.outVal_.x_ == Value.outVal_.y_)
		{
			// This may as well be a constant - don't bother doing the SRand scaling on it.
			return RDO_None;
		}
	}
	return RDO_Random;
}

UInt32 DistributionFloatUniformCurve::InitializeRawEntry(float Time, float* Values) const
{
	Vector2 MinMax = GetMinMaxValue(Time, NULL);
	Values[0] = MinMax.x_;
	Values[1] = MinMax.y_;
	return 2;
}
//#endif

Vector2 DistributionFloatUniformCurve::GetMinMaxValue(float F, Object* Data) const
{
	return constantCurve_.Eval(F, Vector2(0.f, 0.f));
}

Int32 DistributionFloatUniformCurve::GetNumKeys() const
{
	return constantCurve_.points_.Size();
}

Int32 DistributionFloatUniformCurve::GetNumSubCurves() const
{
	return 2;
}

Color DistributionFloatUniformCurve::GetSubCurveButtonColor(Int32 SubCurveIndex, bool bIsSubCurveHidden) const
{
	// Check for array out of bounds because it will crash the program
	ASSERT(SubCurveIndex >= 0);
	ASSERT(SubCurveIndex < GetNumSubCurves());

	Color ButtonColor;

	switch (SubCurveIndex)
	{
	case 0:
		// Red
		ButtonColor = bIsSubCurveHidden ? Color(32, 0, 0) : Color::RED;
		break;
	case 1:
		// Green
		ButtonColor = bIsSubCurveHidden ? Color(0, 32, 0) : Color::GREEN;
		break;
	default:
		// A bad sub-curve index was given. 
		ASSERT(false);
		break;
	}

	return ButtonColor;
}

float DistributionFloatUniformCurve::GetKeyIn(Int32 KeyIndex)
{
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));
	return constantCurve_.points_[KeyIndex].inVal_;
}

float DistributionFloatUniformCurve::GetKeyOut(Int32 SubIndex, Int32 KeyIndex)
{
	ASSERT((SubIndex >= 0) && (SubIndex < 2));
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));

	if (SubIndex == 0)
	{
		return constantCurve_.points_[KeyIndex].outVal_.x_;
	}
	else
	{
		return constantCurve_.points_[KeyIndex].outVal_.y_;
	}
}

Color DistributionFloatUniformCurve::GetKeyColor(Int32 SubIndex, Int32 KeyIndex, const Color& CurveColor)
{
	ASSERT((SubIndex >= 0) && (SubIndex < 2));
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));

	if (SubIndex == 0)
	{
		return Color::RED;
	}
	else
	{
		return Color::GREEN;
	}
}

void DistributionFloatUniformCurve::GetInRange(float& MinIn, float& MaxIn) const
{
	if (constantCurve_.points_.Size() == 0)
	{
		MinIn = 0.f;
		MaxIn = 0.f;
	}
	else
	{
		float Min = BIG_NUMBER;
		float Max = -BIG_NUMBER;
		for (Int32 Index = 0; Index < constantCurve_.points_.Size(); Index++)
		{
			float Value = constantCurve_.points_[Index].inVal_;
			if (Value < Min)
			{
				Min = Value;
			}
			if (Value > Max)
			{
				Max = Value;
			}
		}
		MinIn = Min;
		MaxIn = Max;
	}
}

void DistributionFloatUniformCurve::GetOutRange(float& MinOut, float& MaxOut) const
{
	Vector2 MinVec, MaxVec;
	constantCurve_.CalcBounds(MinVec, MaxVec, Vector2::ZERO);
	MinOut = Min(MinVec.x_, MinVec.y_);
	MaxOut = Max(MaxVec.x_, MaxVec.y_);
}

InterpCurveMode DistributionFloatUniformCurve::GetKeyInterpMode(Int32 KeyIndex) const
{
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));
	return constantCurve_.points_[KeyIndex].interpMode_;
}

void DistributionFloatUniformCurve::GetTangents(Int32 SubIndex, Int32 KeyIndex, float& ArriveTangent, float& LeaveTangent) const
{
	ASSERT((SubIndex >= 0) && (SubIndex < 2));
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));

	if (SubIndex == 0)
	{
		ArriveTangent = constantCurve_.points_[KeyIndex].arriveTangent_.x_;
		LeaveTangent = constantCurve_.points_[KeyIndex].leaveTangent_.x_;
	}
	else
	{
		ArriveTangent = constantCurve_.points_[KeyIndex].arriveTangent_.y_;
		LeaveTangent = constantCurve_.points_[KeyIndex].leaveTangent_.y_;
	}
}

float DistributionFloatUniformCurve::EvalSub(Int32 SubIndex, float InVal)
{
	ASSERT((SubIndex >= 0) && (SubIndex < 2));

	Vector2 OutVal = constantCurve_.Eval(InVal, Vector2::ZERO);

	if (SubIndex == 0)
	{
		return OutVal.x_;
	}
	else
	{
		return OutVal.y_;
	}
}

Int32 DistributionFloatUniformCurve::CreateNewKey(float KeyIn)
{
	Vector2 NewKeyVal = constantCurve_.Eval(KeyIn, Vector2::ZERO);
	Int32 NewPointIndex = constantCurve_.AddPoint(KeyIn, NewKeyVal);
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;

	return NewPointIndex;
}

void DistributionFloatUniformCurve::DeleteKey(Int32 KeyIndex)
{
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));
	constantCurve_.points_.Erase(KeyIndex);
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;
}

Int32 DistributionFloatUniformCurve::SetKeyIn(Int32 KeyIndex, float NewInVal)
{
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));
	Int32 NewPointIndex = constantCurve_.MovePoint(KeyIndex, NewInVal);
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;

	return NewPointIndex;
}

void DistributionFloatUniformCurve::SetKeyOut(Int32 SubIndex, Int32 KeyIndex, float NewOutVal)
{
	ASSERT((SubIndex >= 0) && (SubIndex < 2));
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));

	if (SubIndex == 0)
	{
		constantCurve_.points_[KeyIndex].outVal_.x_ = NewOutVal;
	}
	else
	{
		constantCurve_.points_[KeyIndex].outVal_.y_ = NewOutVal;
	}

	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;
}

void DistributionFloatUniformCurve::SetKeyInterpMode(Int32 KeyIndex, InterpCurveMode NewMode)
{
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));
	constantCurve_.points_[KeyIndex].interpMode_ = NewMode;
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;
}

void DistributionFloatUniformCurve::SetTangents(Int32 SubIndex, Int32 KeyIndex, float ArriveTangent, float LeaveTangent)
{
	ASSERT((SubIndex >= 0) && (SubIndex < 2));
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));

	if (SubIndex == 0)
	{
		constantCurve_.points_[KeyIndex].arriveTangent_.x_ = ArriveTangent;
		constantCurve_.points_[KeyIndex].leaveTangent_.x_ = LeaveTangent;
	}
	else
	{
		constantCurve_.points_[KeyIndex].arriveTangent_.y_ = ArriveTangent;
		constantCurve_.points_[KeyIndex].leaveTangent_.y_ = LeaveTangent;
	}

	isDirty_ = true;
}

Vector3 DistributionVectorConstant::GetValue(float F, Object* Data, Int32 Extreme, struct RandomStream* InRandomStream) const
{
	switch (lockedAxes_)
	{
	case EDVLF_XY:
		return Vector3(constant_.x_, constant_.x_, constant_.z_);
	case EDVLF_XZ:
		return Vector3(constant_.x_, constant_.y_, constant_.x_);
	case EDVLF_YZ:
		return Vector3(constant_.x_, constant_.y_, constant_.y_);
	case EDVLF_XYZ:
		return Vector3(constant_.x_, constant_.x_, constant_.x_);
	case EDVLF_None:
	default:
		return constant_;
	}
}

Int32 DistributionVectorConstant::GetNumKeys() const
{
	return 1;
}

Int32 DistributionVectorConstant::GetNumSubCurves() const
{
	switch (lockedAxes_)
	{
	case EDVLF_XY:
	case EDVLF_XZ:
	case EDVLF_YZ:
		return 2;
	case EDVLF_XYZ:
		return 1;
	}
	return 3;
}

Color DistributionVectorConstant::GetSubCurveButtonColor(Int32 SubCurveIndex, bool bIsSubCurveHidden) const
{
	// Check for array out of bounds because it will crash the program
	ASSERT(SubCurveIndex >= 0);
	ASSERT(SubCurveIndex < GetNumSubCurves());

	Color ButtonColor;

	switch (SubCurveIndex)
	{
	case 0:
		// Red
		ButtonColor = bIsSubCurveHidden ? Color(32, 0, 0) : Color::RED;
		break;
	case 1:
		// Green
		ButtonColor = bIsSubCurveHidden ? Color(0, 32, 0) : Color::GREEN;
		break;
	case 2:
		// Blue
		ButtonColor = bIsSubCurveHidden ? Color(0, 0, 32) : Color::BLUE;
		break;
	default:
		// A bad sub-curve index was given. 
		ASSERT(false);
		break;
	}

	return ButtonColor;
}

float DistributionVectorConstant::GetKeyIn(Int32 KeyIndex)
{
	ASSERT(KeyIndex == 0);
	return 0.f;
}

float DistributionVectorConstant::GetKeyOut(Int32 SubIndex, Int32 KeyIndex)
{
	ASSERT(SubIndex >= 0 && SubIndex < 3);
	ASSERT(KeyIndex == 0);

	if (SubIndex == 0)
	{
		return constant_.x_;
	}
	else
		if (SubIndex == 1)
		{
			if ((lockedAxes_ == EDVLF_XY) || (lockedAxes_ == EDVLF_XYZ))
			{
				return constant_.x_;
			}
			else
			{
				return constant_.y_;
			}
		}
		else
		{
			if ((lockedAxes_ == EDVLF_XZ) || (lockedAxes_ == EDVLF_XYZ))
			{
				return constant_.x_;
			}
			else
				if (lockedAxes_ == EDVLF_YZ)
				{
					return constant_.y_;
				}
				else
				{
					return constant_.z_;
				}
		}
}

Color DistributionVectorConstant::GetKeyColor(Int32 SubIndex, Int32 KeyIndex, const Color& CurveColor)
{
	ASSERT(SubIndex >= 0 && SubIndex < 3);
	ASSERT(KeyIndex == 0);

	if (SubIndex == 0)
	{
		return Color::RED;
	}
	else if (SubIndex == 1)
	{
		return Color::GREEN;
	}
	else
	{
		return Color::BLUE;
	}
}

void DistributionVectorConstant::GetInRange(float& MinIn, float& MaxIn) const
{
	MinIn = 0.f;
	MaxIn = 0.f;
}

void DistributionVectorConstant::GetOutRange(float& MinOut, float& MaxOut) const
{
	Vector3 Local;

	switch (lockedAxes_)
	{
	case EDVLF_XY:
		Local = Vector3(constant_.x_, constant_.x_, constant_.z_);
		break;
	case EDVLF_XZ:
		Local = Vector3(constant_.x_, constant_.y_, constant_.x_);
		break;
	case EDVLF_YZ:
		Local = Vector3(constant_.x_, constant_.y_, constant_.y_);
		break;
	case EDVLF_XYZ:
		Local = Vector3(constant_.x_, constant_.x_, constant_.x_);
		break;
	case EDVLF_None:
	default:
		Local = Vector3(constant_);
		break;
	}

	MinOut = Local.GetMin();
	MaxOut = Local.GetMax();
}

InterpCurveMode DistributionVectorConstant::GetKeyInterpMode(Int32 KeyIndex) const
{
	ASSERT(KeyIndex == 0);
	return CIM_Constant;
}

void DistributionVectorConstant::GetTangents(Int32 SubIndex, Int32 KeyIndex, float& ArriveTangent, float& LeaveTangent) const
{
	ASSERT(SubIndex >= 0 && SubIndex < 3);
	ASSERT(KeyIndex == 0);
	ArriveTangent = 0.f;
	LeaveTangent = 0.f;
}

float DistributionVectorConstant::EvalSub(Int32 SubIndex, float InVal)
{
	ASSERT(SubIndex >= 0 && SubIndex < 3);
	return GetKeyOut(SubIndex, 0);
}

Int32 DistributionVectorConstant::CreateNewKey(float KeyIn)
{
	return 0;
}

void DistributionVectorConstant::DeleteKey(Int32 KeyIndex)
{
	ASSERT(KeyIndex == 0);
}

Int32 DistributionVectorConstant::SetKeyIn(Int32 KeyIndex, float NewInVal)
{
	ASSERT(KeyIndex == 0);
	return 0;
}

void DistributionVectorConstant::SetKeyOut(Int32 SubIndex, Int32 KeyIndex, float NewOutVal)
{
	ASSERT(SubIndex >= 0 && SubIndex < 3);
	ASSERT(KeyIndex == 0);

	if (SubIndex == 0)
		constant_.x_ = NewOutVal;
	else if (SubIndex == 1)
		constant_.y_ = NewOutVal;
	else if (SubIndex == 2)
		constant_.z_ = NewOutVal;

	isDirty_ = true;
}

void DistributionVectorConstant::SetKeyInterpMode(Int32 KeyIndex, InterpCurveMode NewMode)
{
	ASSERT(KeyIndex == 0);
}

void DistributionVectorConstant::SetTangents(Int32 SubIndex, Int32 KeyIndex, float ArriveTangent, float LeaveTangent)
{
	ASSERT(SubIndex >= 0 && SubIndex < 3);
	ASSERT(KeyIndex == 0);
}

void DistributionVectorConstant::GetRange(Vector3& OutMin, Vector3& OutMax) const
{
	OutMin = constant_;
	OutMax = constant_;
}

Vector3 DistributionVectorConstantCurve::GetValue(float F, Object* Data, Int32 Extreme, struct RandomStream* InRandomStream) const
{
	Vector3 Val = constantCurve_.Eval(F, Vector3::ZERO);
	switch (lockedAxes_)
	{
	case EDVLF_XY:
		return Vector3(Val.x_, Val.x_, Val.z_);
	case EDVLF_XZ:
		return Vector3(Val.x_, Val.y_, Val.x_);
	case EDVLF_YZ:
		return Vector3(Val.x_, Val.y_, Val.y_);
	case EDVLF_XYZ:
		return Vector3(Val.x_, Val.x_, Val.x_);
	case EDVLF_None:
	default:
		return Val;
	}
}

Int32 DistributionVectorConstantCurve::GetNumKeys() const
{
	return constantCurve_.points_.Size();
}

Int32 DistributionVectorConstantCurve::GetNumSubCurves() const
{
	switch (lockedAxes_)
	{
	case EDVLF_XY:
	case EDVLF_XZ:
	case EDVLF_YZ:
		return 2;
	case EDVLF_XYZ:
		return 1;
	}
	return 3;
}

Color DistributionVectorConstantCurve::GetSubCurveButtonColor(Int32 SubCurveIndex, bool bIsSubCurveHidden) const
{
	// Check for array out of bounds because it will crash the program
	ASSERT(SubCurveIndex >= 0);
	ASSERT(SubCurveIndex < GetNumSubCurves());

	Color ButtonColor;

	switch (SubCurveIndex)
	{
	case 0:
		// Red
		ButtonColor = bIsSubCurveHidden ? Color(32, 0, 0) : Color::RED;
		break;
	case 1:
		// Green
		ButtonColor = bIsSubCurveHidden ? Color(0, 32, 0) : Color::GREEN;
		break;
	case 2:
		// Blue
		ButtonColor = bIsSubCurveHidden ? Color(0, 0, 32) : Color::BLUE;
		break;
	default:
		// A bad sub-curve index was given. 
		ASSERT(false);
		break;
	}

	return ButtonColor;
}

float DistributionVectorConstantCurve::GetKeyIn(Int32 KeyIndex)
{
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());
	return constantCurve_.points_[KeyIndex].inVal_;
}

float DistributionVectorConstantCurve::GetKeyOut(Int32 SubIndex, Int32 KeyIndex)
{
	ASSERT(SubIndex >= 0 && SubIndex < 3);
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());

	if (SubIndex == 0)
	{
		return constantCurve_.points_[KeyIndex].outVal_.x_;
	}
	else
		if (SubIndex == 1)
		{
			if ((lockedAxes_ == EDVLF_XY) || (lockedAxes_ == EDVLF_XYZ))
			{
				return constantCurve_.points_[KeyIndex].outVal_.x_;
			}

			return constantCurve_.points_[KeyIndex].outVal_.y_;
		}
		else
		{
			if ((lockedAxes_ == EDVLF_XZ) || (lockedAxes_ == EDVLF_XYZ))
			{
				return constantCurve_.points_[KeyIndex].outVal_.x_;
			}
			else
				if (lockedAxes_ == EDVLF_YZ)
				{
					return constantCurve_.points_[KeyIndex].outVal_.y_;
				}

			return constantCurve_.points_[KeyIndex].outVal_.z_;
		}
}

Color DistributionVectorConstantCurve::GetKeyColor(Int32 SubIndex, Int32 KeyIndex, const Color& CurveColor)
{
	ASSERT(SubIndex >= 0 && SubIndex < 3);
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());

	if (SubIndex == 0)
	{
		return Color::RED;
	}
	else if (SubIndex == 1)
	{
		return Color::GREEN;
	}
	else
	{
		return Color::BLUE;
	}
}

void DistributionVectorConstantCurve::GetInRange(float& MinIn, float& MaxIn) const
{
	if (constantCurve_.points_.Size() == 0)
	{
		MinIn = 0.f;
		MaxIn = 0.f;
	}
	else
	{
		float Min = BIG_NUMBER;
		float Max = -BIG_NUMBER;
		for (Int32 Index = 0; Index < constantCurve_.points_.Size(); Index++)
		{
			float Value = constantCurve_.points_[Index].inVal_;
			if (Value < Min)
			{
				Min = Value;
			}
			if (Value > Max)
			{
				Max = Value;
			}
		}
		MinIn = Min;
		MaxIn = Max;
	}
}

void DistributionVectorConstantCurve::GetOutRange(float& MinOut, float& MaxOut) const
{
	Vector3 MinVec, MaxVec;
	constantCurve_.CalcBounds(MinVec, MaxVec, Vector3::ZERO);

	switch (lockedAxes_)
	{
	case EDVLF_XY:
		MinVec.y_ = MinVec.x_;
		MaxVec.y_ = MaxVec.x_;
		break;
	case EDVLF_XZ:
		MinVec.z_ = MinVec.x_;
		MaxVec.z_ = MaxVec.x_;
		break;
	case EDVLF_YZ:
		MinVec.z_ = MinVec.y_;
		MaxVec.z_ = MaxVec.y_;
		break;
	case EDVLF_XYZ:
		MinVec.y_ = MinVec.x_;
		MinVec.z_ = MinVec.x_;
		MaxVec.y_ = MaxVec.x_;
		MaxVec.z_ = MaxVec.x_;
		break;
	case EDVLF_None:
	default:
		break;
	}

	MinOut = MinVec.GetMin();
	MaxOut = MaxVec.GetMax();
}

InterpCurveMode DistributionVectorConstantCurve::GetKeyInterpMode(Int32 KeyIndex) const
{
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());
	return constantCurve_.points_[KeyIndex].interpMode_;
}

void DistributionVectorConstantCurve::GetTangents(Int32 SubIndex, Int32 KeyIndex, float& ArriveTangent, float& LeaveTangent) const
{
	ASSERT(SubIndex >= 0 && SubIndex < 3);
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());

	if (SubIndex == 0)
	{
		ArriveTangent = constantCurve_.points_[KeyIndex].arriveTangent_.x_;
		LeaveTangent = constantCurve_.points_[KeyIndex].leaveTangent_.x_;
	}
	else if (SubIndex == 1)
	{
		ArriveTangent = constantCurve_.points_[KeyIndex].arriveTangent_.y_;
		LeaveTangent = constantCurve_.points_[KeyIndex].leaveTangent_.y_;
	}
	else if (SubIndex == 2)
	{
		ArriveTangent = constantCurve_.points_[KeyIndex].arriveTangent_.z_;
		LeaveTangent = constantCurve_.points_[KeyIndex].leaveTangent_.z_;
	}
}

float DistributionVectorConstantCurve::EvalSub(Int32 SubIndex, float InVal)
{
	ASSERT(SubIndex >= 0 && SubIndex < 3);

	Vector3 OutVal = constantCurve_.Eval(InVal, Vector3::ZERO);

	if (SubIndex == 0)
		return OutVal.x_;
	else if (SubIndex == 1)
		return OutVal.y_;
	else
		return OutVal.z_;
}

Int32 DistributionVectorConstantCurve::CreateNewKey(float KeyIn)
{
	Vector3 NewKeyVal = constantCurve_.Eval(KeyIn, Vector3::ZERO);
	Int32 NewPointIndex = constantCurve_.AddPoint(KeyIn, NewKeyVal);
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;

	return NewPointIndex;
}

void DistributionVectorConstantCurve::DeleteKey(Int32 KeyIndex)
{
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());
	constantCurve_.points_.Erase(KeyIndex);
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;
}

Int32 DistributionVectorConstantCurve::SetKeyIn(Int32 KeyIndex, float NewInVal)
{
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());
	Int32 NewPointIndex = constantCurve_.MovePoint(KeyIndex, NewInVal);
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;

	return NewPointIndex;
}

void DistributionVectorConstantCurve::SetKeyOut(Int32 SubIndex, Int32 KeyIndex, float NewOutVal)
{
	ASSERT(SubIndex >= 0 && SubIndex < 3);
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());

	if (SubIndex == 0)
		constantCurve_.points_[KeyIndex].outVal_.x_ = NewOutVal;
	else if (SubIndex == 1)
		constantCurve_.points_[KeyIndex].outVal_.y_ = NewOutVal;
	else
		constantCurve_.points_[KeyIndex].outVal_.z_ = NewOutVal;

	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;
}

void DistributionVectorConstantCurve::SetKeyInterpMode(Int32 KeyIndex, InterpCurveMode NewMode)
{
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());

	constantCurve_.points_[KeyIndex].interpMode_ = NewMode;
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;
}

void DistributionVectorConstantCurve::SetTangents(Int32 SubIndex, Int32 KeyIndex, float ArriveTangent, float LeaveTangent)
{
	ASSERT(SubIndex >= 0 && SubIndex < 3);
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());

	if (SubIndex == 0)
	{
		constantCurve_.points_[KeyIndex].arriveTangent_.x_ = ArriveTangent;
		constantCurve_.points_[KeyIndex].leaveTangent_.x_ = LeaveTangent;
	}
	else if (SubIndex == 1)
	{
		constantCurve_.points_[KeyIndex].arriveTangent_.y_ = ArriveTangent;
		constantCurve_.points_[KeyIndex].leaveTangent_.y_ = LeaveTangent;
	}
	else if (SubIndex == 2)
	{
		constantCurve_.points_[KeyIndex].arriveTangent_.z_ = ArriveTangent;
		constantCurve_.points_[KeyIndex].leaveTangent_.z_ = LeaveTangent;
	}

	isDirty_ = true;
}

// DistributionVector interface
void DistributionVectorConstantCurve::GetRange(Vector3& OutMin, Vector3& OutMax) const
{
	Vector3 MinVec, MaxVec;
	constantCurve_.CalcBounds(MinVec, MaxVec, Vector3::ZERO);

	switch (lockedAxes_)
	{
	case EDVLF_XY:
		MinVec.y_ = MinVec.x_;
		MaxVec.y_ = MaxVec.x_;
		break;
	case EDVLF_XZ:
		MinVec.z_ = MinVec.x_;
		MaxVec.z_ = MaxVec.x_;
		break;
	case EDVLF_YZ:
		MinVec.z_ = MinVec.y_;
		MaxVec.z_ = MaxVec.y_;
		break;
	case EDVLF_XYZ:
		MinVec.y_ = MinVec.x_;
		MinVec.z_ = MinVec.x_;
		MaxVec.y_ = MaxVec.x_;
		MaxVec.z_ = MaxVec.x_;
		break;
	case EDVLF_None:
	default:
		break;
	}

	OutMin = MinVec;
	OutMax = MaxVec;
}

DistributionVectorUniform::DistributionVectorUniform()
{
	mirrorFlags_[0] = EDVMF_Different;
	mirrorFlags_[1] = EDVMF_Different;
	mirrorFlags_[2] = EDVMF_Different;
	useExtremes_ = false;
}

Vector3 DistributionVectorUniform::GetValue(float F, Object* Data, Int32 Extreme, struct RandomStream* InRandomStream) const
{
	Vector3 LocalMax = max_;
	Vector3 LocalMin = min_;

	LocalMin.x_ = (mirrorFlags_[0] == EDVMF_Different) ? LocalMin.x_ : ((mirrorFlags_[0] == EDVMF_Mirror) ? -LocalMax.x_ : LocalMax.x_);
	LocalMin.y_ = (mirrorFlags_[1] == EDVMF_Different) ? LocalMin.y_ : ((mirrorFlags_[1] == EDVMF_Mirror) ? -LocalMax.y_ : LocalMax.y_);
	LocalMin.z_ = (mirrorFlags_[2] == EDVMF_Different) ? LocalMin.z_ : ((mirrorFlags_[2] == EDVMF_Mirror) ? -LocalMax.z_ : LocalMax.z_);

	float fX;
	float fY;
	float fZ;

	bool bMin = true;
	if (useExtremes_)
	{
		if (Extreme == 0)
		{
			if (DIST_GET_RANDOM_VALUE(InRandomStream) > 0.5f)
			{
				bMin = false;
			}
		}
		else if (Extreme > 0)
		{
			bMin = false;
		}
	}

	switch (lockedAxes_)
	{
	case EDVLF_XY:
		if (useExtremes_)
		{
			if (bMin)
			{
				fX = LocalMin.x_;
				fZ = LocalMin.z_;
			}
			else
			{
				fX = LocalMax.x_;
				fZ = LocalMax.z_;
			}
		}
		else
		{
			fX = LocalMax.x_ + (LocalMin.x_ - LocalMax.x_) * DIST_GET_RANDOM_VALUE(InRandomStream);
			fZ = LocalMax.z_ + (LocalMin.z_ - LocalMax.z_) * DIST_GET_RANDOM_VALUE(InRandomStream);
		}
		fY = fX;
		break;
	case EDVLF_XZ:
		if (useExtremes_)
		{
			if (bMin)
			{
				fX = LocalMin.x_;
				fY = LocalMin.y_;
			}
			else
			{
				fX = LocalMax.x_;
				fY = LocalMax.y_;
			}
		}
		else
		{
			fX = LocalMax.x_ + (LocalMin.x_ - LocalMax.x_) * DIST_GET_RANDOM_VALUE(InRandomStream);
			fY = LocalMax.y_ + (LocalMin.y_ - LocalMax.y_) * DIST_GET_RANDOM_VALUE(InRandomStream);
		}
		fZ = fX;
		break;
	case EDVLF_YZ:
		if (useExtremes_)
		{
			if (bMin)
			{
				fX = LocalMin.x_;
				fY = LocalMin.y_;
			}
			else
			{
				fX = LocalMax.x_;
				fY = LocalMax.y_;
			}
		}
		else
		{
			fX = LocalMax.x_ + (LocalMin.x_ - LocalMax.x_) * DIST_GET_RANDOM_VALUE(InRandomStream);
			fY = LocalMax.y_ + (LocalMin.y_ - LocalMax.y_) * DIST_GET_RANDOM_VALUE(InRandomStream);
		}
		fZ = fY;
		break;
	case EDVLF_XYZ:
		if (useExtremes_)
		{
			if (bMin)
			{
				fX = LocalMin.x_;
			}
			else
			{
				fX = LocalMax.x_;
			}
		}
		else
		{
			fX = LocalMax.x_ + (LocalMin.x_ - LocalMax.x_) * DIST_GET_RANDOM_VALUE(InRandomStream);
		}
		fY = fX;
		fZ = fX;
		break;
	case EDVLF_None:
	default:
		if (useExtremes_)
		{
			if (bMin)
			{
				fX = LocalMin.x_;
				fY = LocalMin.y_;
				fZ = LocalMin.z_;
			}
			else
			{
				fX = LocalMax.x_;
				fY = LocalMax.y_;
				fZ = LocalMax.z_;
			}
		}
		else
		{
			fX = LocalMax.x_ + (LocalMin.x_ - LocalMax.x_) * DIST_GET_RANDOM_VALUE(InRandomStream);
			fY = LocalMax.y_ + (LocalMin.y_ - LocalMax.y_) * DIST_GET_RANDOM_VALUE(InRandomStream);
			fZ = LocalMax.z_ + (LocalMin.z_ - LocalMax.z_) * DIST_GET_RANDOM_VALUE(InRandomStream);
		}
		break;
	}

	return Vector3(fX, fY, fZ);
}

//@todo.CONSOLE
RawDistributionOperation DistributionVectorUniform::GetOperation() const
{
	if (min_ == max_)
	{
		// This may as well be a constant - don't bother doing the SRand scaling on it.
		return RDO_None;
	}
	// override the operation to use
	return useExtremes_ ? RDO_Extreme : RDO_Random;
}

UInt8 DistributionVectorUniform::GetLockFlag() const
{
	return (UInt8)lockedAxes_;
}

UInt32 DistributionVectorUniform::InitializeRawEntry(float Time, float* Values) const
{
	// get the locked/mirrored min and max
	Vector3 ValueMin = GetMinValue();
	Vector3 ValueMax = GetMaxValue();
	Values[0] = ValueMin.x_;
	Values[1] = ValueMin.y_;
	Values[2] = ValueMin.z_;
	Values[3] = ValueMax.x_;
	Values[4] = ValueMax.y_;
	Values[5] = ValueMax.z_;

	// six elements per value
	return 6;
}

//#endif

Vector3 DistributionVectorUniform::GetMinValue() const
{
	Vector3 LocalMax = max_;
	Vector3 LocalMin = min_;

	for (Int32 i = 0; i < 3; i++)
	{
		switch (mirrorFlags_[i])
		{
		case EDVMF_Same:	LocalMin[i] = LocalMax[i];		break;
		case EDVMF_Mirror:	LocalMin[i] = -LocalMax[i];		break;
		}
	}

	float fX;
	float fY;
	float fZ;

	switch (lockedAxes_)
	{
	case EDVLF_XY:
		fX = LocalMin.x_;
		fY = LocalMin.x_;
		fZ = LocalMin.z_;
		break;
	case EDVLF_XZ:
		fX = LocalMin.x_;
		fY = LocalMin.y_;
		fZ = fX;
		break;
	case EDVLF_YZ:
		fX = LocalMin.x_;
		fY = LocalMin.y_;
		fZ = fY;
		break;
	case EDVLF_XYZ:
		fX = LocalMin.x_;
		fY = fX;
		fZ = fX;
		break;
	case EDVLF_None:
	default:
		fX = LocalMin.x_;
		fY = LocalMin.y_;
		fZ = LocalMin.z_;
		break;
	}

	return Vector3(fX, fY, fZ);
}

Vector3 DistributionVectorUniform::GetMaxValue() const
{
	Vector3 LocalMax = max_;

	float fX;
	float fY;
	float fZ;

	switch (lockedAxes_)
	{
	case EDVLF_XY:
		fX = LocalMax.x_;
		fY = LocalMax.x_;
		fZ = LocalMax.z_;
		break;
	case EDVLF_XZ:
		fX = LocalMax.x_;
		fY = LocalMax.y_;
		fZ = fX;
		break;
	case EDVLF_YZ:
		fX = LocalMax.x_;
		fY = LocalMax.y_;
		fZ = fY;
		break;
	case EDVLF_XYZ:
		fX = LocalMax.x_;
		fY = fX;
		fZ = fX;
		break;
	case EDVLF_None:
	default:
		fX = LocalMax.x_;
		fY = LocalMax.y_;
		fZ = LocalMax.z_;
		break;
	}

	return Vector3(fX, fY, fZ);
}

Int32 DistributionVectorUniform::GetNumKeys() const
{
	return 1;
}

Int32 DistributionVectorUniform::GetNumSubCurves() const
{
	switch (lockedAxes_)
	{
	case EDVLF_XY:
	case EDVLF_XZ:
	case EDVLF_YZ:
		return 4;
	case EDVLF_XYZ:
		return 2;
	}
	return 6;
}


Color DistributionVectorUniform::GetSubCurveButtonColor(Int32 SubCurveIndex, bool bIsSubCurveHidden) const
{
	const Int32 SubCurves = GetNumSubCurves();

	// Check for array out of bounds because it will crash the program
	ASSERT(SubCurveIndex >= 0);
	ASSERT(SubCurveIndex < SubCurves);

	const bool bShouldGroupMinAndMax = ((SubCurves == 4) || (SubCurves == 6));
	Color ButtonColor;

	switch (SubCurveIndex)
	{
	case 0:
		// Red
		ButtonColor = bIsSubCurveHidden ? Color(32, 0, 0) : Color::RED;
		break;
	case 1:
		if (bShouldGroupMinAndMax)
		{
			// Dark red
			ButtonColor = bIsSubCurveHidden ? Color(28, 0, 0) : Color(196, 0, 0);
		}
		else
		{
			// Green
			ButtonColor = bIsSubCurveHidden ? Color(0, 32, 0) : Color::GREEN;
		}
		break;
	case 2:
		if (bShouldGroupMinAndMax)
		{
			// Green
			ButtonColor = bIsSubCurveHidden ? Color(0, 32, 0) : Color::GREEN;
		}
		else
		{
			// Blue
			ButtonColor = bIsSubCurveHidden ? Color(0, 0, 32) : Color::BLUE;
		}
		break;
	case 3:
		// Dark green
		ButtonColor = bIsSubCurveHidden ? Color(0, 28, 0) : Color(0, 196, 0);
		break;
	case 4:
		// Blue
		ButtonColor = bIsSubCurveHidden ? Color(0, 0, 32) : Color::BLUE;
		break;
	case 5:
		// Dark blue
		ButtonColor = bIsSubCurveHidden ? Color(0, 0, 28) : Color(0, 0, 196);
		break;
	default:
		// A bad sub-curve index was given. 
		ASSERT(false);
		break;
	}

	return ButtonColor;
}

float DistributionVectorUniform::GetKeyIn(Int32 KeyIndex)
{
	ASSERT(KeyIndex == 0);
	return 0.f;
}

float DistributionVectorUniform::GetKeyOut(Int32 SubIndex, Int32 KeyIndex)
{
	ASSERT(SubIndex >= 0 && SubIndex < 6);
	ASSERT(KeyIndex == 0);

	Vector3 LocalMax = max_;
	Vector3 LocalMin = min_;

	for (Int32 i = 0; i < 3; i++)
	{
		switch (mirrorFlags_[i])
		{
		case EDVMF_Same:	LocalMin[i] = LocalMax[i];		break;
		case EDVMF_Mirror:	LocalMin[i] = -LocalMax[i];		break;
		}
	}

	switch (lockedAxes_)
	{
	case EDVLF_XY:
		LocalMin.y_ = LocalMin.x_;
		break;
	case EDVLF_XZ:
		LocalMin.z_ = LocalMin.x_;
		break;
	case EDVLF_YZ:
		LocalMin.z_ = LocalMin.y_;
		break;
	case EDVLF_XYZ:
		LocalMin.y_ = LocalMin.x_;
		LocalMin.z_ = LocalMin.x_;
		break;
	case EDVLF_None:
	default:
		break;
	}

	switch (SubIndex)
	{
	case 0:		return LocalMin.x_;
	case 1:		return LocalMax.x_;
	case 2:		return LocalMin.y_;
	case 3:		return LocalMax.y_;
	case 4:		return LocalMin.z_;
	}
	return LocalMax.z_;
}

Color DistributionVectorUniform::GetKeyColor(Int32 SubIndex, Int32 KeyIndex, const Color& CurveColor)
{
	ASSERT(SubIndex >= 0 && SubIndex < 6);
	ASSERT(KeyIndex == 0);

	if (SubIndex == 0)
	{
		return Color(128, 0, 0);
	}
	else if (SubIndex == 1)
	{
		return Color::RED;
	}
	else if (SubIndex == 2)
	{
		return Color(0, 128, 0);
	}
	else if (SubIndex == 3)
	{
		return Color::GREEN;
	}
	else if (SubIndex == 4)
	{
		return Color(0, 0, 128);
	}
	else
	{
		return Color::BLUE;
	}
}

void DistributionVectorUniform::GetInRange(float& MinIn, float& MaxIn) const
{
	MinIn = 0.f;
	MaxIn = 0.f;
}

void DistributionVectorUniform::GetOutRange(float& MinOut, float& MaxOut) const
{
	Vector3 LocalMax = max_;
	Vector3 LocalMin = min_;

	for (Int32 i = 0; i < 3; i++)
	{
		switch (mirrorFlags_[i])
		{
		case EDVMF_Same:	LocalMin[i] = LocalMax[i];		break;
		case EDVMF_Mirror:	LocalMin[i] = -LocalMax[i];		break;
		}
	}

	Vector3 LocalMin2;
	Vector3 LocalMax2;

	switch (lockedAxes_)
	{
	case EDVLF_XY:
		LocalMin2 = Vector3(LocalMin.x_, LocalMin.x_, LocalMin.z_);
		LocalMax2 = Vector3(LocalMax.x_, LocalMax.x_, LocalMax.z_);
		break;
	case EDVLF_XZ:
		LocalMin2 = Vector3(LocalMin.x_, LocalMin.y_, LocalMin.x_);
		LocalMax2 = Vector3(LocalMax.x_, LocalMax.y_, LocalMax.x_);
		break;
	case EDVLF_YZ:
		LocalMin2 = Vector3(LocalMin.x_, LocalMin.y_, LocalMin.y_);
		LocalMax2 = Vector3(LocalMax.x_, LocalMax.y_, LocalMax.y_);
		break;
	case EDVLF_XYZ:
		LocalMin2 = Vector3(LocalMin.x_, LocalMin.x_, LocalMin.x_);
		LocalMax2 = Vector3(LocalMax.x_, LocalMin.x_, LocalMin.x_);
		break;
	case EDVLF_None:
	default:
		LocalMin2 = Vector3(LocalMin.x_, LocalMin.y_, LocalMin.z_);
		LocalMax2 = Vector3(LocalMax.x_, LocalMax.y_, LocalMax.z_);
		break;
	}

	MinOut = LocalMin2.GetMin();
	MaxOut = LocalMax2.GetMax();
}

InterpCurveMode DistributionVectorUniform::GetKeyInterpMode(Int32 KeyIndex) const
{
	ASSERT(KeyIndex == 0);
	return CIM_Constant;
}

void DistributionVectorUniform::GetTangents(Int32 SubIndex, Int32 KeyIndex, float& ArriveTangent, float& LeaveTangent) const
{
	ASSERT(SubIndex >= 0 && SubIndex < 6);
	ASSERT(KeyIndex == 0);
	ArriveTangent = 0.f;
	LeaveTangent = 0.f;
}

float DistributionVectorUniform::EvalSub(Int32 SubIndex, float InVal)
{
	return GetKeyOut(SubIndex, 0);
}

Int32 DistributionVectorUniform::CreateNewKey(float KeyIn)
{
	return 0;
}

void DistributionVectorUniform::DeleteKey(Int32 KeyIndex)
{
	ASSERT(KeyIndex == 0);
}

Int32 DistributionVectorUniform::SetKeyIn(Int32 KeyIndex, float NewInVal)
{
	ASSERT(KeyIndex == 0);
	return 0;
}

void DistributionVectorUniform::SetKeyOut(Int32 SubIndex, Int32 KeyIndex, float NewOutVal)
{
	ASSERT(SubIndex >= 0 && SubIndex < 6);
	ASSERT(KeyIndex == 0);

	if (SubIndex == 0)
		min_.x_ = Min<float>(NewOutVal, max_.x_);
	else if (SubIndex == 1)
		max_.x_ = Max<float>(NewOutVal, min_.x_);
	else if (SubIndex == 2)
		min_.y_ = Min<float>(NewOutVal, max_.y_);
	else if (SubIndex == 3)
		max_.y_ = Max<float>(NewOutVal, min_.y_);
	else if (SubIndex == 4)
		min_.z_ = Min<float>(NewOutVal, max_.z_);
	else
		max_.z_ = Max<float>(NewOutVal, min_.z_);

	isDirty_ = true;
}

void DistributionVectorUniform::SetKeyInterpMode(Int32 KeyIndex, InterpCurveMode NewMode)
{
	ASSERT(KeyIndex == 0);
}

void DistributionVectorUniform::SetTangents(Int32 SubIndex, Int32 KeyIndex, float ArriveTangent, float LeaveTangent)
{
	ASSERT(SubIndex >= 0 && SubIndex < 6);
	ASSERT(KeyIndex == 0);
}

// DistributionVector interface
void DistributionVectorUniform::GetRange(Vector3& OutMin, Vector3& OutMax) const
{
	OutMin = min_;
	OutMax = max_;
}


DistributionVectorUniformCurve::DistributionVectorUniformCurve()
{
	lockAxes1_ = false;
	lockAxes2_ = false;
	lockedAxes_[0] = EDVLF_None;
	lockedAxes_[1] = EDVLF_None;
	mirrorFlags_[0] = EDVMF_Different;
	mirrorFlags_[1] = EDVMF_Different;
	mirrorFlags_[2] = EDVMF_Different;
	useExtremes_ = false;
}

Vector3 DistributionVectorUniformCurve::GetValue(float F, Object* Data, Int32 Extreme, struct RandomStream* InRandomStream) const
{
	TwoVectors	Val = constantCurve_.Eval(F, TwoVectors());

	bool bMin = true;
	if (useExtremes_)
	{
		if (Extreme == 0)
		{
			if (DIST_GET_RANDOM_VALUE(InRandomStream) > 0.5f)
			{
				bMin = false;
			}
		}
		else if (Extreme < 0)
		{
			bMin = false;
		}
	}

	LockAndMirror(Val);
	if (useExtremes_)
	{
		return ((bMin == true) ? Vector3(Val.v2_.x_, Val.v2_.y_, Val.v2_.z_) : Vector3(Val.v1_.x_, Val.v1_.y_, Val.v1_.z_));
	}
	else
	{
		return Vector3(
			Val.v1_.x_ + (Val.v2_.x_ - Val.v1_.x_) * DIST_GET_RANDOM_VALUE(InRandomStream),
			Val.v1_.y_ + (Val.v2_.y_ - Val.v1_.y_) * DIST_GET_RANDOM_VALUE(InRandomStream),
			Val.v1_.z_ + (Val.v2_.z_ - Val.v1_.z_) * DIST_GET_RANDOM_VALUE(InRandomStream));
	}
}

//@todo.CONSOLE
RawDistributionOperation DistributionVectorUniformCurve::GetOperation() const
{
	if (constantCurve_.points_.Size() == 1)
	{
		// Only a single point - so see if Min == Max
		const InterpCurvePoint<TwoVectors>& Value = constantCurve_.points_[0];
		if (Value.outVal_.v1_ == Value.outVal_.v2_)
		{
			// This may as well be a constant - don't bother doing the SRand scaling on it.
			return RDO_None;
		}
	}
	return useExtremes_ ? RDO_Extreme : RDO_Random;
}


UInt32 DistributionVectorUniformCurve::InitializeRawEntry(float Time, float* Values) const
{
	// get the min and max values at the current time (just Eval the curve)
	TwoVectors MinMax = GetMinMaxValue(Time, NULL);
	// apply any axis locks and mirroring (in place)
	LockAndMirror(MinMax);

	// copy out the values
	Values[0] = MinMax.v1_.x_;
	Values[1] = MinMax.v1_.y_;
	Values[2] = MinMax.v1_.z_;
	Values[3] = MinMax.v2_.x_;
	Values[4] = MinMax.v2_.y_;
	Values[5] = MinMax.v2_.z_;

	// we wrote size elements
	return 6;
}

//#endif


TwoVectors DistributionVectorUniformCurve::GetMinMaxValue(float F, Object* Data) const
{
	return constantCurve_.Eval(F, TwoVectors());
}

Vector3 DistributionVectorUniformCurve::GetMinValue() const
{
	ASSERT(!"Don't call me!");
	return Vector3::ZERO;
}

// PVS-Studio notices that the implementation of GetMinValue is identical to this one
// and warns us. In this case, it is intentional, so we disable the warning:
Vector3 DistributionVectorUniformCurve::GetMaxValue() const //-V524
{
	ASSERT(!"Don't call me!");
	return Vector3::ZERO;
}

Int32 DistributionVectorUniformCurve::GetNumKeys() const
{
	return constantCurve_.points_.Size();
}

Int32 DistributionVectorUniformCurve::GetNumSubCurves() const
{
	Int32 Count = 0;
	/***
		switch (lockedAxes_[0])
		{
		case EDVLF_XY:	Count += 2;	break;
		case EDVLF_XZ:	Count += 2;	break;
		case EDVLF_YZ:	Count += 2;	break;
		case EDVLF_XYZ:	Count += 1;	break;
		default:		Count += 3;	break;
		}

		switch (lockedAxes_[1])
		{
		case EDVLF_XY:	Count += 2;	break;
		case EDVLF_XZ:	Count += 2;	break;
		case EDVLF_YZ:	Count += 2;	break;
		case EDVLF_XYZ:	Count += 1;	break;
		default:		Count += 3;	break;
		}
	***/
	Count = 6;
	return Count;
}

Color DistributionVectorUniformCurve::GetSubCurveButtonColor(Int32 SubCurveIndex, bool bIsSubCurveHidden) const
{
	const Int32 SubCurves = GetNumSubCurves();

	// Check for array out of bounds because it will crash the program
	ASSERT(SubCurveIndex >= 0);
	ASSERT(SubCurveIndex < SubCurves);

	const bool bShouldGroupMinAndMax = ((SubCurves == 4) || (SubCurves == 6));
	Color ButtonColor;

	switch (SubCurveIndex)
	{
	case 0:
		// Red
		ButtonColor = bIsSubCurveHidden ? Color(32, 0, 0) : Color::RED;
		break;
	case 1:
		if (bShouldGroupMinAndMax)
		{
			// Dark red
			ButtonColor = bIsSubCurveHidden ? Color(28, 0, 0) : Color(196, 0, 0);
		}
		else
		{
			// Green
			ButtonColor = bIsSubCurveHidden ? Color(0, 32, 0) : Color::GREEN;
		}
		break;
	case 2:
		if (bShouldGroupMinAndMax)
		{
			// Green
			ButtonColor = bIsSubCurveHidden ? Color(0, 32, 0) : Color::GREEN;
		}
		else
		{
			// Blue
			ButtonColor = bIsSubCurveHidden ? Color(0, 0, 32) : Color::BLUE;
		}
		break;
	case 3:
		// Dark green
		ButtonColor = bIsSubCurveHidden ? Color(0, 28, 0) : Color(0, 196, 0);
		break;
	case 4:
		// Blue
		ButtonColor = bIsSubCurveHidden ? Color(0, 0, 32) : Color::BLUE;
		break;
	case 5:
		// Dark blue
		ButtonColor = bIsSubCurveHidden ? Color(0, 0, 28) : Color(0, 0, 196);
		break;
	default:
		// A bad sub-curve index was given. 
		ASSERT(false);
		break;
	}

	return ButtonColor;
}

float DistributionVectorUniformCurve::GetKeyIn(Int32 KeyIndex)
{
	ASSERT(KeyIndex >= 0 && KeyIndex < constantCurve_.points_.Size());
	return constantCurve_.points_[KeyIndex].inVal_;
}

float DistributionVectorUniformCurve::GetKeyOut(Int32 SubIndex, Int32 KeyIndex)
{
	ASSERT((SubIndex >= 0) && (SubIndex < 6));
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));


	// Grab the value
	InterpCurvePoint<TwoVectors>	Point = constantCurve_.points_[KeyIndex];

	TwoVectors	Val = Point.outVal_;
	LockAndMirror(Val);
	if ((SubIndex % 2) == 0)
	{
		return Val.v1_[SubIndex / 2];
	}
	return Val.v2_[SubIndex / 2];
}

Color DistributionVectorUniformCurve::GetKeyColor(Int32 SubIndex, Int32 KeyIndex, const Color& CurveColor)
{
	ASSERT((SubIndex >= 0) && (SubIndex < 6));
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));

	if (SubIndex == 0)
	{
		return Color::RED;
	}
	else if (SubIndex == 1)
	{
		return Color(128, 0, 0);
	}
	else if (SubIndex == 2)
	{
		return Color::GREEN;
	}
	else if (SubIndex == 3)
	{
		return Color(0, 128, 0);
	}
	else if (SubIndex == 4)
	{
		return Color::BLUE;
	}
	else
	{
		return Color(0, 0, 128);
	}
}

void DistributionVectorUniformCurve::GetInRange(float& MinIn, float& MaxIn) const
{
	if (constantCurve_.points_.Size() == 0)
	{
		MinIn = 0.f;
		MaxIn = 0.f;
	}
	else
	{
		float Min = BIG_NUMBER;
		float Max = -BIG_NUMBER;
		for (Int32 Index = 0; Index < constantCurve_.points_.Size(); Index++)
		{
			float Value = constantCurve_.points_[Index].inVal_;
			if (Value < Min)
			{
				Min = Value;
			}
			if (Value > Max)
			{
				Max = Value;
			}
		}
		MinIn = Min;
		MaxIn = Max;
	}
}

void DistributionVectorUniformCurve::GetOutRange(float& MinOut, float& MaxOut) const
{
	TwoVectors	MinVec, MaxVec;

	constantCurve_.CalcBounds(MinVec, MaxVec, TwoVectors());
	LockAndMirror(MinVec);
	LockAndMirror(MaxVec);

	MinOut = Min<float>(MinVec.GetMin(), MaxVec.GetMin());
	MaxOut = Max<float>(MinVec.GetMax(), MaxVec.GetMax());
}

InterpCurveMode DistributionVectorUniformCurve::GetKeyInterpMode(Int32 KeyIndex) const
{
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));
	return constantCurve_.points_[KeyIndex].interpMode_;
}

void DistributionVectorUniformCurve::GetTangents(Int32 SubIndex, Int32 KeyIndex, float& ArriveTangent, float& LeaveTangent) const
{
	ASSERT((SubIndex >= 0) && (SubIndex < 6));
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));

	if (SubIndex == 0)
	{
		ArriveTangent = constantCurve_.points_[KeyIndex].arriveTangent_.v1_.x_;
		LeaveTangent = constantCurve_.points_[KeyIndex].leaveTangent_.v1_.x_;
	}
	else
		if (SubIndex == 1)
		{
			ArriveTangent = constantCurve_.points_[KeyIndex].arriveTangent_.v2_.x_;
			LeaveTangent = constantCurve_.points_[KeyIndex].leaveTangent_.v2_.x_;
		}
		else
			if (SubIndex == 2)
			{
				ArriveTangent = constantCurve_.points_[KeyIndex].arriveTangent_.v1_.y_;
				LeaveTangent = constantCurve_.points_[KeyIndex].leaveTangent_.v1_.y_;
			}
			else
				if (SubIndex == 3)
				{
					ArriveTangent = constantCurve_.points_[KeyIndex].arriveTangent_.v2_.y_;
					LeaveTangent = constantCurve_.points_[KeyIndex].leaveTangent_.v2_.y_;
				}
				else
					if (SubIndex == 4)
					{
						ArriveTangent = constantCurve_.points_[KeyIndex].arriveTangent_.v1_.z_;
						LeaveTangent = constantCurve_.points_[KeyIndex].leaveTangent_.v1_.z_;
					}
					else
						if (SubIndex == 5)
						{
							ArriveTangent = constantCurve_.points_[KeyIndex].arriveTangent_.v2_.z_;
							LeaveTangent = constantCurve_.points_[KeyIndex].leaveTangent_.v2_.z_;
						}
}

float DistributionVectorUniformCurve::EvalSub(Int32 SubIndex, float InVal)
{
	ASSERT((SubIndex >= 0) && (SubIndex < 6));

	TwoVectors Default;
	TwoVectors OutVal = constantCurve_.Eval(InVal, Default);
	LockAndMirror(OutVal);
	if ((SubIndex % 2) == 0)
	{
		return OutVal.v1_[SubIndex / 2];
	}
	else
	{
		return OutVal.v2_[SubIndex / 2];
	}
}

Int32 DistributionVectorUniformCurve::CreateNewKey(float KeyIn)
{
	TwoVectors NewKeyVal = constantCurve_.Eval(KeyIn, TwoVectors());
	Int32 NewPointIndex = constantCurve_.AddPoint(KeyIn, NewKeyVal);
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;

	return NewPointIndex;
}

void DistributionVectorUniformCurve::DeleteKey(Int32 KeyIndex)
{
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));
	constantCurve_.points_.Erase(KeyIndex);
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;
}

Int32 DistributionVectorUniformCurve::SetKeyIn(Int32 KeyIndex, float NewInVal)
{
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));
	Int32 NewPointIndex = constantCurve_.MovePoint(KeyIndex, NewInVal);
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;

	return NewPointIndex;
}

void DistributionVectorUniformCurve::SetKeyOut(Int32 SubIndex, Int32 KeyIndex, float NewOutVal)
{
	ASSERT((SubIndex >= 0) && (SubIndex < 6));
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));

	float Value;

	InterpCurvePoint<TwoVectors>* Point = &(constantCurve_.points_[KeyIndex]);
	ASSERT(Point);

	if (SubIndex == 0)
	{
		Value = Max<float>(NewOutVal, Point->outVal_.v2_.x_);
		Point->outVal_.v1_.x_ = Value;
	}
	else
		if (SubIndex == 1)
		{
			Value = Min<float>(NewOutVal, Point->outVal_.v1_.x_);
			Point->outVal_.v2_.x_ = Value;
		}
		else
			if (SubIndex == 2)
			{
				Value = Max<float>(NewOutVal, Point->outVal_.v2_.y_);
				Point->outVal_.v1_.y_ = Value;
			}
			else
				if (SubIndex == 3)
				{
					Value = Min<float>(NewOutVal, Point->outVal_.v1_.y_);
					Point->outVal_.v2_.y_ = Value;
				}
				else
					if (SubIndex == 4)
					{
						Value = Max<float>(NewOutVal, Point->outVal_.v2_.z_);
						Point->outVal_.v1_.z_ = Value;
					}
					else
						if (SubIndex == 5)
						{
							Value = Min<float>(NewOutVal, Point->outVal_.v1_.z_);
							Point->outVal_.v2_.z_ = Value;
						}

	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;
}

void DistributionVectorUniformCurve::SetKeyInterpMode(Int32 KeyIndex, InterpCurveMode NewMode)
{
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));

	constantCurve_.points_[KeyIndex].interpMode_ = NewMode;
	constantCurve_.AutoSetTangents(0.f);

	isDirty_ = true;
}

void DistributionVectorUniformCurve::SetTangents(Int32 SubIndex, Int32 KeyIndex, float ArriveTangent, float LeaveTangent)
{
	ASSERT((SubIndex >= 0) && (SubIndex < 6));
	ASSERT((KeyIndex >= 0) && (KeyIndex < constantCurve_.points_.Size()));

	if (SubIndex == 0)
	{
		constantCurve_.points_[KeyIndex].arriveTangent_.v1_.x_ = ArriveTangent;
		constantCurve_.points_[KeyIndex].leaveTangent_.v1_.x_ = LeaveTangent;
	}
	else
		if (SubIndex == 1)
		{
			constantCurve_.points_[KeyIndex].arriveTangent_.v2_.x_ = ArriveTangent;
			constantCurve_.points_[KeyIndex].leaveTangent_.v2_.x_ = LeaveTangent;
		}
		else
			if (SubIndex == 2)
			{
				constantCurve_.points_[KeyIndex].arriveTangent_.v1_.y_ = ArriveTangent;
				constantCurve_.points_[KeyIndex].leaveTangent_.v1_.y_ = LeaveTangent;
			}
			else
				if (SubIndex == 3)
				{
					constantCurve_.points_[KeyIndex].arriveTangent_.v2_.y_ = ArriveTangent;
					constantCurve_.points_[KeyIndex].leaveTangent_.v2_.y_ = LeaveTangent;
				}
				else
					if (SubIndex == 4)
					{
						constantCurve_.points_[KeyIndex].arriveTangent_.v1_.z_ = ArriveTangent;
						constantCurve_.points_[KeyIndex].leaveTangent_.v1_.z_ = LeaveTangent;
					}
					else
						if (SubIndex == 5)
						{
							constantCurve_.points_[KeyIndex].arriveTangent_.v2_.z_ = ArriveTangent;
							constantCurve_.points_[KeyIndex].leaveTangent_.v2_.z_ = LeaveTangent;
						}

	isDirty_ = true;
}



void DistributionVectorUniformCurve::LockAndMirror(TwoVectors& Val) const
{
	// Handle the mirror flags...
	for (Int32 i = 0; i < 3; i++)
	{
		switch (mirrorFlags_[i])
		{
		case EDVMF_Same:		Val.v2_[i] = Val.v1_[i];	break;
		case EDVMF_Mirror:		Val.v2_[i] = -Val.v1_[i];	break;
		}
	}

	// Handle the lock axes flags
	switch (lockedAxes_[0])
	{
	case EDVLF_XY:
		Val.v1_.y_ = Val.v1_.x_;
		break;
	case EDVLF_XZ:
		Val.v1_.z_ = Val.v1_.x_;
		break;
	case EDVLF_YZ:
		Val.v1_.z_ = Val.v1_.y_;
		break;
	case EDVLF_XYZ:
		Val.v1_.y_ = Val.v1_.x_;
		Val.v1_.z_ = Val.v1_.x_;
		break;
	}

	switch (lockedAxes_[0])
	{
	case EDVLF_XY:
		Val.v2_.y_ = Val.v2_.x_;
		break;
	case EDVLF_XZ:
		Val.v2_.z_ = Val.v2_.x_;
		break;
	case EDVLF_YZ:
		Val.v2_.z_ = Val.v2_.y_;
		break;
	case EDVLF_XYZ:
		Val.v2_.y_ = Val.v2_.x_;
		Val.v2_.z_ = Val.v2_.x_;
		break;
	}
}

// DistributionVector interface
void DistributionVectorUniformCurve::GetRange(Vector3& OutMin, Vector3& OutMax) const
{
	TwoVectors	MinVec, MaxVec;

	constantCurve_.CalcBounds(MinVec, MaxVec, TwoVectors());
	LockAndMirror(MinVec);
	LockAndMirror(MaxVec);

	if (MinVec.v1_.x_ < MaxVec.v1_.x_)	OutMin.x_ = MinVec.v1_.x_;
	else							OutMin.x_ = MaxVec.v1_.x_;
	if (MinVec.v1_.y_ < MaxVec.v1_.y_)	OutMin.y_ = MinVec.v1_.y_;
	else							OutMin.y_ = MaxVec.v1_.y_;
	if (MinVec.v1_.z_ < MaxVec.v1_.z_)	OutMin.z_ = MinVec.v1_.z_;
	else							OutMin.z_ = MaxVec.v1_.z_;

	if (MinVec.v2_.x_ > MaxVec.v2_.x_)	OutMax.x_ = MinVec.v2_.x_;
	else							OutMax.x_ = MaxVec.v2_.x_;
	if (MinVec.v2_.y_ > MaxVec.v2_.y_)	OutMax.y_ = MinVec.v2_.y_;
	else							OutMax.y_ = MaxVec.v2_.y_;
	if (MinVec.v2_.z_ > MaxVec.v2_.z_)	OutMax.z_ = MinVec.v2_.z_;
	else							OutMax.z_ = MaxVec.v2_.z_;
}


DistributionFloatParameterBase::DistributionFloatParameterBase()
{
	maxInput_ = 1.0f;
	maxOutput_ = 1.0f;
}

float DistributionFloatParameterBase::GetValue(float F, Object* Data, struct RandomStream* InRandomStream) const
{
	float ParamFloat = 0.f;
	bool bFoundParam = GetParamValue(Data, parameterName_, ParamFloat);
	if (!bFoundParam)
	{
		ParamFloat = constant_;
	}

	if (paramMode_ == DPM_Direct)
	{
		return ParamFloat;
	}
	else if (paramMode_ == DPM_Abs)
	{
		ParamFloat = Abs(ParamFloat);
	}

	float Gradient;
	if (maxInput_ <= minInput_)
		Gradient = 0.f;
	else
		Gradient = (maxOutput_ - minOutput_) / (maxInput_ - minInput_);

	float ClampedParam = Clamp(ParamFloat, minInput_, maxInput_);
	float Output = minOutput_ + ((ClampedParam - minInput_) * Gradient);

	return Output;
}


DistributionVectorParameterBase::DistributionVectorParameterBase()
{
	maxInput_ = Vector3(1.0f, 1.0f, 1.0f);
	maxOutput_ = Vector3(1.0f, 1.0f, 1.0f);

}

Vector3 DistributionVectorParameterBase::GetValue(float F, Object* Data, Int32 Extreme, struct RandomStream* InRandomStream) const
{
	Vector3 ParamVector(0.f, 0.f, 0.f);
	bool bFoundParam = GetParamValue(Data, parameterName_, ParamVector);
	if (!bFoundParam)
	{
		ParamVector = constant_;
	}

	if (paramModes_[0] == DPM_Abs)
	{
		ParamVector.x_ = Abs(ParamVector.x_);
	}

	if (paramModes_[1] == DPM_Abs)
	{
		ParamVector.y_ = Abs(ParamVector.y_);
	}

	if (paramModes_[2] == DPM_Abs)
	{
		ParamVector.z_ = Abs(ParamVector.z_);
	}

	Vector3 Gradient;
	if (maxInput_.x_ <= minInput_.x_)
		Gradient.x_ = 0.f;
	else
		Gradient.x_ = (maxOutput_.x_ - minOutput_.x_) / (maxInput_.x_ - minInput_.x_);

	if (maxInput_.y_ <= minInput_.y_)
		Gradient.y_ = 0.f;
	else
		Gradient.y_ = (maxOutput_.y_ - minOutput_.y_) / (maxInput_.y_ - minInput_.y_);

	if (maxInput_.z_ <= minInput_.z_)
		Gradient.z_ = 0.f;
	else
		Gradient.z_ = (maxOutput_.z_ - minOutput_.z_) / (maxInput_.z_ - minInput_.z_);

	Vector3 ClampedParam;
	ClampedParam.x_ = Clamp(ParamVector.x_, minInput_.x_, maxInput_.x_);
	ClampedParam.y_ = Clamp(ParamVector.y_, minInput_.y_, maxInput_.y_);
	ClampedParam.z_ = Clamp(ParamVector.z_, minInput_.z_, maxInput_.z_);

	Vector3 Output = minOutput_ + ((ClampedParam - minInput_) * Gradient);

	if (paramModes_[0] == DPM_Direct)
	{
		Output.x_ = ParamVector.x_;
	}

	if (paramModes_[1] == DPM_Direct)
	{
		Output.y_ = ParamVector.y_;
	}

	if (paramModes_[2] == DPM_Direct)
	{
		Output.z_ = ParamVector.z_;
	}

	return Output;
}


}
