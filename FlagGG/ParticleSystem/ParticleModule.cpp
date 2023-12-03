#include "ParticleModule.h"
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
	//		DistributionFloatParticleParameter* DistFloatParam = DynamicCast<DistributionFloatParticleParameter>(Distribution);
	//		DistributionVectorParticleParameter* DistVectorParam = DynamicCast<DistributionVectorParticleParameter>(Distribution);
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

	DistributionFloatConstant*				distConstant		= Cast<DistributionFloatConstant>(floatDist);
	DistributionFloatConstantCurve*		    distConstantCurve	= Cast<DistributionFloatConstantCurve>(floatDist);
	DistributionFloatUniform*				distUniform			= Cast<DistributionFloatUniform>(floatDist);
	DistributionFloatUniformCurve*			distUniformCurve	= Cast<DistributionFloatUniformCurve>(floatDist);
	DistributionFloatParticleParameter*	    distParticleParam	= Cast<DistributionFloatParticleParameter>(floatDist);

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
		DistributionFloatConstant*	SourceConstant	= Cast<DistributionFloatConstant>(sourceFloatDist);
		ASSERT(SourceConstant);
		distConstant->SetKeyOut(0, 0, SourceConstant->constant_ * multiplier);
	}
	else
	if (distConstantCurve)
	{
		DistributionFloatConstantCurve* sourceConstantCurve	= Cast<DistributionFloatConstantCurve>(sourceFloatDist);
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

	DistributionVectorConstant*				distConstant		= Cast<DistributionVectorConstant>(vectorDist);
	DistributionVectorConstantCurve*		distConstantCurve	= Cast<DistributionVectorConstantCurve>(vectorDist);
	DistributionVectorUniform*				distUniform			= Cast<DistributionVectorUniform>(vectorDist);
	DistributionVectorUniformCurve*			distUniformCurve	= Cast<DistributionVectorUniformCurve>(vectorDist);
	DistributionVectorParticleParameter*	distParticleParam	= Cast<DistributionVectorParticleParameter>(vectorDist);

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
		LODValidity_ |= (1 << destLODLevel->Level);
		return this;
	}

	// Otherwise, construct a new object and set the values appropriately... if required.
	ParticleModule* NewModule = NULL;

	Object* DupObject = StaticDuplicateObject(this, GetOuter());
	if (DupObject)
	{
		NewModule = CastChecked<ParticleModule>(DupObject);
		NewModule->LODValidity = (1 << destLODLevel->Level);
		if (generateModuleData)
		{
			if (NewModule->GenerateLODModuleValues(this, percentage, destLODLevel) == false)
			{
				FString NameDump;
				GetName(NameDump);
				UE_LOG(LogParticles, Log, TEXT("ERROR - GenerateFromLODLevel - Failed to generate LOD module values for %s!"), *NameDump);
				NewModule = NULL;
			}
		}
	}
	
	return NewModule;
}


bool ParticleModule::IsUsedInLODLevel(Int32 SourceLODIndex) const
{
	if ((SourceLODIndex >= 0) && (SourceLODIndex <= 7))
	{
		return ((LODValidity & (1 << SourceLODIndex)) != 0);
	}
	return false;
}


void ParticleModule::GetParticleSysParamsUtilized(Vector<FString>& ParticleSysParamList)
{
}
						   

void ParticleModule::GetParticleParametersUtilized(Vector<FString>& ParticleParameterList)
{
	for (TFieldIterator<FStructProperty> It(GetClass()); It; ++It)
	{
		// attempt to get a distribution from a random struct property
		Object* Distribution = FRawDistribution::TryGetDistributionObjectFromRawDistributionProperty(*It, (UInt8*)this);
		if (Distribution)
		{
			DistributionFloatParticleParameter* FloatPP = Cast<DistributionFloatParticleParameter>(Distribution);
			DistributionVectorParticleParameter* VectorPP = Cast<DistributionVectorParticleParameter>(Distribution);

			// only handle particle param types
			if (FloatPP)
			{
				ParticleParameterList.Add(
					FString::Printf(
						TEXT("float : %32s - MinIn %10.5f, MaxIn %10.5f, MinOut %10.5f, MaxOut %10.5f, Mode %10s, Constant %10.5f\n"),
						*(FloatPP->ParameterName.ToString()),
						FloatPP->MinInput,
						FloatPP->MaxInput,
						FloatPP->minOutput_,
						FloatPP->maxOutput_,
						(FloatPP->ParamMode == DPM_Normal) ? TEXT("Normal") :
							(FloatPP->ParamMode == DPM_Abs) ? TEXT("Absolute") :
								(FloatPP->ParamMode == DPM_Direct) ? TEXT("Direct") :
									TEXT("????"),
						FloatPP->constant_)
						);
			}
			else 
			if (VectorPP)
			{
				FString ParamString;

				ParamString = FString::Printf(TEXT("VECTOR: %32s - "), *(VectorPP->ParameterName.ToString()));
				ParamString += FString::Printf(TEXT("MinIn %10.5f,%10.5f,%10.5f, "), 
					VectorPP->MinInput.x_, VectorPP->MinInput.y_, VectorPP->MinInput.z_);
				ParamString += FString::Printf(TEXT("MaxIn %10.5f,%10.5f,%10.5f, "),
					VectorPP->MaxInput.x_, VectorPP->MaxInput.y_, VectorPP->MaxInput.z_);
				ParamString += FString::Printf(TEXT("MinOut %10.5f,%10.5f,%10.5f, "),
						VectorPP->minOutput_.x_, VectorPP->minOutput_.y_, VectorPP->minOutput_.z_);
				ParamString += FString::Printf(TEXT("MaxOut %10.5f,%10.5f,%10.5f, "),
						VectorPP->maxOutput_.x_, VectorPP->maxOutput_.y_, VectorPP->maxOutput_.z_);
				ParamString += FString::Printf(TEXT("Mode %10s,%10s,%10s, "),
						(VectorPP->ParamModes[0] == DPM_Normal) ? TEXT("Normal") :
							(VectorPP->ParamModes[0] == DPM_Abs) ? TEXT("Absolute") :
								(VectorPP->ParamModes[0] == DPM_Direct) ? TEXT("Direct") :
									TEXT("????"),
						(VectorPP->ParamModes[1] == DPM_Normal) ? TEXT("Normal") :
							(VectorPP->ParamModes[1] == DPM_Abs) ? TEXT("Absolute") :
								(VectorPP->ParamModes[1] == DPM_Direct) ? TEXT("Direct") :
									TEXT("????"),
						(VectorPP->ParamModes[2] == DPM_Normal) ? TEXT("Normal") :
							(VectorPP->ParamModes[2] == DPM_Abs) ? TEXT("Absolute") :
								(VectorPP->ParamModes[2] == DPM_Direct) ? TEXT("Direct") :
									TEXT("????"));
				ParamString += FString::Printf(TEXT("Constant %10.5f,%10.5f,%10.5f\n"),
						VectorPP->constant_.x_, VectorPP->constant_.y_, VectorPP->constant_.z_);
				ParticleParameterList.Add(ParamString);
			}
		}
	}
}


UInt32 ParticleModule::PrepRandomSeedInstancePayload(ParticleEmitterInstance* owner, ParticleRandomSeedInstancePayload* InRandSeedPayload, const ParticleRandomSeedInfo& InRandSeedInfo)
{
    // These should never be null
	if (!ensure( owner != nullptr && owner->Component != nullptr ))
	{
		return 0xffffffff;
	}

	if (InRandSeedPayload != nullptr)
	{
		new(InRandSeedPayload) ParticleRandomSeedInstancePayload();

		// See if the parameter is set on the instance...
		if (InRandSeedInfo.bGetSeedFromInstance == true)
		{
			float SeedValue;
			if (owner->Component->GetFloatParameter(InRandSeedInfo.ParameterName, SeedValue) == true)
			{
				if (InRandSeedInfo.bInstanceSeedIsIndex == false)
				{
					InRandSeedPayload->RandomStream.Initialize(FMath::RoundToInt(SeedValue));
				}
				else
				{
					if (InRandSeedInfo.RandomSeeds.Size() > 0)
					{
						Int32 Index = FMath::Min<Int32>((InRandSeedInfo.RandomSeeds.Size() - 1), FMath::TruncToInt(SeedValue));
						InRandSeedPayload->RandomStream.Initialize(InRandSeedInfo.RandomSeeds[Index]);
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
		if (InRandSeedInfo.RandomSeeds.Size() > 0)
		{
			Int32 Index = 0;

			if (InRandSeedInfo.bRandomlySelectSeedArray)
			{
				Index = owner->Component->RandomStream.RandHelper(InRandSeedInfo.RandomSeeds.Size());
			}

			InRandSeedPayload->RandomStream.Initialize(InRandSeedInfo.RandomSeeds[Index]);
			return 0;
		}
		else if (FApp::bUseFixedSeed)
		{
			InRandSeedPayload->RandomStream.Initialize(GetFName());
			return 0;
		}
	}
	return 0xffffffff;
}


bool ParticleModule::SetRandomSeedEntry(Int32 InIndex, Int32 InRandomSeed)
{
	ParticleRandomSeedInfo* SeedInfo = GetRandomSeedInfo();
	if (SeedInfo != NULL)
	{
		if (SeedInfo->RandomSeeds.Size() <= InIndex)
		{
			SeedInfo->RandomSeeds.AddZeroed(InIndex - SeedInfo->RandomSeeds.Size() + 1);
		}

		SeedInfo->RandomSeeds[InIndex] = InRandomSeed;
		return true;
	}
	return false;
}

bool ParticleModule::IsUsedInGPUEmitter()const
{
	ParticleSystem* Sys = Cast<ParticleSystem>(GetOuter());

	if( Sys )
	{
		for( Int32 EmitterIdx=0 ; EmitterIdx < Sys->Emitters.Size() ; ++EmitterIdx )
		{
			ParticleEmitter* Emitter = Sys->Emitters[EmitterIdx];
			if( Emitter && Emitter->LODLevels.Size() )
			{
				//Have to make sure this module is used in this emitter before checking it's type data.
				bool bUsedInThisEmitter = false;
				for( Int32 LodIdx=0 ; LodIdx < Emitter->LODLevels.Size() && !bUsedInThisEmitter ; ++LodIdx )
				{
					ParticleLODLevel* LODLevel = Emitter->LODLevels[LodIdx];
					if( LODLevel )
					{
						if( LODLevel->RequiredModule == this )
						{
							bUsedInThisEmitter = true;
						}
						else
						{
							for( Int32 ModuleIdx=0 ; ModuleIdx < LODLevel->Modules.Size() && !bUsedInThisEmitter ; ++ModuleIdx )
							{
								ParticleModule* Module = LODLevel->Modules[ModuleIdx];
								if( Module == this )
								{
									bUsedInThisEmitter = true;
								}
							}
						}
					}
				}

				//If this module is used in this emitter then ASSERT it's type data and return whether it's GPU or not
				if( bUsedInThisEmitter )
				{
					//Can just ASSERT the highest lod.
					ParticleLODLevel* LODLevel = Emitter->LODLevels[0];
					if( LODLevel )
					{
						ParticleModule* TypeDataModule = LODLevel->TypeDataModule;
						if( TypeDataModule && TypeDataModule->IsA(UParticleModuleTypeDataGpu::StaticClass()) )
						{
							return true;//Module is used in a GPU emitter.
						}
					}
				}
			}
		}
	}

	return false;
}

RandomStream& ParticleModule::GetRandomStream(ParticleEmitterInstance* owner)
{
	ParticleRandomSeedInstancePayload* Payload = owner->GetModuleRandomSeedInstanceData(this);
	RandomStream& RandomStream = (Payload != nullptr) ? Payload->RandomStream : owner->Component->RandomStream;
	return RandomStream;
}

}
