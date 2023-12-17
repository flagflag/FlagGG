#include "ParticleModule.h"
#include "ParticleSystem/ParticleLODLevel.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleSystemComponent.h"
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
#include "Math/InterpCurveEdSetup.h"
#include "Log.h"

namespace FlagGG
{
	
/*-----------------------------------------------------------------------------
	ParticleModule implementation.
-----------------------------------------------------------------------------*/
ParticleModule::ParticleModule()
{
	supported3DDrawMode_ = false;
	_3DDrawMode_ = false;
	enabled_ = true;
	editable_ = true;
	LODDuplicate_ = true;
	supportsRandomSeed_ = false;
	requiresLoopingNotification_ = false;
	updateForGPUEmitter_ = false;
}

void ParticleModule::CompileModule(ParticleEmitterBuildInfo& emitterInfo)
{
	if (spawnModule_)
	{
		emitterInfo.spawnModules_.Push(this);
	}
}

void ParticleModule::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
}

void ParticleModule::Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime)
{
}


void ParticleModule::FinalUpdate(ParticleEmitterInstance* owner, Int32 offset, float deltaTime)
{
}

UInt32 ParticleModule::RequiredBytes(ParticleModuleTypeDataBase* typeData)
{
	return 0;
}

UInt32 ParticleModule::RequiredBytesPerInstance()
{
	return 0;
}

UInt32 ParticleModule::PrepPerInstanceBlock(ParticleEmitterInstance* owner, void* instData)
{
	return 0xffffffff;
}


void ParticleModule::SetToSensibleDefaults(ParticleEmitter* owner)
{
	// The default implementation does nothing...
}

void ParticleModule::GetCurveObjects(Vector<ParticleCurvePair>& outCurves)
{
	//for (TFieldIterator<FProperty> It(GetClass()); It; ++It)
	//{
	//	Object* Distribution = NULL;
	//	FProperty* Property = *It;
	//	ASSERT(Property != NULL);

	//	// attempt to get a distribution from a random struct property
	//	if (Property->IsA(FStructProperty::StaticClass()))
	//	{
	//		Distribution = FRawDistribution::TryGetDistributionObjectFromRawDistributionProperty((FStructProperty*)Property, (UInt8*)this);
	//	}
	//	else if (Property->IsA(FObjectPropertyBase::StaticClass()))
	//	{
	//		FObjectPropertyBase* ObjProperty = (FObjectPropertyBase*)(Property);
	//		if (ObjProperty && (ObjProperty->PropertyClass == DistributionFloat::StaticClass() ||
	//			ObjProperty->PropertyClass == DistributionVector::StaticClass()))
	//		{
	//			Distribution = ObjProperty->GetObjectPropertyValue(ObjProperty->ContainerPtrToValuePtr<void>(this));
	//		}
	//	}

	//	if (Distribution)
	//	{
	//		ParticleCurvePair* NewCurve = new(outCurves)ParticleCurvePair;
	//		ASSERT(NewCurve);
	//		NewCurve->CurveObject = Distribution;
	//		NewCurve->CurveName = It->GetName();
	//	}
	//}
}

bool ParticleModule::AddModuleCurvesToEditor(InterpCurveEdSetup* edSetup, PODVector<const CurveEdEntry*>& outCurveEntries)
{
	bool newCurve = false;
#if WITH_EDITORONLY_DATA
	Vector<ParticleCurvePair> OutCurves;
	GetCurveObjects(OutCurves);
	for (Int32 CurveIndex = 0; CurveIndex < OutCurves.Size(); CurveIndex++)
	{
		Object* Distribution = OutCurves[CurveIndex].curveObject_;
		if (Distribution)
		{
			CurveEdEntry* Curve = NULL;
			newCurve |= edSetup->AddCurveToCurrentTab(Distribution, OutCurves[CurveIndex].CurveName, ModuleEditorColor, &Curve, bCurvesAsColor, bCurvesAsColor);
			outCurveEntries.Add( Curve );		
		}
	}
#endif // WITH_EDITORONLY_DATA
	return newCurve;
}

void ParticleModule::RemoveModuleCurvesFromEditor(InterpCurveEdSetup* edSetup)
{
	Vector<ParticleCurvePair> OutCurves;
	GetCurveObjects(OutCurves);
	for (Int32 CurveIndex = 0; CurveIndex < OutCurves.Size(); CurveIndex++)
	{
		Object* Distribution = OutCurves[CurveIndex].curveObject_;
		if (Distribution)
		{
			edSetup->RemoveCurve(Distribution);
		}
	}
}

bool ParticleModule::ModuleHasCurves()
{
	Vector<ParticleCurvePair> Curves;
	GetCurveObjects(Curves);

	return (Curves.Size() > 0);
}

bool ParticleModule::IsDisplayedInCurveEd(InterpCurveEdSetup* edSetup)
{
	Vector<ParticleCurvePair> Curves;
	GetCurveObjects(Curves);

	for(Int32 i=0; i<Curves.Size(); i++)
	{
		if(edSetup->ShowingCurve(Curves[i].curveObject_))
		{
			return true;
		}	
	}

	return false;
}

void ParticleModule::ChangeEditorColor(Color& color, InterpCurveEdSetup* edSetup)
{
#if WITH_EDITORONLY_DATA
	ModuleEditorColor	= Color;

	Vector<ParticleCurvePair> Curves;
	GetCurveObjects(Curves);

	for (Int32 TabIndex = 0; TabIndex < edSetup->Tabs.Size(); TabIndex++)
	{
		FCurveEdTab*	Tab = &(edSetup->Tabs[TabIndex]);
		for (Int32 CurveIndex = 0; CurveIndex < Tab->Curves.Size(); CurveIndex++)
		{
			CurveEdEntry* Entry	= &(Tab->Curves[CurveIndex]);
			for (Int32 MyCurveIndex = 0; MyCurveIndex < Curves.Size(); MyCurveIndex++)
			{
				if (Curves[MyCurveIndex].curveObject_ == Entry->CurveObject)
				{
					Entry->CurveColor	= Color;
				}
			}
		}
	}
#endif // WITH_EDITORONLY_DATA
}

void ParticleModule::AutoPopulateInstanceProperties(ParticleSystemComponent* pSysComp)
{
	//ASSERT(IsInGameThread());
	//for (TFieldIterator<FStructProperty> It(GetClass()); It; ++It)
	//{
	//	// attempt to get a distribution from a random struct property
	//	Object* Distribution = FRawDistribution::TryGetDistributionObjectFromRawDistributionProperty(*It, (UInt8*)this);
	//	if (Distribution)
	//	{
	//		EParticleSysParamType ParamType	= PSPT_None;
	//		FName ParamName;

	//		// only handle particle param types
	//		DistributionFloatParticleParameter* DistFloatParam = Cast<DistributionFloatParticleParameter>(Distribution);
	//		DistributionVectorParticleParameter* DistVectorParam = Cast<DistributionVectorParticleParameter>(Distribution);
	//		if (DistFloatParam != NULL)
	//		{
	//			ParamType = PSPT_Scalar;
	//			ParamName = DistFloatParam->ParameterName;
	//			
	//		}
	//		else if (DistVectorParam != NULL)
	//		{
	//			ParamType = PSPT_Vector;
	//			ParamName = DistVectorParam->ParameterName;
	//		}

	//		if (ParamType != PSPT_None)
	//		{
	//			bool	bFound	= false;
	//			for (Int32 i = 0; i < PSysComp->InstanceParameters.Size(); i++)
	//			{
	//				FParticleSysParam* Param = &(PSysComp->InstanceParameters[i]);
	//				
	//				if (Param->Name == ParamName)
	//				{
	//					bFound	=	true;
	//					break;
	//				}
	//			}

	//			if (!bFound)
	//			{
	//				Int32 NewParamIndex = PSysComp->InstanceParameters.AddZeroed();
	//				PSysComp->InstanceParameters[NewParamIndex].Name		= ParamName;
	//				PSysComp->InstanceParameters[NewParamIndex].ParamType	= ParamType;
	//				PSysComp->InstanceParameters[NewParamIndex].Actor		= NULL;
	//				// Populate a Vector or Scalar using GetValue. (If we just call GetValue with no parameters we will get the default value based on the setting of the Parameter)
	//				switch (ParamType)
	//				{
	//				case PSPT_Vector:
	//					PSysComp->InstanceParameters[NewParamIndex].Vector = DistVectorParam->GetValue();
	//					PSysComp->InstanceParameters[NewParamIndex].Vector_Low = DistVectorParam->minOutput_;
	//					break;
	//				case PSPT_Scalar:
	//					PSysComp->InstanceParameters[NewParamIndex].Scalar = DistFloatParam->GetValue();
	//					PSysComp->InstanceParameters[NewParamIndex].Scalar_Low = DistFloatParam->minOutput_;
	//					break;
	//				}
	//			}
	//		}
	//	}
	//}

	//ParticleRandomSeedInfo* SeedInfo = GetRandomSeedInfo();
	//if (SeedInfo != NULL)
	//{
	//	if (SeedInfo->ParameterName != NAME_None)
	//	{
	//		bool	bFound	= false;
	//		for (Int32 ParamIdx = 0; ParamIdx < PSysComp->InstanceParameters.Size(); ParamIdx++)
	//		{
	//			FParticleSysParam* Param = &(PSysComp->InstanceParameters[ParamIdx]);
	//			if (Param->Name == SeedInfo->ParameterName)
	//			{
	//				bFound = true;
	//				break;
	//			}
	//		}

	//		if (bFound == false)
	//		{
	//			Int32 NewParamIndex = PSysComp->InstanceParameters.AddZeroed();
	//			PSysComp->InstanceParameters[NewParamIndex].Name = SeedInfo->ParameterName;
	//			PSysComp->InstanceParameters[NewParamIndex].ParamType = PSPT_Scalar;
	//		}
	//	}
	//}
}


bool ParticleModule::GenerateLODModuleValues(ParticleModule* sourceModule, float percentage, ParticleLODLevel* LODLevel)
{
	return true;
}


bool ParticleModule::ConvertFloatDistribution(DistributionFloat* floatDist, DistributionFloat* sourceFloatDist, float percentage)
{
	float multiplier = percentage / 100.0f;

	DistributionFloatConstant*				distConstant		= RTTICast<DistributionFloatConstant>(floatDist);
	DistributionFloatConstantCurve*		    distConstantCurve	= RTTICast<DistributionFloatConstantCurve>(floatDist);
	DistributionFloatUniform*				distUniform			= RTTICast<DistributionFloatUniform>(floatDist);
	DistributionFloatUniformCurve*			distUniformCurve	= RTTICast<DistributionFloatUniformCurve>(floatDist);
	DistributionFloatParticleParameter*	    distParticleParam	= RTTICast<DistributionFloatParticleParameter>(floatDist);

	if (distParticleParam)
	{
		distParticleParam->minOutput_	*= multiplier;
		distParticleParam->maxOutput_	*= multiplier;
	}
	else
	if (distUniformCurve)
	{
		for (Int32 keyIndex = 0; keyIndex < distUniformCurve->GetNumKeys(); keyIndex++)
		{
			for (Int32 subIndex = 0; subIndex < distUniformCurve->GetNumSubCurves(); subIndex++)
			{
				float Value = distUniformCurve->GetKeyOut(subIndex, keyIndex);
				distUniformCurve->SetKeyOut(subIndex, keyIndex, Value * multiplier);
			}
		}
	}
	else
	if (distConstant)
	{
		DistributionFloatConstant*	SourceConstant	= RTTICast<DistributionFloatConstant>(sourceFloatDist);
		ASSERT(SourceConstant);
		distConstant->SetKeyOut(0, 0, SourceConstant->constant_ * multiplier);
	}
	else
	if (distConstantCurve)
	{
		DistributionFloatConstantCurve* sourceConstantCurve	= RTTICast<DistributionFloatConstantCurve>(sourceFloatDist);
		ASSERT(sourceConstantCurve);

		for (Int32 keyIndex = 0; keyIndex < sourceConstantCurve->GetNumKeys(); keyIndex++)
		{
			distConstantCurve->CreateNewKey(sourceConstantCurve->GetKeyIn(keyIndex));
			for (Int32 subIndex = 0; subIndex < sourceConstantCurve->GetNumSubCurves(); subIndex++)
			{
				float value = sourceConstantCurve->GetKeyOut(subIndex, keyIndex);
				distConstantCurve->SetKeyOut(subIndex, keyIndex, value * multiplier);
			}
		}
	}
	else
	if (distUniform)
	{
		distUniform->SetKeyOut(0, 0, distUniform->min_ * multiplier);
		distUniform->SetKeyOut(1, 0, distUniform->max_ * multiplier);
	}
	else
	{
		FLAGGG_LOG_ERROR("ParticleModule::ConvertFloatDistribution> Invalid distribution?");
		return false;
	}

	// Safety catch to ensure that the distribution lookup tables get rebuilt...
	floatDist->isDirty_ = true;
	return true;
}


bool ParticleModule::ConvertVectorDistribution(DistributionVector* vectorDist, DistributionVector* sourceVectorDist, float percentage)
{
	float multiplier = percentage / 100.0f;

	DistributionVectorConstant*				distConstant		= RTTICast<DistributionVectorConstant>(vectorDist);
	DistributionVectorConstantCurve*		distConstantCurve	= RTTICast<DistributionVectorConstantCurve>(vectorDist);
	DistributionVectorUniform*				distUniform			= RTTICast<DistributionVectorUniform>(vectorDist);
	DistributionVectorUniformCurve*			distUniformCurve	= RTTICast<DistributionVectorUniformCurve>(vectorDist);
	DistributionVectorParticleParameter*	distParticleParam	= RTTICast<DistributionVectorParticleParameter>(vectorDist);

	if (distParticleParam)
	{
		distParticleParam->minOutput_.x_ *= multiplier;
		distParticleParam->minOutput_.y_ *= multiplier;
		distParticleParam->minOutput_.z_ *= multiplier;
		distParticleParam->maxOutput_.x_ *= multiplier;
		distParticleParam->maxOutput_.y_ *= multiplier;
		distParticleParam->maxOutput_.z_ *= multiplier;
	}
	else
	if (distUniformCurve)
	{
		for (Int32 keyIndex = 0; keyIndex < distUniformCurve->GetNumKeys(); keyIndex++)
		{
			for (Int32 subIndex = 0; subIndex < distUniformCurve->GetNumSubCurves(); subIndex++)
			{
				float	Value	= distUniformCurve->GetKeyOut(subIndex, keyIndex);
				distUniformCurve->SetKeyOut(subIndex, keyIndex, Value * multiplier);
			}
		}
	}
	else
	if (distConstant)
	{
		distConstant->constant_.x_ *= multiplier;
		distConstant->constant_.y_ *= multiplier;
		distConstant->constant_.z_ *= multiplier;
	}
	else
	if (distConstantCurve)
	{
		for (Int32 keyIndex = 0; keyIndex < distConstantCurve->GetNumKeys(); keyIndex++)
		{
			for (Int32 subIndex = 0; subIndex < distConstantCurve->GetNumSubCurves(); subIndex++)
			{
				float	Value	= distConstantCurve->GetKeyOut(subIndex, keyIndex);
				distConstantCurve->SetKeyOut(subIndex, keyIndex, Value * multiplier);
			}
		}
	}
	else
	if (distUniform)
	{
		distUniform->min_.x_	*= multiplier;
		distUniform->min_.y_ *= multiplier;
		distUniform->min_.z_ *= multiplier;
		distUniform->max_.x_	*= multiplier;
		distUniform->max_.y_ *= multiplier;
		distUniform->max_.z_ *= multiplier;
	}
	else
	{
		FLAGGG_LOG_ERROR("ParticleModule::ConvertVectorDistribution> Invalid distribution?");
		return false;
	}

	// Safety catch to ensure that the distribution lookup tables get rebuilt...
	vectorDist->isDirty_ = true;
	return true;
}

ParticleModule* ParticleModule::GenerateLODModule(ParticleLODLevel* sourceLODLevel, ParticleLODLevel* destLODLevel, float percentage, 
	bool generateModuleData, bool forceModuleConstruction)
{
	if (WillGeneratedModuleBeIdentical(sourceLODLevel, destLODLevel, percentage) && !forceModuleConstruction)
	{
		LODValidity_ |= (1 << destLODLevel->level_);
		return this;
	}

	// Otherwise, construct a new object and set the values appropriately... if required.
	ParticleModule* newModule = NULL;
	
	newModule = new ParticleModule();
	newModule->LODValidity_ = (1 << destLODLevel->level_);
	if (generateModuleData)
	{
		if (newModule->GenerateLODModuleValues(this, percentage, destLODLevel) == false)
		{
			FLAGGG_LOG_ERROR("GenerateFromLODLevel - Failed to generate LOD module values.");
			delete newModule;
			newModule = NULL;
		}
	}
	
	return newModule;
}


bool ParticleModule::IsUsedInLODLevel(Int32 sourceLODIndex) const
{
	if ((sourceLODIndex >= 0) && (sourceLODIndex <= 7))
	{
		return ((LODValidity_ & (1 << sourceLODIndex)) != 0);
	}
	return false;
}


void ParticleModule::GetParticleSysParamsUtilized(Vector<String>& particleSysParamList)
{
}
						   

void ParticleModule::GetParticleParametersUtilized(Vector<String>& particleParameterList)
{

}


UInt32 ParticleModule::PrepRandomSeedInstancePayload(ParticleEmitterInstance* owner, ParticleRandomSeedInstancePayload* inRandSeedPayload, const ParticleRandomSeedInfo& inRandSeedInfo)
{
    // These should never be null
	if (!CRY_ENSURE( owner != nullptr && owner->component_ != nullptr ))
	{
		return 0xffffffff;
	}

	if (inRandSeedPayload != nullptr)
	{
		new(inRandSeedPayload) ParticleRandomSeedInstancePayload();

		// See if the parameter is set on the instance...
		if (inRandSeedInfo.getSeedFromInstance_ == true)
		{
			float seedValue;
			if (owner->component_->GetFloatParameter(inRandSeedInfo.parameterName_, seedValue) == true)
			{
				if (inRandSeedInfo.instanceSeedIsIndex_ == false)
				{
					inRandSeedPayload->randomStream_.Initialize(RoundToInt(seedValue));
				}
				else
				{
					if (inRandSeedInfo.randomSeeds_.Size() > 0)
					{
						Int32 Index = Min<Int32>((inRandSeedInfo.randomSeeds_.Size() - 1), FloorToInt(seedValue));
						inRandSeedPayload->randomStream_.Initialize(inRandSeedInfo.randomSeeds_[Index]);
						return 0;
					}
					else
					{
						return 0xffffffff;
					}
				}
				return 0;
			}
		}

		// Pick a seed to use and initialize it!!!!
		if (inRandSeedInfo.randomSeeds_.Size() > 0)
		{
			Int32 Index = 0;

			if (inRandSeedInfo.randomlySelectSeedArray_)
			{
				Index = owner->component_->randomStream_.RandHelper(inRandSeedInfo.randomSeeds_.Size());
			}

			inRandSeedPayload->randomStream_.Initialize(inRandSeedInfo.randomSeeds_[Index]);
			return 0;
		}
	}
	return 0xffffffff;
}


bool ParticleModule::SetRandomSeedEntry(Int32 inIndex, Int32 inRandomSeed)
{
	ParticleRandomSeedInfo* SeedInfo = GetRandomSeedInfo();
	if (SeedInfo != NULL)
	{
		if (SeedInfo->randomSeeds_.Size() <= inIndex)
		{
			SeedInfo->randomSeeds_.Resize(inIndex + 1);
		}

		SeedInfo->randomSeeds_[inIndex] = inRandomSeed;
		return true;
	}
	return false;
}

bool ParticleModule::IsUsedInGPUEmitter() const
{
	return false;
}

RandomStream& ParticleModule::GetRandomStream(ParticleEmitterInstance* owner)
{
	ParticleRandomSeedInstancePayload* payload = owner->GetModuleRandomSeedInstanceData(this);
	RandomStream& randomStream = (payload != nullptr) ? payload->randomStream_ : owner->component_->randomStream_;
	return randomStream;
}

}
