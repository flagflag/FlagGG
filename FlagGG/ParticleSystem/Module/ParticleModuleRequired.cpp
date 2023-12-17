#include "ParticleSystem/Module/ParticleModuleRequired.h"
#include "Math/InterpCurveEdSetup.h"
#include "Math/Distributions/DistributionFloatConstant.h"

namespace FlagGG
{

/*-----------------------------------------------------------------------------
	ParticleModuleRequired implementation.
-----------------------------------------------------------------------------*/
ParticleModuleRequired::ParticleModuleRequired()
{
	screenAlignment_ = PSA_Square;
	spawnModule_ = true;
	updateModule_ = true;
	emitterDuration_ = 1.0f;
	emitterDurationLow_ = 0.0f;
	emitterDurationUseRange_ = false;
	emitterDelay_ = 0.0f;
	emitterDelayLow_ = 0.0f;
	emitterDelayUseRange_ = false;
	emitterLoops_ = 0;	
	subImages_Horizontal_ = 1;
	subImages_Vertical_ = 1;
	useMaxDrawCount_ = true;
	maxDrawCount_ = 500;
	LODDuplicate_ = true;
	normalsSphereCenter_ = Vector3(0.0f, 0.0f, 100.0f);
	normalsCylinderDirection_ = Vector3(0.0f, 0.0f, 1.0f);
	useLegacyEmitterTime_ = true;
	supportLargeWorldCoordinates_ = true;
	UVFlippingMode_ = ParticleUVFlipMode::None;
	boundingMode_ = BVC_EightVertices;
	alphaThreshold_ = 0.1f;
}

void ParticleModuleRequired::InitializeDefaults()
{
	if (!spawnRate_.IsCreated())
	{
		spawnRate_.distribution_ = MakeShared<DistributionFloatConstant>();
	}
}

void ParticleModuleRequired::SetToSensibleDefaults(ParticleEmitter* owner)
{
	Super::SetToSensibleDefaults(owner);
	useLegacyEmitterTime_ = false;
}

bool ParticleModuleRequired::GenerateLODModuleValues(ParticleModule* sourceModule, float percentage, ParticleLODLevel* LODLevel)
{
	// Convert the module values
	ParticleModuleRequired*	requiredSource	= RTTICast<ParticleModuleRequired>(sourceModule);
	if (!requiredSource)
	{
		return false;
	}

	bool bResult	= true;

	material_ = requiredSource->material_;
	screenAlignment_ = requiredSource->screenAlignment_;

	//bUseLocalSpace
	//bKillOnDeactivate
	//bKillOnCompleted
	//EmitterDuration
	//EmitterLoops
	//SpawnRate
	//InterpolationMethod
	//SubImages_Horizontal
	//SubImages_Vertical
	//bScaleUV
	//RandomImageTime
	//RandomImageChanges
	//SubUVDataOffset
	//EmitterRenderMode
	//EmitterEditorColor

	return bResult;
}

void ParticleModuleRequired::CacheDerivedData()
{
#if WITH_EDITORONLY_DATA
	const FString KeyString = FSubUVDerivedData::GetDDCKeyString(CutoutTexture->Source.GetId(), SubImages_Horizontal, SubImages_Vertical, (int32)BoundingMode, AlphaThreshold, (int32)OpacitySourceMode);
	TArray<uint8> Data;

	COOK_STAT(auto Timer = SubUVAnimationCookStats::UsageStats.TimeSyncWork());
	if (GetDerivedDataCacheRef().GetSynchronous(*KeyString, Data, GetPathName()))
	{
		COOK_STAT(Timer.AddHit(Data.Num()));
		DerivedData.BoundingGeometry.Empty(Data.Num() / sizeof(FVector2f));
		DerivedData.BoundingGeometry.AddUninitialized(Data.Num() / sizeof(FVector2f));
		FPlatformMemory::Memcpy(DerivedData.BoundingGeometry.GetData(), Data.GetData(), Data.Num() * Data.GetTypeSize());
	}
	else
	{
		DerivedData.Build(CutoutTexture, SubImages_Horizontal, SubImages_Vertical, BoundingMode, AlphaThreshold, OpacitySourceMode);

		Data.Empty(DerivedData.BoundingGeometry.Num() * sizeof(FVector2f));
		Data.AddUninitialized(DerivedData.BoundingGeometry.Num() * sizeof(FVector2f));
		FPlatformMemory::Memcpy(Data.GetData(), DerivedData.BoundingGeometry.GetData(), DerivedData.BoundingGeometry.Num() * DerivedData.BoundingGeometry.GetTypeSize());
		GetDerivedDataCacheRef().Put(*KeyString, Data, GetPathName());
		COOK_STAT(Timer.AddMiss(Data.Num()));
	}
#endif
}

void ParticleModuleRequired::InitBoundingGeometryBuffer()
{
	// The SRV is only needed for platforms that can render particles with instancing
	//if (boundingGeometryBuffer_->vertices_->Size())
	//{
	//	BeginInitResource(boundingGeometryBuffer_);
	//}
}

void ParticleModuleRequired::GetDefaultCutout()
{
#if WITH_EDITOR
	if (Material && GetDefault<URendererSettings>()->bDefaultParticleCutouts)
	{
		// Try to find an opacity mask texture to default to, if not try to find an opacity texture
		TArray<UTexture*> OpacityMaskTextures;
		Material->GetTexturesInPropertyChain(MP_OpacityMask, OpacityMaskTextures, nullptr, nullptr);

		if (OpacityMaskTextures.Num())
		{
			CutoutTexture = (UTexture2D*)OpacityMaskTextures[0];
		}
		else
		{
			TArray<UTexture*> OpacityTextures;
			Material->GetTexturesInPropertyChain(MP_Opacity, OpacityTextures, nullptr, nullptr);

			if (OpacityTextures.Num())
			{
				CutoutTexture = (UTexture2D*)OpacityTextures[0];
			}
		}
	}
#endif
}

}
